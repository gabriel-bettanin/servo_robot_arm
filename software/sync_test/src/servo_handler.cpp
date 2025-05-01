#include "servo_handler.hpp"

// Class constructors

ServoHandler::ServoHandler() { SCS::End = 0; }
ServoHandler::ServoHandler(std::vector<uint8_t> ids, int baud_rate, const char * serial_port) : ids_(ids)
{
  SCS::End = 0;

  std::cout << ids_.data() << std::endl;

  // Resize vectors with id vecotr size
  std::lock_guard<std::mutex> lock_state(state_mutex_);
  std::lock_guard<std::mutex> lock_command(command_mutex_);
  command_.resize(ids_.size());
  state_.resize(ids_.size());

  // Begin serial communication using given rate and port
  if (!SCSerial::begin(baud_rate, serial_port))
  {
    std::cerr << "Failed to init servo motor bus!" << std::endl;
  }
}
ServoHandler::~ServoHandler() { SCSerial::end(); }

// Servo management methods

int ServoHandler::enableTorque(u8 id, bool enable) { return SCS::writeByte(id, STS_TORQUE_ENABLE, static_cast<u8>(enable)); }
int ServoHandler::enableEeprom(u8 id, bool enable) { return SCS::writeByte(id, STS_LOCK, static_cast<u8>(enable)); }
int ServoHandler::operationMode(u8 id, int mode) { return SCS::writeByte(id, STS_MODE, static_cast<u8>(mode)); }
int ServoHandler::homePosition(u8 id) { return SCS::writeByte(id, STS_TORQUE_ENABLE, 128); }

// Getters and setters

std::vector<uint8_t> ServoHandler::getIDs() const { return ids_; }

std::vector<ServoState> ServoHandler::getState()
{
  std::lock_guard<std::mutex> lock(state_mutex_);
  return state_;
}

std::vector<ServoCommand> ServoHandler::getCommand()
{
  std::lock_guard<std::mutex> lock(command_mutex_);
  return command_;
}

void ServoHandler::setCommand(const std::vector<ServoCommand> & command)
{
  std::lock_guard<std::mutex> lock(command_mutex_);
  if (command.size() == command_.size())
  {
    command_ = command;
  }
}

// Real-time read and write methods

void ServoHandler::read()
{
  uint8_t rx_packet[15];  // 15 bytes for full sync read block

  // SYNC_READ from 0x38 (Position) to 0x45 (Current), read 15 bytes per servo
  syncReadBegin(ids_.size(), sizeof(rx_packet));
  syncReadPacketTx(ids_.data(), ids_.size(), STS_PRESENT_POSITION_L, sizeof(rx_packet));

  // Lock access to the state vector
  std::lock_guard<std::mutex> lock(state_mutex_);

  for (uint8_t i = 0; i < ids_.size(); i++)
  {
    if (!syncReadPacketRx(ids_[i], rx_packet))
    {
      std::cerr << "ID:" << int(ids_[i]) << " sync read error!" << std::endl;
    }
    state_[i].position = rx_packet[0] | (rx_packet[1] << 8);
    state_[i].speed = decodeSignedWithDirBit((rx_packet[2] | (rx_packet[3] << 8)), 14);
    state_[i].load = rx_packet[4] | (rx_packet[5] << 8);
    state_[i].voltage = rx_packet[6];
    state_[i].temperature = rx_packet[7];
    state_[i].move = rx_packet[10];
    state_[i].current = rx_packet[13] | (rx_packet[14] << 8);
  }

  syncReadEnd();
}

void ServoHandler::write()
{
  std::vector<uint8_t> data;

  // Lock access to the command vector and then create a copy for usage
  std::vector<ServoCommand> command_temp;
  {
    std::lock_guard<std::mutex> lock(command_mutex_);
    command_temp = command_;
  }

  for (int i = 0; i < ids_.size(); i++)
  {
    const ServoCommand & cmd = command_temp[i];

    int position = std::clamp(cmd.position, -MAX_POSITION, MAX_POSITION);
    int speed = std::clamp(cmd.speed, -MAX_SPEED, MAX_SPEED);
    int acceleration = std::clamp(cmd.acceleration, 0, MAX_ACCEL);
    int time = std::clamp(cmd.time, -MAX_TIME, MAX_TIME);

    uint16_t enc_position = encodeSignedWithDirBit(position, 15);
    uint16_t enc_time = encodeSignedWithDirBit(time, 10);
    uint16_t enc_speed = encodeSignedWithDirBit(speed, 15);

    data.push_back(static_cast<uint8_t>(acceleration));
    data.push_back(enc_position & 0xFF);
    data.push_back(enc_position >> 8);
    data.push_back(enc_time & 0xFF);
    data.push_back(enc_time >> 8);
    data.push_back(enc_speed & 0xFF);
    data.push_back(enc_speed >> 8);

    /* std::cout << "ID:" << int(ids_[i]) << " Pos=" << position << " (0x" << std::hex << enc_position << ")"
              << " Spd=" << speed << " (0x" << enc_speed << ")"
              << " Acc=" << acceleration << " Time=" << time << " (0x" << enc_time << ")"
              << " Bytes=["
              << "0x" << std::hex << int(data[data.size() - 7]) << ", "
              << "0x" << int(data[data.size() - 6]) << ", "
              << "0x" << int(data[data.size() - 5]) << ", "
              << "0x" << int(data[data.size() - 4]) << ", "
              << "0x" << int(data[data.size() - 3]) << ", "
              << "0x" << int(data[data.size() - 2]) << ", "
              << "0x" << int(data[data.size() - 1]) << "]" << std::dec << std::endl; */
  }

  // SYNC_WRITE from 0x29 (Acceleration) to 0x2E (Running speed), write 07 bytes per servo
  snycWrite(ids_.data(), ids_.size(), STS_ACC, data.data(), 7);
}

// Helper methods

uint16_t ServoHandler::encodeSignedWithDirBit(int value, int bit) { return static_cast<uint16_t>((value < 0) ? ((-value) | (1 << bit)) : value); }

int ServoHandler::decodeSignedWithDirBit(uint16_t value, int bit)
{
  int mask = 1 << bit;
  int magnitude = value & ~mask;
  return (value & mask) ? -magnitude : magnitude;
}