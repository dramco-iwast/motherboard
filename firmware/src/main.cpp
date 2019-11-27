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
 // lora.begin(settings);

  // put your setup code here, to run once:
  nm.begin();
  nm.runConfigMode(); // run config
  //nm.runConfigMode(true); // run config forever

  // join lora network
  lora.join();
  lora.sleep();
}

void loop() {
  // put your main code here, to run repeatedly:
  nm.loop();

  // aggregate by using payload size (return value of NodeManager::payloadAvailable())
  uint8_t pSize = nm.payloadAvailable();
  if(pSize > 10){
    DEBUG.println("\nMAIN APP: Enough data available!");
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

  // TODO: make RN2483 sleep as well?
/* From the reference guide
sys sleep <length><length>: decimal number representing the number of milliseconds the system is put to sleep, 
from 100 to 4294967296.

Response:
- ok after the system gets back from Sleep mode
- invalid_param if the length is not valid

This command puts the system to sleep for the specified number of milliseconds.

The module can be forced to exit from sleep by sending the UART a Break condition followed by a 0x55 character.
Forcing the module from sleep in the manner also triggers the UART auto baud detection. The module will adjust 
the UART baud rate to match the baud rate at which the 0x55 character was sent. Refer to the note box in 1.4 
“UART Interface”.Example: sys sleep 120// Puts the system to sleep for 120 ms.

=== G.O. says: 
To make this work for us:
  -> make it go to sleep for a very long time
  -> provide means to skip response (if not "invalid_param")
  -> provide means to capture "ok" on wake
  -> and provide means for sending "break condition" etc. 
*/

  // Handle errors due to channel not free
  //  lora.sendData(buf, pSize);
  }

}