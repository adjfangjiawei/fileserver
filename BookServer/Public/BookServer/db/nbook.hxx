// neo4j的书
#include <nlohmann/json.hpp>
// 书籍
class NbBook {
  public:
    int ID;
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(NbBook, ID);
};

// 作者
class NbAuthor {
    int ID;
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(NbAuthor, ID);
};

// 书籍标签
class NBookTag {
    int ID;
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(NBookTag, ID);
};