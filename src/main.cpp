#include <Arduino.h>
#include <Wire.h>
#include "NodeManager.h"
#include "LoRaWAN.h"
#include "RN2483_Modem.h"
#include "device_settings.h"
#include "DebugSerial.h"

NodeManager nm(MOTHERBOARD_ID);

LoRaSettings_t settings = LORA_INIT_MY_DEVICE;

uint8_t buf[16];

void setup() {
  delay(5000);
  DEBUG.begin();
  // USB Serial for configuration
  SerialAT.begin(115200); 
  
  // configure lora (no networking yet)
  lora.begin(settings);

  // put your setup code here, to run once:
  nm.begin();
  nm.runConfigMode(); // run config
  //nm.runConfigMode(true); // run config forever

  // join lora network
  //lora.join();
}

void loop() {
  // put your main code here, to run repeatedly:
  nm.loop();

  // aggregate by using payload size (return value of NodeManager::payloadAvailable())
  uint8_t pSize = nm.payloadAvailable();
  if(pSize){
    DEBUG.println("\nMAIN APP: Data available!");
    if(nm.getLoraPayload(buf, 16)){
      DEBUG.print("Payload: ");
      for(uint8_t i=0; i<pSize; i++){
        if(buf[i]<16){
          DEBUG.print("0");
        }
        DEBUG.print(buf[i], HEX);
      }
      DEBUG.println("\n");
    }
    else{
      DEBUG.println("Empty payload.\n");
    }
  }

  // TODO: make RN2483 sleep as well?
  // Handle errors due to channel not free
  //lora.sendData(testdata, 4);
  //testdata[3]++;
}