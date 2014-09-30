#include <MandolynProtocol.h>

// Connect RF sender to PIN 7

MandolynProtocol MP(7);

void setup(){

}
void loop(){
  // sendSensorSignal(const unsigned int ID, unsigned int RH, double Temp);
  MP.sendSensorSignal(74, 66, 66);
  delay(10000);
}
