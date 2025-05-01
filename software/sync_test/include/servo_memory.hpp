#ifndef SERVO_MEMORY_HPP
#define SERVO_MEMORY_HPP

// Baud rate definitions
#define STS_1M 0
#define STS_0_5M 1
#define STS_250K 2
#define STS_128K 3
#define STS_115200 4
#define STS_76800 5
#define STS_57600 6
#define STS_38400 7

// Memory table definitions
//-------EPROM(Read-only)--------
#define STS_MODEL_L 3
#define STS_MODEL_H 4

// -------EPROM (Read/Write)--------
#define STS_ID 5
#define STS_BAUD_RATE 6
#define STS_MIN_ANGLE_LIMIT_L 9
#define STS_MIN_ANGLE_LIMIT_H 10
#define STS_MAX_ANGLE_LIMIT_L 11
#define STS_MAX_ANGLE_LIMIT_H 12
#define STS_CW_DEAD 26
#define STS_CCW_DEAD 27
#define STS_OFS_L 31
#define STS_OFS_H 32
#define STS_MODE 33

// -------SRAM (Read/Write)--------
#define STS_TORQUE_ENABLE 40
#define STS_ACC 41
#define STS_GOAL_POSITION_L 42
#define STS_GOAL_POSITION_H 43
#define STS_GOAL_TIME_L 44
#define STS_GOAL_TIME_H 45
#define STS_GOAL_SPEED_L 46
#define STS_GOAL_SPEED_H 47
#define STS_LOCK 55

// -------SRAM (Read-only)--------
#define STS_PRESENT_POSITION_L 56
#define STS_PRESENT_POSITION_H 57
#define STS_PRESENT_SPEED_L 58
#define STS_PRESENT_SPEED_H 59
#define STS_PRESENT_LOAD_L 60
#define STS_PRESENT_LOAD_H 61
#define STS_PRESENT_VOLTAGE 62
#define STS_PRESENT_TEMPERATURE 63
#define STS_MOVING 66
#define STS_PRESENT_CURRENT_L 69
#define STS_PRESENT_CURRENT_H 70

#endif  // SERVO_MEMORY_HPP