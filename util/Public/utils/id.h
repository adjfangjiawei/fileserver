#ifndef UTILS_ID
#define UTILS_ID
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
}  // namespace utils
#endif