#include "LoRaRadio.h"

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
}

void loop( void )
{
    Serial.println(LoRaRadio.readRssi(868000000));
    delay(100);
}
