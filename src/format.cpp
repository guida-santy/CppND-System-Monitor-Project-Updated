#include <string>
#include "format.h"

using std::string;
string Format::ElapsedTime(long seconds) {
  int hour = 0;
  int minute = 0;
  int second = seconds;

  if (seconds > 60 && seconds < 3600) {
    minute = seconds / 60;
    second = seconds - 60 * minute;
  } else if (seconds >= 3600) {
    hour = seconds / 3600;
    int aux = seconds - hour * 3600;
    minute = aux / 60;
    second = aux - 60 * minute;
  }

  std::string str_hour = std::to_string(hour);
  if (hour < 10) {
    str_hour = "0" + str_hour;
  }
  std::string str_min = std::to_string(minute);
  if (minute < 10) {
    str_min = "0" + str_min;
  }

  std::string str_sec = std::to_string(second);
  if (second < 10) {
    str_sec = "0" + str_sec;
  }

  std::string timestamp = str_hour + ":" + str_min + ":" + str_sec;
  return timestamp;
}