#include <unistd.h>
#include <cstddef>
#include <set>
#include <string>
#include <vector>

#include "process.h"
#include "processor.h"
#include "system.h"
#include "linux_parser.h"

using std::set;
using std::size_t;
using std::string;
using std::vector;


Processor& System::Cpu() { return cpu_; }

vector<Process>& System::Processes() {
  //https://knowledge.udacity.com/questions/74924
  processes_.clear();
  auto vec = LinuxParser::Pids();

  for (auto& element : vec) {
    Process newProcess{element};
    processes_.push_back(newProcess);
  }

  std::sort(processes_.begin(), processes_.end());

  return processes_;
}

std::string System::Kernel() { return LinuxParser::Kernel(); }

float System::MemoryUtilization() { return LinuxParser::MemoryUtilization(); }

std::string System::OperatingSystem() { return LinuxParser::OperatingSystem(); }

int System::RunningProcesses() { return LinuxParser::RunningProcesses(); }

int System::TotalProcesses() { return LinuxParser::TotalProcesses(); }

long int System::UpTime() { return LinuxParser::UpTime(); }