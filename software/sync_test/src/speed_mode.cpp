/* 
Servo factory speed unit is 0.0146rpm, speed changed to V=2400
*/

#include <iostream>
#include "SCServo.h"

SMS_STS servo_bus;

int main(int argc, char ** argv)
{
  if (argc < 2)
  {
    std::cout << "argc error!" << std::endl;
    return 0;
  }

  std::cout << "serial:" << argv[1] << std::endl;
  if (!servo_bus.begin(1000000, argv[1]))
  {
    std::cout << "Failed to init sms/sts motor!" << std::endl;
    return 0;
  }

  servo_bus.wheelMode(1);  //Constant speed mode
  std::cout << "mode = " << 1 << std::endl;
  while (1)
  {
    servo_bus.writeSpeed(1, 2400, 50);  // Servo (ID1) rotates at a maximum speed of V=2400 (steps/sec) and an acceleration of A=50 (50*100 steps/sec^2)
    std::cout << "speed = " << 2400 << std::endl;
    sleep(2);
    servo_bus.writeSpeed(1, 0, 50);  // Servo (ID1) stops rotation (V=0) with acceleration A=50 (50*100 steps/sec^2)
    std::cout << "speed = " << 0 << std::endl;
    sleep(2);
    servo_bus.writeSpeed(1, -2400, 50);  // Servo (ID1) rotates in reverse at a maximum speed of V=2400 (steps/sec) and an acceleration of A=50 (50*100 steps/sec^2)
    std::cout << "speed = " << -2400 << std::endl;
    sleep(2);
    servo_bus.writeSpeed(1, 0, 50);  // Servo (ID1) stops rotation (V=0) with acceleration A=50 (50*100 steps/sec^2)
    std::cout << "speed = " << 0 << std::endl;
    sleep(2);
  }

  servo_bus.end();
  return 1;
}