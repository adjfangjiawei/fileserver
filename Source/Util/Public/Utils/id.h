#ifndef UTILS_ID
#define UTILS_ID
#include <snowflake.hpp>
#include <string>
namespace utils {
    // 书名的正则表达式，形式为books/*
    extern std::string_view bookReg;
    extern std::string_view bookTagReg;
    extern std::string_view authorReg;
    extern uint BookGetIDFromName(std::string_view name);
    extern std::string BookGetNameFromID(uint id);
    extern uint BookTagGetIDFromName(std::string_view name);
    extern std::string BookTagGetNameFromID(uint id);
    extern uint AuthorGetIDFromName(std::string_view name);
    extern std::string AuthorGetNameFromID(uint id);
    extern uint GetRequestID();
    using snowflake_t = snowflake<1534832906275L>;
    extern snowflake_t InitSnowFlakeID();
    extern uint GenerateSnowFlakeID();
}  // namespace utils
#endif