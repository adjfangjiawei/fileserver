#include <utils/id.h>

#include <boost/lexical_cast.hpp>
#include <boost/regex.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
namespace utils {
    std::string_view bookReg = "(books)/([0-9]+)";
    std::string_view AuthorReg = "(authors)/([0-9]+)";
    std::string BookGetNameFromID(uint id) {
        char buff[30];
        sprintf(buff, "books/%d", id);
        return buff;
    }
    uint BookGetIDFromName(std::string_view name) {
        boost::regex bookRegImpl{bookReg.data()};
        bool r = false;
        try {
            r = boost::regex_match(name.data(), bookRegImpl);
        } catch (std::exception& e) {
            return 0;
        }
        if (r) {
            auto id = atoi(name.substr(6).data());
            return id;
        } else {
            return 0;
        }
    }
    std::string BookTagGetNameFromID(uint id) {}
    uint BookTagGetIDFromName(std::string_view name) {}

    uint GetRequestID() {
        boost::uuids::random_generator rgen{};
        return atoi(boost::uuids::to_string(rgen()).c_str());
    }

    uint AuthorGetIDFromName(std::string_view name) {
        boost::regex authorRegImpl{AuthorReg.data()};
        bool r = false;
        try {
            r = boost::regex_match(name.data(), authorRegImpl);
        } catch (std::exception& e) {
            return 0;
        }
        if (r) {
            auto id = atoi(name.substr(8).data());
            return id;
        }
        return 0;
    }
    std::string AuthorGetNameFromID(uint id) {
        char buff[30];
        sprintf(buff, "authors/%d", id);
        return buff;
    }
}  // namespace utils
