#include "Logger.hpp"
#include "ConsoleColor.hpp"
#include <chrono>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <sstream>

std::map<unsigned int, std::string> Logger::registered_;

std::string Logger::get_time(const std::string& format)
{
    using namespace std::chrono;

    // get current time
    auto now = system_clock::now();

    // get number of milliseconds for the current second
    // (remainder after division into seconds)
    auto ms = duration_cast<milliseconds>(now.time_since_epoch()) % 1000;

    // convert to std::time_t in order to convert to std::tm (broken time)
    auto timer = system_clock::to_time_t(now);

    // convert to broken time
    std::tm bt = *std::localtime(&timer);

    std::ostringstream oss;

    oss << std::put_time(&bt, format.c_str()); // HH:MM:SS
    oss << '.' << std::setfill('0') << std::setw(3) << ms.count();

    return oss.str();
}

void Logger::log(const std::string& message, unsigned int level)
{
    std::cerr << Color::Compatibility::Text::LightGray << "[" << Color::Text::Aqua << Color::Formatting::Bold
              << get_time("%d-%m-%Y %H:%M:%S") << Color::Reset << Color::Compatibility::Text::LightGray << "] ";
    switch (level)
    {
    case Error:
        std::cerr << Color::Compatibility::Text::LightGray << "[ " << Color::Compatibility::Text::Red
                  << Color::Formatting::Bold << "Error" << Color::Reset << Color::Compatibility::Text::LightGray
                  << "   ] " << Color::Compatibility::Text::Red << message << Color::Reset << std::endl;
        break;
    case Warning:
        std::cerr << Color::Compatibility::Text::LightGray << "[ " << Color::Compatibility::Text::Yellow
                  << Color::Formatting::Bold << "Warning" << Color::Reset << Color::Compatibility::Text::LightGray
                  << " ] " << Color::Compatibility::Text::Yellow << message << Color::Reset << std::endl;
        break;
    case Information:
        std::cerr << Color::Compatibility::Text::LightGray << "[ " << Color::Compatibility::Text::Green
                  << Color::Formatting::Bold << "Info" << Color::Reset << Color::Compatibility::Text::LightGray
                  << "    ] " << Color::Compatibility::Text::Green << message << Color::Reset << std::endl;
        break;

    default:
        if (registered_.count(level))
        {
            std::cerr << registered_[level] << message << Color::Reset << std::endl;
        }
        break;
    }
}
void Logger::register_level(const std::string& format, unsigned int level)
{
    registered_.insert(std::make_pair(level, format));
}
