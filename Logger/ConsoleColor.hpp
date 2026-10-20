#pragma once

#include <string>

namespace Color
{
namespace Compatibility
{
namespace Text
{
static const std::string Default = "\x1b[39m";
static const std::string Black = "\x1b[30m";
static const std::string Red = "\x1b[31m";
static const std::string Green = "\x1b[32m";
static const std::string Yellow = "\x1b[33m";
static const std::string Blue = "\x1b[34m";
static const std::string Magenta = "\x1b[35m";
static const std::string Cyan = "\x1b[36m";

static const std::string LightGray = "\x1b[37m";
static const std::string DarkGray = "\x1b[90m";
static const std::string LightRed = "\x1b[91m";
static const std::string LightGreen = "\x1b[92m";
static const std::string LightYellow = "\x1b[93m";
static const std::string LightBlue = "\x1b[94m";
static const std::string LightMagenta = "\x1b[95m";
static const std::string LightCyan = "\x1b[96m";
static const std::string White = "\x1b[97m";
} // namespace Text
namespace Background
{
static const std::string Default = "\x1b[49m";
static const std::string Black = "\x1b[40m";
static const std::string Red = "\x1b[41m";
static const std::string Green = "\x1b[42m";
static const std::string Yellow = "\x1b[43m";
static const std::string Blue = "\x1b[44m";
static const std::string Magenta = "\x1b[45m";
static const std::string Cyan = "\x1b[46m";

static const std::string LightGray = "\x1b[47m";
static const std::string DarkGray = "\x1b[100m";
static const std::string LightRed = "\x1b[101m";
static const std::string LightGreen = "\x1b[102m";
static const std::string LightYellow = "\x1b[103m";
static const std::string LightBlue = "\x1b[104m";
static const std::string LightMagenta = "\x1b[105m";
static const std::string LightCyan = "\x1b[106m";
static const std::string White = "\x1b[107m";
} // namespace Background

} // namespace Compatibility

namespace Backgound
{
static const std::string Default = "\x1b[49m";
static const std::string Black = "\x1b[48;5;0m";
static const std::string Maroon = "\x1b[48;5;1m";
static const std::string Green = "\x1b[48;5;2m";
static const std::string Olive = "\x1b[48;5;3m";
static const std::string Navi = "\x1b[48;5;4m";
static const std::string Purple = "\x1b[48;5;5m";
static const std::string Teal = "\x1b[48;5;6m";
static const std::string Silver = "\x1b[48;5;7m";
static const std::string Grey = "\x1b[48;5;8m";
static const std::string Red = "\x1b[48;5;9m";
static const std::string Lime = "\x1b[48;5;10m";
static const std::string Yellow = "\x1b[48;5;11m";
static const std::string Blue = "\x1b[48;5;12m";
static const std::string Fuchsia = "\x1b[48;5;13m";
static const std::string Aqua = "\x1b[48;5;14m";
static const std::string White = "\x1b[48;5;15m";
inline std::string get_color(unsigned short index)
{
    return "\x1b[48;5;" + std::to_string(index) + "m";
}
} // namespace Backgound

namespace Text
{
static const std::string Default = "\x1b[39m";
static const std::string Black = "\x1b[38;5;0m";
static const std::string Maroon = "\x1b[38;5;1m";
static const std::string Green = "\x1b[38;5;2m";
static const std::string Olive = "\x1b[38;5;3m";
static const std::string Navi = "\x1b[38;5;4m";
static const std::string Purple = "\x1b[38;5;5m";
static const std::string Teal = "\x1b[38;5;6m";
static const std::string Silver = "\x1b[38;5;7m";
static const std::string Grey = "\x1b[38;5;8m";
static const std::string Red = "\x1b[38;5;9m";
static const std::string Lime = "\x1b[38;5;10m";
static const std::string Yellow = "\x1b[38;5;11m";
static const std::string Blue = "\x1b[38;5;12m";
static const std::string Fuchsia = "\x1b[38;5;13m";
static const std::string Aqua = "\x1b[38;5;14m";
static const std::string White = "\x1b[38;5;15m";
inline std::string get_color(unsigned short index)
{
    return "\x1b[38;5;" + std::to_string(index) + "m";
}

} // namespace Text

namespace Formatting
{
static const std::string Bold = "\x1b[1m";
static const std::string Dim = "\x1b[2m";
static const std::string Italic = "\x1b[3m";
static const std::string Underlined = "\x1b[4m";
static const std::string Blink = "\x1b[5m";
static const std::string Reverse = "\x1b[7m";
static const std::string Hidden = "\x1b[8m";

static const std::string ResetBold = "\x1b[21m";
static const std::string ResetDim = "\x1b[22m";
static const std::string ResetItalic = "\x1b[23m";
static const std::string ResetUnderlined = "\x1b[24m";
static const std::string ResetBlink = "\x1b[25m";
static const std::string ResetReverse = "\x1b[27m";
static const std::string ResetHidden = "\x1b[28m";
} // namespace Formatting
static const std::string Reset = "\x1b[0m";
} // namespace Color