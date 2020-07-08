#include "LoRaWAN.h"

// #define REGION_AS923_920_923 /* Japan, Malaysia, Singapore */
// #define REGION_AS923_923_925 /* Brunei, Cambodia, Hong Kong, Indonesia, Laos, Taiwan, Thailand, Vietnam */
// #define REGION_AU915
#define REGION_EU868
// #define REGION_IN865
// #define REGION_KR920
// #define REGION_US915

const char *devAddr = "26011920";
const char *nwkSKey = "0A7E92D44B3648E762F25457A0397E8E";
const char *appSKey = "9CD0F7344C2A551882208D66E078F6C2";

void setup( void )
{
    Serial.begin(115200);
    pinMode(PB2, INPUT);



    LoRaWAN.begin(EU868);
    LoRaWAN.setRX2Channel(869525000, 3);

    //Data rate 3 equals to SF9 and BW125
    LoRaWAN.setDataRate(3);
    //LoRaWAN.setDataRate(0);
    LoRaWAN.setTxPower(1);
    LoRaWAN.setPublicNetwork(true);

    LoRaWAN.setDutyCycle(false);     
    // LoRaWAN.setAntennaGain(2.0);
    LoRaWAN.joinABP(devAddr, nwkSKey, appSKey);


    Serial.println("JOIN( )");
}
uint8_t message_len = 8;
const uint8_t payload[8] = {0x0d, 0x0e, 0x0a, 0x0d, 0x0b, 0x0e, 0x0e, 0x0f};
void loop( void )
{
    if (LoRaWAN.joined() && !LoRaWAN.busy() && !digitalRead(PB2))
    {
        Serial.print("TRANSMIT( ");
        Serial.print("TimeOnAir: ");
        Serial.print(LoRaWAN.getTimeOnAir());
        Serial.print(", NextTxTime: ");
        Serial.print(LoRaWAN.getNextTxTime());
        Serial.print(", MaxPayloadSize: ");
        Serial.print(LoRaWAN.getMaxPayloadSize());
        Serial.print(", DR: ");
        Serial.print(LoRaWAN.getDataRate());
        Serial.print(", TxPower: ");
        Serial.print(LoRaWAN.getTxPower(), 1);
        Serial.print("dbm, UpLinkCounter: ");
        Serial.print(LoRaWAN.getUpLinkCounter());
        Serial.print(", DownLinkCounter: ");
        Serial.print(LoRaWAN.getDownLinkCounter());
        Serial.println(" )");

        LoRaWAN.sendPacket(payload, message_len);
    }
}
