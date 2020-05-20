#include "LoRaRadio.h"

/*
 * This sketch will print out time needed for spectrum scan and number of 
 * frequencies that were scanned.
 */


#define START_FREQ_MHZ  800
#define END_FREQ_MHZ    892
#define FREQ_JUMP_MHZ   0.1f
#define BUF_LEN         1000

int16_t buf[BUF_LEN];

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
    
    pinMode(PB6, OUTPUT);
}

void loop( void )
{
    uint32_t start = millis();
    uint16_t num_points = LoRaRadio.readSpectrum(START_FREQ_MHZ, 
                                                 END_FREQ_MHZ, 
                                                 FREQ_JUMP_MHZ, 
                                                 buf, 
                                                 BUF_LEN);
       
    uint32_t end = millis();

    if(num_points == 0)
    {
        Serial.println("Buffer too small!"); 
    } 

    memset(buf, 0, BUF_LEN);
    
    Serial.println("----------------------------------------");
    Serial.print("Frequency range in MHz: ");
    Serial.print(START_FREQ_MHZ);
    Serial.print(" - ");
    Serial.println(END_FREQ_MHZ);
    Serial.print("Frequency jump in KHz: ");
    Serial.println(FREQ_JUMP_MHZ * 1000);
    Serial.print("Number of scanned frequency points: ");
    Serial.println(num_points);
    Serial.print("Time needed to scan whole spectrum: ");
    Serial.print(end - start);
    Serial.println(" ms");
    Serial.print("Time needed to scan one frequency: ");
    float one_freq_time = (end - start) / (float)num_points;
    Serial.print(one_freq_time);
    Serial.println(" ms");
    Serial.println("----------------------------------------");
    Serial.println();
    
    delay(5000);
}
