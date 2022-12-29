#include <unistd.h>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>

#include "process.h"
#include "linux_parser.h"

using std::string;
using std::to_string;
using std::vector;

int Process::Pid() { return _pid; }

float Process::CpuUtilization() const {
  return LinuxParser::CpuUtilization(_pid);
}


string Process::Command() { return LinuxParser::Command(_pid); }

string Process::Ram() {
  auto ram_kb = LinuxParser::Ram(_pid);
  int ram_mb;
  // Sanity check in case Ram() is a string with random data;
  try {
    ram_mb = std::stod(ram_kb) * 0.001;
  } catch (...) {
    return string();
  }
  return std::to_string(ram_mb);
}

string Process::User() { return LinuxParser::User(_pid); }

long int Process::UpTime() { return LinuxParser::UpTime(_pid); }

bool Process::operator<(Process const& a) const {
  return a.CpuUtilization() < CpuUtilization();
}
