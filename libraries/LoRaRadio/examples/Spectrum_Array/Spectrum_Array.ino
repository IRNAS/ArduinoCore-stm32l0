#include "LoRaRadio.h"
#include "STM32L0.h"

#define NUM_FREQ 1000
int16_t rssi_values[NUM_FREQ];
float frequencies[NUM_FREQ];
uint16_t freq_count = 0;

void setup( void )
{
    Serial.begin(500000);

    while (!Serial) { }

    LoRaRadio.begin(868000000);

    LoRaRadio.setFrequency(868000000);
    LoRaRadio.setTxPower(14);
    LoRaRadio.setBandwidth(LoRaRadio.BW_125);
    LoRaRadio.setSpreadingFactor(LoRaRadio.SF_7);
    LoRaRadio.setCodingRate(LoRaRadio.CR_4_5);
    LoRaRadio.setLnaBoost(true);
    
    //pinMode(PB6, OUTPUT);
    //digitalWrite(PB6, LOW);
    //pinMode(PB5, OUTPUT);
    //digitalWrite(PB5, LOW);
    
    char uart_buffer[100];
    bool received_token = false;
    float num_f = 0.0;
    float max_freq = 0.0;
    float min_freq = 0.0;

    Serial.println("AWAKE");
    while(!received_token)
    {
        Serial.println("READY_TOKEN");
        //delay(100);
        //digitalWrite(PB6, HIGH);
        delay(1);
        //digitalWrite(PB6, LOW);

        if (Serial.available()) 
        {      
            Serial.readBytesUntil('\n', uart_buffer, 100);

            if(memcmp(uart_buffer, "START_TOKEN", 11) == 0)
            {
                received_token = true;
            }
            else if(memcmp(uart_buffer, "RESET", 5) == 0)
            {
                STM32L0.reset();
            }
            else 
            {
                if(max_freq == 0.0)
                {
                    max_freq = strtof(uart_buffer, NULL);
                    Serial.println("OK");
                    digitalWrite(PB6, HIGH);
                }
                else if(min_freq == 0.0)
                {
                    min_freq = strtof(uart_buffer, NULL);
                    Serial.println("OK");
                    digitalWrite(PB5, HIGH);
                }
                else
                {
                    //We got a number
                    num_f = strtof(uart_buffer, NULL);
                    if((min_freq <= num_f) && (num_f <= max_freq))
                    {
                        frequencies[freq_count++] = num_f;
                        Serial.println("OK");
                    }
                    else
                    {
                        Serial.println("NOT OK");
                    }
                }
            }
            // Clean buffer
            memset(uart_buffer, 0, 100);
        }
    }

    Serial.println("END OF SETUP");
}

void loop( void )
{
    char uart_buffer[100];
    bool received_token = false;

    while(!received_token)
    {
        Serial.println("READY_TOKEN");
        delay(1);

        if (Serial.available()) 
        {      
            Serial.readBytesUntil('\n', uart_buffer, 100);

            if(memcmp(uart_buffer, "START_TOKEN", 11) == 0)
            {
                received_token = true;
            }
            else if(memcmp(uart_buffer, "RESET", 5) == 0)
            {
                STM32L0.reset();
            }
            // Clean buffer
            memset(uart_buffer, 0, 100);
        }
    }

    LoRaRadio.readSpectrumArray(frequencies, rssi_values, freq_count);
       
    Serial.println("PRINT_TOKEN"); 
    for(uint16_t i = 0; i < freq_count; i++)
    {
        Serial.print(frequencies[i]);
        Serial.print(",");
        Serial.println(rssi_values[i]);
    }

    Serial.println("DONE_TOKEN"); 
}

