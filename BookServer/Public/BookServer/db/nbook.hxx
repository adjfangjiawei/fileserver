// neo4j的书
#include <nlohmann/json.hpp>
// 书籍
class NbBook {
  public:
    unsigned long ID;
    static std::string node_name;
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(NbBook, node_name, ID);
};
std::string NbBook::node_name = "Book";

// 作者
class NbAuthor {
  public:
    unsigned long ID;
    static std::string node_name;
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(NbAuthor, node_name, ID);
};
std::string NbAuthor::node_name = "Author";

class NbBookAuthorRelation {
  public:
    // 节点起点rnb+数字
    // 节点重点rne+数字
    static std::string nodeBegin;
    static std::string nodeEnd;
    static std::string relation_name;
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(NbBookAuthorRelation, relation_name);
};
std::string NbBookAuthorRelation::nodeBegin{"Book"};
std::string NbBookAuthorRelation::nodeEnd{"Author"};
std::string NbBookAuthorRelation::relation_name{"booksAuthor"};
// 书籍标签
class NbBookTag {
  public:
    unsigned long ID;
    static std::string node_name;
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(NbBookTag, node_name, ID);
};
std::string NbBookTag::node_name = "BookTag";

class NbPublisher {
  public:
    unsigned long ID;
    static std::string node_name;
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(NbPublisher, node_name, ID);
};
std::string NbPublisher::node_name = "Publisher";
class NbBookPublisherRelation {
  public:
    // 节点起点rnb+数字
    // 节点重点rne+数字
    static std::string nodeBegin;
    static std::string nodeEnd;
    static std::string relation_name;
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(NbBookPublisherRelation, relation_name);
};
std::string NbBookPublisherRelation::nodeBegin{"Book"};
std::string NbBookPublisherRelation::nodeEnd{"Publisher"};
std::string NbBookPublisherRelation::relation_name{"booksPublisher"};