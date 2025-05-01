cmake_minimum_required(VERSION 3.10)
project(sync_test)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include SCServo headers from FTServo_Linux
include_directories(${CMAKE_SOURCE_DIR}/../docs/FTServo_Linux/include)

# Include your own headers (if needed)
include_directories(${CMAKE_SOURCE_DIR}/include)

# sync_read
add_executable(sync_read src/sync_read.cpp)
target_link_libraries(
  sync_read
  PRIVATE ${CMAKE_SOURCE_DIR}/../docs/FTServo_Linux/build/libSCServo.a)

# sync_write
add_executable(sync_write src/sync_write.cpp)
target_link_libraries(
  sync_write
  PRIVATE ${CMAKE_SOURCE_DIR}/../docs/FTServo_Linux/build/libSCServo.a)

# speed_mode
add_executable(speed_mode src/speed_mode.cpp)
target_link_libraries(
  speed_mode
  PRIVATE ${CMAKE_SOURCE_DIR}/../docs/FTServo_Linux/build/libSCServo.a)

# input_test
add_executable(input_test src/main.cpp src/servo_handler.cpp)
target_link_libraries(
  input_test
  PRIVATE ${CMAKE_SOURCE_DIR}/../docs/FTServo_Linux/build/libSCServo.a)
