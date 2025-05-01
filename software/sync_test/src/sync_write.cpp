/*
Servo factory speed unit is 0.0146rpm, speed changed to V=2400
*/

#include <iostream>
#include "SCServo.h"

SMS_STS servo_bus;

u8 ids[3] = {1, 2, 3};
s16 position[3];
u16 speed[3] = {2400, 2400, 2400};
u8 acceleration[3] = {50, 50, 50};

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

  while (1)
  {
    // Servo (ids1/ids2) running at maximum speed V=2400 (steps/sec), acceleration A=50 (50*100 steps/sec^2) to position P1=4095
    for (int i = 0; i < 3; ++i) position[i] = 4095;
    servo_bus.syncWritePosEx(ids, sizeof(ids), position, speed, acceleration);
    std::cout << "pos = " << 4095 << std::endl;
    usleep(2187 * 1000);  //[(P1-P0)/V]*1000+[V/(A*100)]*1000

    //Servo (ids1/ids2) running at maximum speed V=2400 (steps/sec), acceleration A=50 (50*100 steps/sec^2), to position P0=0
    for (int i = 0; i < 3; ++i) position[i] = 0;
    servo_bus.syncWritePosEx(ids, sizeof(ids), position, speed, acceleration);
    std::cout << "pos = " << 0 << std::endl;
    usleep(2187 * 1000);  //[(P1-P0)/V]*1000+[V/(A*100)]*1000
  }

  servo_bus.end();
  return 1;
}
