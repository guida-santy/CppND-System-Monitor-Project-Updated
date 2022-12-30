#include "processor.h"
#include "linux_parser.h"
#include <chrono>
#include <thread>

// https://stackoverflow.com/questions/23367857/accurate-calculation-of-cpu-usage-given-in-percentage-in-linux
float Processor::Utilization() {
  auto prev_vec = LinuxParser::CpuUtilization();
  long prev_time_Idle;
  try {
    prev_time_Idle = std::stol(prev_vec[LinuxParser::CPUStates::kIdle_]) +
                     std::stol(prev_vec[LinuxParser::CPUStates::kIOwait_]);
  } catch (...) {
    return 0;
  }
  long prev_time_Total = 0;

  for (auto element : prev_vec) {
    prev_time_Total += std::stol(element);
  }

  using namespace std::chrono_literals;
  std::this_thread::sleep_for(1000ms);

  auto vec = LinuxParser::CpuUtilization();
  long time_Idle;
  try {
    time_Idle = std::stol(vec[LinuxParser::CPUStates::kIdle_]) +
                std::stol(vec[LinuxParser::CPUStates::kIOwait_]);

  } catch (...) {
    return 0;
  }
  long time_Total = 0;

  for (auto element : vec) {
    time_Total += std::stol(element);
  }

  float totald = time_Total - prev_time_Total;

  // Sanity check, prevent division by 0;
  if (totald <= 0) {
    return 0;
  }

  float idled = time_Idle - prev_time_Idle;
  return (totald - idled) / totald;
}