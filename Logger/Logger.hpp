#pragma once
#include <map>
#include <string>

class Logger
{
  public:
    enum LogLevel
    {
        Error,
        Warning,
        Information,
    };

    static void log(const std::string &message, unsigned int level);
    // Adds a custom level printed with the given prefix (e.g. a color code).
    static void register_level(const std::string &format, unsigned int level);

  private:
    static std::map<unsigned int, std::string> registered_;
    static std::string get_time(const std::string &format);
};
