/* Simple Ping-Pong with carrier sense for a LoRa Radio/Modem
 *
 * In setup() below please adjust your country specific frequency ranges,
 * as well as the Bandwidth/SpreadingFactor/CodingRate settings.
 *
 * They way this example works is that the device first listens for 5000ms.
 * If it received a "PING" message, it considers itself a SLAVE. If not
 * it considers itself a MASTER. A SLAVE waits for an incoming "PING" message,
 * which it answers with a "PONG" message. A MASTER simply sends periodically
 * every 1000ms) a "PING" message, and collects "PONG" replies while waiting.
 *
 * In this variant the code checks whether a channel is free before sending data.
 *    
 *    
 * This example code is in the public domain.
 */

 // Direct output at txp 5dBm = 2380
 

#include "LoRaWAN.h"
#include "STM32L0.h"
#include "TimerMillis.h"
#include "wiring_private.h"

TimerMillis timerOff;
TimerMillis timerOn;

#define debug
#define serial_debug  Serial

#define VSWR_EN PB5
#define VSWR_ADC PA3

float freq_start  = 840000000;
float freq_stop   = 880000000;
float freq=0;



void callbackOff(void)
{
    digitalWrite(LED_BUILTIN, 0);
}

void callbackOn(void)
{
      int sensorValue = analogRead(VSWR_ADC);
  // print out the value you read:
  Serial.print(freq);
  Serial.print(",");
  Serial.println(sensorValue);
}

const char *devAddr = "260119A7";
const char *nwkSKey = "28A6AA276537A2DB9ABD7ECB063F44C3";
const char *appSKey = "C9B2B9F66CDB33015ADFADE7EC5617C4";


boolean scan_vswr(uint32_t start, uint32_t stop, int8_t power, uint16_t samples, uint32_t time, uint8_t *output)
{
    uint32_t freq = start;
    uint32_t step = (stop-start)/samples;

    uint32_t measurement_sum = 0;
    uint16_t measurement_count = 0;

    if(step==0 | start>=stop){
        return false;
    }

    for(uint16_t i;i<samples;i++){
        #ifdef debug
            serial_debug.print("Scan( "); serial_debug.print(freq); serial_debug.print(" Hz ");
        #endif
        // limit time to maximal value
        if(time>4000){
            time=4000;
        }
        delay(1000);
        LoRaWAN.setTxContinuousWave(freq,15,100);
        unsigned long start_time=millis();
        unsigned long elapsed=millis()-start_time;
        float avg=0;
        float count = 0;
        while(elapsed<110){
            elapsed=millis()-start_time;
            int adc=analogRead(VSWR_ADC);
            if(adc>0){
            avg+=adc;
            count++;
            }
        }


        double value = avg/count; // calculate average and convert to volts
        double dbm=-31; // covnert to dBm
        // secure against division by zero
        if(value!=0){
            dbm = 10*log10(value)-27;
        }
        #ifdef debug
            serial_debug.print(value); 
            serial_debug.print(" ");
            serial_debug.print(dbm); 
            serial_debug.println(" dBm) ");
        #endif
        // create an 8 bit datapoint use dBm=result/10-30 on the server side
        uint8_t result = (dbm+31)*10;
        *output=result;
        output++;
        //increment frequency
        freq+=step;
        if(freq>stop){
            break;
        }
    }
    return true;
}

void setup( void )
{
    DTC_Initialize(STM32L0_GPIO_PIN_PB12, 4, STM32L0_GPIO_PIN_NONE, 0b0);
    analogReadResolution(12);
    LoRaWAN.begin(EU868);
    LoRaWAN.setDutyCycle(false);
    LoRaWAN.setTxPower(20);    
    LoRaWAN.setDataRate(5);
    LoRaWAN.joinABP(devAddr, nwkSKey, appSKey); 
    Serial.begin(115200);
    pinMode(VSWR_EN,OUTPUT);
    digitalWrite(VSWR_EN,HIGH);
    freq=freq_start;

}

void loop( void )
{

  LoRaWAN.beginPacket();
  LoRaWAN.write(0xef);
  LoRaWAN.write(0xbe);
  LoRaWAN.write(0xad);
  LoRaWAN.write(0xde);
  LoRaWAN.endPacket();
  delay(4000);
  uint8_t output[100];
  scan_vswr(freq_start, freq_stop, 5, 40, 100, &output[0]);

  /*delay(1000);
  freq+=freq_step;
  if(freq>freq_stop){
    freq=freq_start;
    STM32L0.stop();
    }
  LoRaWAN.setTxContinuousWave(freq,15,100);
  unsigned long start_time=millis();
  unsigned long elapsed=millis()-start_time;
  float avg=0;
  float count = 0;
  while(elapsed<110){
    elapsed=millis()-start_time;
    int adc=analogRead(VSWR_ADC);
    if(adc>0){
      avg+=adc;
      count++;
    }
  }
  Serial.print(freq);
  Serial.print(",");
  Serial.print(count);
  Serial.print(",");
  Serial.println(avg/count);*/

}
