#include <Arduino.h>
#include <Wire.h>
#include "NodeManager.h"
#include "LoRaWAN.h"
#include "device_settings.h"
#include "DebugSerial.h"

NodeManager nm(MOTHERBOARD_ID);
LoRaSettings_t settings = LORA_INIT_MY_DEVICE;

uint8_t buf[LORA_MAX_PAYLOAD_SIZE];

void setup(){
  // start serial interface for printing debug messages
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
  lora.join();

  // put lora modem in sleep (save energy)
  lora.sleep();
}

void loop() {
  // put your main code here, to run repeatedly:
  nm.loop();

  // aggregate by using payload size (return value of NodeManager::payloadAvailable())
  uint8_t pSize = nm.payloadAvailable();

#ifdef LORA_AGGREGATE_THRESHOLD
  if(pSize > LORA_AGGREGATE_THRESHOLD){
#else
  if(pSize > 0){
#endif
    DEBUG.println("\nMAIN APP: Data available!");
    if(nm.getLoraPayload(buf, LORA_MAX_PAYLOAD_SIZE)){
      DEBUG.print("Payload: ");
      for(uint8_t i=0; i<pSize; i++){
        if(buf[i] < 0x10){
          DEBUG.print("0");
        }
        DEBUG.print(buf[i], HEX);
      }
      DEBUG.println("\n");

      lora.wake();
      lora.sendData(buf, pSize);
      lora.sleep();
    }
    else{
      DEBUG.println("Empty payload.\n");
    }
  }

}