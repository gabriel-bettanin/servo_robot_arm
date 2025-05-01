#ifndef SERVO_HANDLER_HPP
#define SERVO_HANDLER_HPP

#include <algorithm>
#include <iostream>
#include <mutex>
#include <vector>

#include "SCSerial.h"

#include "servo_memory.hpp"

constexpr int MAX_POSITION = 32766;
constexpr int MAX_SPEED = 32766;
constexpr int MAX_TIME = 1000;
constexpr int MAX_ACCEL = 254;

struct ServoState
{
  int position = 0;
  int speed = 0;
  int load = 0;
  int current = 0;
  int voltage = 0;
  int temperature = 0;
  int move = 0;
};

struct ServoCommand
{
  int position = 0;
  int speed = 0;
  int acceleration = 0;
  int time = 0;
};

class ServoHandler : public SCSerial
{
public:
  ServoHandler();
  ServoHandler(std::vector<uint8_t> ids, int baud_rate, const char * serial_port);
  ~ServoHandler();

  int enableTorque(u8 id, bool enable);  // Enable torque control
  int enableEeprom(u8 id, bool enable);  // Enable writing to eeprom
  int operationMode(u8 id, int mode);    // Switch operation mode
  int homePosition(u8 id);               // Set current postion as zero

  void read();   // Populate the state vector values using Sync read
  void write();  // Sync write using the command vector values

  std::vector<uint8_t> getIDs() const;
  std::vector<ServoState> getState();
  std::vector<ServoCommand> getCommand();
  void setCommand(const std::vector<ServoCommand> & command);

private:
  std::vector<uint8_t> ids_;

  std::mutex state_mutex_;
  std::mutex command_mutex_;
  std::vector<ServoState> state_;
  std::vector<ServoCommand> command_;

  uint16_t encodeSignedWithDirBit(int value, int bit);
  int decodeSignedWithDirBit(uint16_t value, int bit);
};

#endif  // SERVO_HANDLER_HPP