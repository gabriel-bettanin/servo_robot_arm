#include <iostream>
#include "SCServo.h"

SMS_STS servo_bus;

uint8_t ids[] = {1, 2, 3};
uint8_t rx_packet[15];  // 15 bytes per servo
int16_t position, speed, load, current;
uint8_t voltage, temperature;

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

  // Sync read

  servo_bus.syncReadBegin(sizeof(ids), sizeof(rx_packet));
  while (1)
  {
    // SYNC READ from 0x38, read 15 bytes per servo
    servo_bus.syncReadPacketTx(ids, sizeof(ids), SMS_STS_PRESENT_POSITION_L, sizeof(rx_packet));

    for (uint8_t i = 0; i < sizeof(ids); i++)
    {
      if (!servo_bus.syncReadPacketRx(ids[i], rx_packet))
      {
        std::cout << "ID:" << int(ids[i]) << " sync read error!" << std::endl;
        continue;
      }

      // Decode data from known offsets
      position = rx_packet[0] | (rx_packet[1] << 8);

      uint16_t raw_speed = rx_packet[2] | (rx_packet[3] << 8);
      if (raw_speed & 0x4000)
      {                                 // 15th bit (0x4000) is set → negative
        speed = -(raw_speed & 0x3FFF);  // Mask out bit 15
      }
      else
      {
        speed = raw_speed & 0x3FFF;
      }

      load = rx_packet[4] | (rx_packet[5] << 8);
      voltage = rx_packet[6];
      temperature = rx_packet[7];
      current = rx_packet[13] | (rx_packet[14] << 8);

      std::cout << "ID:" << int(ids[i]) << " Pos:" << position << " Spd:" << speed << " Load:" << load << " Volt:" << int(voltage) << " Temp:" << int(temperature) << " Curr:" << current << std::endl;
    }
    usleep(1 * 1000);
  }
  servo_bus.syncReadEnd();

  servo_bus.end();
  return 1;
}
