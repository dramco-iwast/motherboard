#include <Arduino.h>
#include <Wire.h>
#include "NodeManager.h"
#include "LoRaWAN.h"
#include "device_settings.h"
#include "DebugSerial.h"
#ifndef NO_WATCHDOG
#include "Adafruit_SleepyDog.h"
#endif
#include "BoardLed.h"
//#include <ArduinoLowPower.h>

BoardLed led;
NodeManager nm;
LoRaSettings_t settings = LORA_INIT_MY_DEVICE;

uint8_t buf[LORA_MAX_PAYLOAD_SIZE];

void setup(){
/*// Low power test 
  SerialUSB.begin(115200);
  while(!SerialUSB);

  SerialUSB.println("Low Power Test");
  SerialUSB.flush();
  delay(1000);
  SerialUSB.end();
  USBDevice.end();
  USBDevice.detach();

  LowPower.sleep();

  while(1);
*/
  led.init();
  led.BoardLed_Action(FLASH, 5);

#ifdef SERIAL_DEBUG_OUTPUT
    delay(5000);
#endif
  // start serial interface for printing debug messages
  DEBUG.begin();
  DEBUG.println(F("DEBUG OUTPUT ENABLED."));

  // check if watchodog was the cause of reset
  // TODO: check effect on power consumption
  // TODO: check if this helps to improve application robustness
  bool skipConfig = false;
  if(nm.watchdogReset()){
    skipConfig = true;
  };

  // USB Serial for configuration
  SerialAT.begin(115200);

  // start the node manager
  nm.begin();

  // if skipConfig is true, sensors will be configured with a previously stored configuration
  nm.runConfigMode(skipConfig); // run config

  // get device specific settings
  nm.getLoraSettings(&settings);

  // configure lora (no networking yet)
  lora.begin(settings, &led);

  // join lora network
  lora.join();

  // put lora modem in sleep (save energy)
  lora.sleep(); // saves approx. 2.5 mA

  // start rtc
  nm.start();
}

void loop() {
  // let the node manager handle the sleeping and sensing
  nm.loop();

  // we are awake -> check for data to send
  bool isStatus;
  uint8_t pSize = nm.payloadAvailable(&isStatus);
  if(pSize > 0){
    DEBUG.println("\nMAIN APP: Data available!");
    DEBUG.print(pSize);
    DEBUG.println(" bytes.");
    if(nm.getLoraPayload(buf, LORA_MAX_PAYLOAD_SIZE)){
      DEBUG.print("Payload: ");
      DEBUG.printHexBuf(buf, pSize);

      // status messages are sent every 6 hours (4 / day) as a confirmed message
      // this allows for data rate adjustments by the ADR mechanism
      if(isStatus && CONFIRMED_MESSAGES_ALLOWED){
        DEBUG.println("Includes status -> confirmed message.");
      }

      // send the data
      lora.wake();
#ifndef NO_WATCHDOG
      Watchdog.reset();
#endif
      lora.sendData(buf, pSize, (isStatus && CONFIRMED_MESSAGES_ALLOWED));
      lora.sleep();
    }
    else{
      DEBUG.println("Empty payload.");
    }
  }

  nm.sleep();
}
