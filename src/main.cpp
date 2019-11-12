#include <Arduino.h>
#include <Wire.h>
#include "NodeManager.h"
#include "LoRaWAN.h"
#include "RN2483_Modem.h"

#include "device_settings.h"

NodeManager nm(MOTHERBOARD_ID);

LoRaSettings_t settings = LORA_INIT_MY_DEVICE;

uint8_t testdata[4] = {0x47, 0x4f, 00, 00};

void setup() {
  delay(5000);
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
}

void loop() {
  // put your main code here, to run repeatedly:
  nm.loop();

  if(nm.dataAvailable()){
    SerialUSB.print("Data available!");
    nm.getSensorData((uint8_t *)0, (uint8_t *)0);
  }

  //lora.sendData(testdata, 4);
  //testdata[3]++;
}
