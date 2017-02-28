#include <orgb417pb.h>

ORGB417PB transciver(0, 3);

void setup() {
  Serial.begin(115200);
}

void loop() {
  // Channel B / ON
  transciver.send(8305925, 6);
  delay(1000);
  // Channel B / OFF
  transciver.send(7952341, 6);
  delay(1000);
}
