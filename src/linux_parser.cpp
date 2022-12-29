#include <dirent.h>
#include <unistd.h>
#include <string>
#include <vector>

#include "linux_parser.h"

using std::stof;
using std::string;
using std::to_string;
using std::vector;

// DONE: An example of how to read data from the filesystem
string LinuxParser::OperatingSystem() {
  string line;
  string key;
  string value;
  std::ifstream filestream(kOSPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), '=', ' ');
      std::replace(line.begin(), line.end(), '"', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "PRETTY_NAME") {
          std::replace(value.begin(), value.end(), '_', ' ');
          return value;
        }
      }
    }
  }
  return value;
}

// DONE: An example of how to read data from the filesystem
string LinuxParser::Kernel() {
  string os, version, kernel;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
  }
  return kernel;
}

// BONUS: Update this to use std::filesystem
vector<int> LinuxParser::Pids() {
  vector<int> pids;
  DIR* directory = opendir(kProcDirectory.c_str());
  struct dirent* file;
  while ((file = readdir(directory)) != nullptr) {
    // Is this a directory?
    if (file->d_type == DT_DIR) {
      // Is every character of the name a digit?
      string filename(file->d_name);
      if (std::all_of(filename.begin(), filename.end(), isdigit)) {
        int pid = stoi(filename);
        pids.push_back(pid);
      }
    }
  }
  closedir(directory);
  return pids;
}

float LinuxParser::MemoryUtilization() {
  std::string line, title, value;
  float MemTotal = -1.0;
  float MemFree = -1.0;
  std::ifstream filestream(kProcDirectory + kMeminfoFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      linestream >> title >> value;

      if (title == "MemTotal:") {
        MemTotal = std::stof(value);
      } else if (title == "MemFree:") {
        MemFree = std::stof(value);
      }

      if (MemFree >= 0 && MemTotal > 0) {
        return (MemTotal - MemFree) / MemTotal;
      }
    }
  }
  return 0.0;
}

long LinuxParser::UpTime() {
  string line, uptime;
  std::ifstream stream(kProcDirectory + kUptimeFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> uptime;
  }

  return stol(uptime);
}

vector<string> LinuxParser::CpuUtilization() {
  string line, title;
  vector<string> values(8, "0");

  std::ifstream filestream(kProcDirectory + kStatFilename);

  if (filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    linestream >> title >> values[0] >> values[1] >> values[2] >> values[3] >>
        values[4] >> values[5] >> values[6] >>
        values[7];  // guest and guest_nice not used >> values[8] >> values[9];
  }

  return values;
}

int LinuxParser::TotalProcesses() {
  string line, title, value;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      linestream >> title >> value;
      if (title == "processes") {
        return stoi(value);
      }
    }
  }
  return 0;
}

int LinuxParser::RunningProcesses() {
  string line, title, value;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      linestream >> title >> value;
      if (title == "procs_running") {
        return stoi(value);
      }
    }
  }
  return 0;
}

string LinuxParser::Command(int pid) {
  std::string line;
  std::ifstream filestream(kProcDirectory + to_string(pid) + kCmdlineFilename);
  if (filestream.is_open()) {
    std::getline(filestream, line);
    return line;
  }
  return string();

}

string LinuxParser::Ram(int pid) {
  std::string line, title, vmsize;
  std::ifstream filestream(kProcDirectory + to_string(pid) + kStatusFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      linestream >> title >> vmsize;
      if (title == "VmSize:") {
        return vmsize;
      }
    }
  }

  return vmsize;
}

string LinuxParser::Uid(int pid) {
  std::string line, title, uid;
  std::ifstream filestream(kProcDirectory + to_string(pid) + kStatusFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      linestream >> title >> uid;
      if (title == "Uid:") {
        return uid;
      }
    }
  }

  return {};
}

string LinuxParser::User(int pid) {
  std::string wanted_uid = Uid(pid);

  std::string line, user, pass, uid;
  std::ifstream filestream(kPasswordPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      while (linestream >> user >> pass >> uid) {
        if (uid == wanted_uid) {
          return user;
        }
      }
    }
  }
  return string();
}

long LinuxParser::UpTime(int pid) {
  std::string line, value;
  std::ifstream filestream(kProcDirectory + to_string(pid) + kStatFilename);
  if (filestream.is_open()) {
    int pos = 1;
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> value) {
        if (pos == 22) {
          // Sanity check in case value is a string with random data;
          long aux;
          try {
            aux = std::stol(value);
          } catch (...) {
            return 0;
          }

          return aux / sysconf(_SC_CLK_TCK);
        }
        pos++;
      }
    }
  }
  return 0;
}

float LinuxParser::CpuUtilization(int pid) {
  std::string line, value;
  std::ifstream filestream(kProcDirectory + to_string(pid) + kStatFilename);

  float utime = 0.0;
  float stime = 0.0;
  float cutime = 0.0;
  float cstime = 0.0;
  float starttime = 0.0;
  auto Hertz = sysconf(_SC_CLK_TCK);
  int pos = 1;
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> value) {
        // Sanity check in case value is a string with random data;
        try {
          if (pos == 14) {
            utime = std::stof(value);
          } else if (pos == 15) {
            stime = std::stof(value);
          } else if (pos == 16) {
            cutime = std::stof(value);
          } else if (pos == 17) {
            cstime = std::stof(value);
          } else if (pos == 22) {
            starttime = std::stof(value);
          }
        } catch (...) {
          return 0;
        }
        pos++;
        if (pos >= 23) break;
      }
    }
  }

  // Sanity check, prevent division by 0;
  if (pos < 22) {
    return 0;
  }

  float total_time = utime + stime + cutime + cstime;
  float seconds = UpTime() - (starttime / (float)Hertz);

  // Sanity check, prevent division by 0;
  if (seconds == 0) return 0;

  float cpu_usage = total_time / (float)Hertz / seconds;

  return cpu_usage;
}
