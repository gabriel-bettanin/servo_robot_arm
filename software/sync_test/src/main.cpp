#include "command_handler.hpp"
#include "servo_handler.hpp"

std::atomic_bool running{true};
std::atomic_bool debug_running{true};
std::thread debug_thread;

void debugLoop(ServoHandler & sh, std::chrono::milliseconds period)
{
  while (debug_running)
  {
    auto start = std::chrono::steady_clock::now();

    std::vector<uint8_t> ids = sh.getIDs();
    std::vector<ServoState> state = sh.getState();
    for (uint8_t i = 0; i < ids.size(); i++)
    {
      std::cout << "ID:" << int(ids[i]) << " Pos:" << state[i].position << " Spd:" << state[i].speed << " Load:" << state[i].load << " Volt:" << state[i].voltage << " Temp:" << state[i].temperature
                << " Curr:" << state[i].current << std::endl;
    }

    auto elapsed = std::chrono::steady_clock::now() - start;
    auto sleep_time = period - std::chrono::duration_cast<std::chrono::milliseconds>(elapsed);

    if (sleep_time > std::chrono::milliseconds(0)) std::this_thread::sleep_for(sleep_time);
  }
}

void controlLoop(ServoHandler & sh, std::chrono::milliseconds period)
{
  while (running)
  {
    auto start = std::chrono::steady_clock::now();

    sh.read();
    sh.write();

    auto elapsed = std::chrono::steady_clock::now() - start;
    auto sleep_time = period - std::chrono::duration_cast<std::chrono::milliseconds>(elapsed);

    if (sleep_time > std::chrono::milliseconds(0)) std::this_thread::sleep_for(sleep_time);
  }
}

void registerCommands(CommandHandler & ch, ServoHandler & sh)
{
  // Enable torque control
  ch.addCommand(
    "e",
    [&](const std::vector<int> & args)
    {
      if (args.size() != 2)
      {
        std::cout << "Usage: e <id> <bool>\n";
        return;
      }
      std::cout << "Switching servo " << args[0] << " to mode " << args[1] << '\n';

      sh.enableTorque(args[0], static_cast<bool>(args[1]));
    });

  // Home position
  ch.addCommand(
    "h",
    [&](const std::vector<int> & args)
    {
      if (args.size() != 1)
      {
        std::cout << "Usage: h <id>\n";
        return;
      }
      std::cout << "Homing servo " << args[0] << '\n';

      sh.homePosition(args[0]);
    });

  // Switching operation mode
  ch.addCommand(
    "m",
    [&](const std::vector<int> & args)
    {
      if (args.size() != 2)
      {
        std::cout << "Usage: m <id> <mode>\n";
        return;
      }
      std::cout << "Switching servo " << args[0] << " to mode " << args[1] << '\n';

      sh.operationMode(args[0], args[1]);
    });

  // Set servo command
  ch.addCommand(
    "c",
    [&](const std::vector<int> & args)
    {
      if (args.size() != 5)
      {
        std::cout << "Usage: c <id> <position> <speed> <acceleration> <time> \n";
        return;
      }
      std::cout << "Commanding servo " << args[0] << " with position: " << args[1] << " speed: " << args[2] << " acceleration: " << args[3] << " time: " << args[4] << '\n';

      std::vector<ServoCommand> cmd = sh.getCommand();
      cmd[args[0]].position = args[1];
      cmd[args[0]].acceleration = args[2];
      cmd[args[0]].speed = args[3];
      cmd[args[0]].time = args[4];
      sh.setCommand(cmd);
    });

  ch.addCommand(
    "d",
    [&](const std::vector<int> & args)
    {
      if (args.size() != 1)
      {
        std::cout << "Usage: d <bool>\n";
        return;
      }

      if (args[0])
      {
        debug_thread = std::thread(debugLoop, std::ref(sh), std::chrono::milliseconds(10));  // 20 Hz
      }
      else
      {
        if (debug_thread.joinable())
        {
          debug_running = false;
          debug_thread.join();
        }
      }
    });
}

int main()
{
  // Initial parameters
  std::vector<uint8_t> ids = {1, 2, 3};
  int baud_rate = 1'000'000;
  const char * serial_port = "/dev/ttyACM0";

  // Creating objects
  ServoHandler sh(ids, baud_rate, serial_port);
  CommandHandler ch;
  registerCommands(ch, sh);

  // Start the control loop thread
  std::thread control_thread(controlLoop, std::ref(sh), std::chrono::milliseconds(1));  // 100 Hz

  /* sh.enableTorque(1, true);
  sh.enableTorque(2, true);
  sh.enableTorque(3, true);
  sh.operationMode(1, 2);
  sh.operationMode(2, 2);
  sh.operationMode(3, 2);

  ServoCommand cmd;
  cmd.position = 4096;
  cmd.acceleration = 50;
  cmd.speed = 2400;
  cmd.time = -500;
  std::vector<ServoCommand> cmd_vector = {cmd, cmd, cmd};
  sh.setCommand(cmd_vector); */

  // Start threads
  ch.start();
  ch.wait();

  // Stop control loop
  running = false;
  control_thread.join();

  return 0;
}
