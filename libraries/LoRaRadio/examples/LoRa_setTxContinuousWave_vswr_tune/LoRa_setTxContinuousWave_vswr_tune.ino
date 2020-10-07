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

#define VSWR_EN PB5
#define VSWR_ADC PA3

float freq_start  = 868000000;
float freq  = 0;

float tune_start  = 0;
float tune_stop   = 0b111111;
float tune_step   = 1;
float tune=0;


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


void setup( void )
{
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
    tune=tune_start;


}

void loop( void )
{

  delay(1000);
  DTC_Initialize(STM32L0_GPIO_PIN_PB12, tune, STM32L0_GPIO_PIN_NONE, 0b0);
  if(tune>tune_stop){
    tune=tune_start;
    STM32L0.stop();
    }
  LoRaWAN.setTxContinuousWave(freq, (int8_t) 5, 100);
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
  Serial.print(tune);
  Serial.print(",");
  Serial.println(avg/count);
  tune+=tune_step;

}
