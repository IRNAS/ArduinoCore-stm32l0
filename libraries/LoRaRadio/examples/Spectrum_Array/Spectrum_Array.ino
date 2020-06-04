#include "LoRaRadio.h"

#define NUM_FREQ 1000
int16_t rssi_values[NUM_FREQ];
float frequencies[NUM_FREQ];
uint16_t freq_count = 0;

void setup( void )
{
    Serial.begin(115200);
    
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
                //Serial.println("Starting spectrum scan");
                received_token = true;
            }
            else 
            {
                //We got a number
                num_f = strtof(uart_buffer, NULL);
                if((200.0 < num_f) && (num_f < 2000.0))
                {
                    frequencies[freq_count++] = num_f;
                    Serial.println("OK");
                }
                else
                {
                    Serial.println("NOT OK");
                }
            }
            // Clean buffer
            memset(uart_buffer, 0, 100);
        }
    }

    //Serial.print("freq_count is: ");
    //Serial.println(freq_count);
    //for(int i = 0; i < freq_count; i++)
    //{
        //Serial.println(frequencies[i]);
    //}

    //while(1)
    //{}
}

void loop( void )
{
    char uart_buffer[100];
    bool received_token = false;

    while(!received_token)
    {
        Serial.println("READY_TOKEN");
        //digitalWrite(PB6, HIGH);
        delay(1);
        //digitalWrite(PB6, LOW);

        if (Serial.available()) 
        {      
            Serial.readBytesUntil('\n', uart_buffer, 100);

            if(memcmp(uart_buffer, "START_TOKEN", 11) == 0)
            {
                //Serial.println("Starting spectrum scan");
                received_token = true;
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

