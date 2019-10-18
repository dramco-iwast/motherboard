#include <Arduino.h>
#include "NodeManager.h"

#define MOTHERBOARD_ID 0x474F

NodeManager nm(MOTHERBOARD_ID);

void setup() {
  Serial.begin(115200);
  delay(4000);

  // put your setup code here, to run once:
  nm.begin();

}

void loop() {
  // put your main code here, to run repeatedly:
  nm.loop();
}
