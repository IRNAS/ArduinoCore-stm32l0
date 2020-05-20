#include "LoRaRadio.h"

#define START_FREQ_MHZ  870
#define END_FREQ_MHZ    892
#define FREQ_JUMP_MHZ   0.1f
#define BUF_LEN         1000

int16_t buf[BUF_LEN];

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
    
    pinMode(PB6, OUTPUT);
}

void loop( void )
{
    char uart_buffer[100];
    bool received_token = false;

    while(!received_token)
    {
        Serial.println("READY_TOKEN");
        digitalWrite(PB6, HIGH);
        delay(1);
        digitalWrite(PB6, LOW);

        if (Serial.available()) 
        {      
            Serial.readBytesUntil('\n', uart_buffer, 100);

            if(memcmp(uart_buffer, "START_TOKEN", 11) == 0)
            {
                //Serial.println("Starting spectrum scan");
                received_token = true;
                Serial.print(START_FREQ_MHZ);
                Serial.print(",");
                Serial.print(END_FREQ_MHZ);
                Serial.print(",");
                Serial.println("0");
            }
            // Clean buffer
            memset(uart_buffer, 0, 100);
        }
    }



    uint16_t num_points = LoRaRadio.readSpectrum(START_FREQ_MHZ, 
                                                 END_FREQ_MHZ, 
                                                 FREQ_JUMP_MHZ, 
                                                 buf, 
                                                 BUF_LEN);
    //Serial.println("Spectrum done"); 
       
    if(num_points == 0)
    {
        Serial.println("Buffer too small"); 
    } 

    for(uint16_t i = 0; i < num_points; i++)
    {
        float freq = START_FREQ_MHZ + i * FREQ_JUMP_MHZ;
        Serial.print(freq);
        Serial.print(",");
        Serial.println(buf[i]); 
    }

    Serial.println("DONE_TOKEN"); 

    memset(buf, 0, BUF_LEN);
}
