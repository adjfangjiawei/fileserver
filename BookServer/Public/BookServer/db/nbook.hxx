// neo4j的书
#include <nlohmann/json.hpp>
// 书籍
class NbBook {
  public:
    unsigned long ID;
    std::string node_name = "Book";
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(NbBook, node_name, ID);
};

// 作者
class NbAuthor {
  public:
    unsigned long ID;
    std::string node_name = "Author";
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(NbAuthor, node_name, ID);
};

class NbBookAuthorRelation {
  public:
    std::string relation_name;
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(NbBookAuthorRelation, relation_name);
};

// 书籍标签
class NBookTag {
  public:
    unsigned long ID;
    std::string node_name = "BookTag";
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(NBookTag, node_name, ID);
};