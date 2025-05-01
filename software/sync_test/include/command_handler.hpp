#ifndef COMMAND_HANDLER_HPP
#define COMMAND_HANDLER_HPP

#include <atomic>
#include <functional>
#include <iostream>
#include <sstream>
#include <string>
#include <thread>
#include <unordered_map>
#include <vector>

class CommandHandler
{
public:
  CommandHandler() : running_(true) {}

  void addCommand(const std::string & name, std::function<void(const std::vector<int> &)> handler) { commands_[name] = handler; }
  void start() { input_thread_ = std::thread(&CommandHandler::inputLoop, this); }
  void wait() { input_thread_.join(); }
  void stop() { running_ = false; }

private:
  std::atomic<bool> running_;
  std::unordered_map<std::string, std::function<void(const std::vector<int> &)>> commands_;
  std::thread input_thread_;

  void inputLoop()
  {
    std::string line;
    while (running_)
    {
      std::getline(std::cin, line);
      std::istringstream iss(line);

      std::string cmd;
      if (!(iss >> cmd)) continue;

      if (cmd == "quit")
      {
        running_ = false;
        break;
      }

      std::vector<int> args;
      int arg;
      while (iss >> arg) args.push_back(arg);

      auto it = commands_.find(cmd);
      if (it != commands_.end())
      {
        it->second(args);
      }
      else
      {
        std::cout << "Unknown command: " << cmd << '\n';
      }
    }
  }
};

#endif  // COMMAND_HANDLER_HPP
