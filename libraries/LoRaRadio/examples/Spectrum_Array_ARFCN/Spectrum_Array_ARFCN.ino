#include "LoRaRadio.h"
// Provide sketch with ARFCN channel numbers and you will automatically get
// frequency numbers.
#define NUM_ARFCN = 5
int16_t rssi_values[NUM_ARFCN * 2];
float frequencies[NUM_ARFCN * 2];
uint16_t ARFCN_arr[NUM_ARFCN] = {0,1,2,3,4,5}

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

    for(int i = 0; i<NUM_ARFCN; i++)
    {
        float f_up = 890.0 + 0.2 * ARFCN_arr[i] ;
        float f_down = f_up + 45;
        frequencies[i] = f_up;
        frequencies[i+1] = f_up;
        Serial.println(frequencies[i]);
        Serial.println(frequencies[i+1]);
    }
    while(1)
    {}
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
            }
            // Clean buffer
            memset(uart_buffer, 0, 100);
        }
    }

    LoRaRadio.readSpectrumArray(frequencies, rssi_values, NUM_FREQ);
       
    Serial.println("PRINT_TOKEN"); 
    for(uint16_t i = 0; i < NUM_FREQ; i++)
    {
        Serial.print(frequencies[i]);
        Serial.print(",");
        Serial.println(rssi_values[i]);
    }

    Serial.println("DONE_TOKEN"); 
}

