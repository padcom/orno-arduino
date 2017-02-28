#include <orgb417pb.h>

ORGB417PB transciver(0, 3);

void setup() {
  Serial.begin(115200);
}

void loop() {
  transciver.send(8305925, 4);
  delay(1000);

  if (transciver.has_code()) {
    Serial.println(transciver.code());
    transciver.clear_code();
  }

  transciver.send(7952341, 4);
  delay(1000);

  if (transciver.has_code()) {
    Serial.println(transciver.code());
    transciver.clear_code();
  }
}
