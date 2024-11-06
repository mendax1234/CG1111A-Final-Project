#include "MeMCore.h"

#define IRWait 30

MePort ir_adapter(PORT_3);
MePort ldr_adapter(PORT_4);

int getAvgReadingIR(int times){
  int reading;
  int total = 0;
  for (int i = 0; i < times; i++) {
    reading = ir_adapter.aRead2();
    total = reading + total;
    delay(IRWait);
  }
  return total / times;
}


int ir_read() {
  ldr_adapter.dWrite1(HIGH);
  ldr_adapter.dWrite2(LOW);
  delay(IRWait);

  int ambient = ir_adapter.aRead2();
   Serial.print("ambient: ");
   Serial.println(ambient);
  ldr_adapter.dWrite1(LOW);
  ldr_adapter.dWrite2(LOW);
  delay(IRWait);
  int reading = ir_adapter.aRead2();
   Serial.print("reading: ");
   Serial.println(reading);
   Serial.print("difference: ");
   Serial.println(ambient - reading);
  int difference = ambient - reading;

}

void setup_for_ir() {
  Serial.begin(9600);
}

void loop_for_ir() {
  ir_read();
}
