#pragma once
#include <string>
#include <map>

class Logger
{
  public:
    enum LogLevel
    {
        Error,
        Warning,
        Information,
    };

  private:
    static std::map<unsigned int, std::string> m_Registered;
    static std::string GetTime(std::string format);

  public:
    Logger(/* args */);
    ~Logger();
    static void Log(const std::string &message, unsigned int level);
    static void Register(const std::string &format, unsigned int index);
};
