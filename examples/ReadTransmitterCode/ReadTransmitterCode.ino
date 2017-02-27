#include <orgb417pb.h>

ORGB417PB receiver(0);
unsigned long value = 0;

void setup() {
  Serial.begin(115200); 
  Serial.println("\nReady.\n\n");
}

void loop() {
  if (receiver.has_code()) {
    if (value != receiver.code()) {
      value = receiver.code();
      Serial.print("Found: ");
      Serial.print(value);
      Serial.print(" - ");
      Serial.println(value, BIN);
    }
    receiver.clear_code();
  }
}
