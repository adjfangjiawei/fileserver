

#include <errno.h>
#include <stdio.h>

#include <iostream>
#include <string>
#include <vector>

// 设置软件的语言
void setLanguage() {}

// 设置书籍翻页方式
void setBookMode() {}

// 设置书籍封面，分为手工上传与自动匹配
void setBookCover() {}

// 用户服务,创建一个用户，暂不考虑用户共享
void createuser() {}

// 标签服务，用于方便探索书籍中的对象的关系，像人物，地点，分析书籍之间对象的联系

// 用于创建一个标签，存入mysql中，作为neo4j的结点名
void createTag() {}

// 绑定标签,创建一个节点,节点名即为tag,并且拥有content的property
void bindTag(std::string tag, std::string property, std::string content) {}

// 删除标签,删除一个节点,节点名即为tag,这会删除mysql的表以及所有的这个tag的节点，以及与这个节点有关的连接
void deleteTag(std::string tag) {}

// 将一个节点连接到另一个节点，比如林黛玉是红楼梦里的人物就是有个连接,connectproperty是连接属性，上面的例子connectproperty即为人物
void connectTag(std::string tag1, std::string tag2, std ::string connectproperty) {}

// 获得与此节点的所有连接的节点，排序规则待定
void getConnectedTag(std::string tag, std::vector<std::string> properties) {}

// 笔记服务，云笔记

// 预取书籍，流式从服务器上拉取书籍观看
void prefetchBook(std::string bookId) {}

// 删除书籍
void deleteBook(std::string bookId) {}

// 列出书籍
// 支持过滤选项
// 最近n天打开过
// 最近n天的打开次数
// 所属的用户名，支持多选，不提供则检索所有
// 返回值
// 书籍的名称 封面图片的url 上一次打开时的stycle 上一次打开的位置
void listBook() {}

// 书籍内容，书籍章节

// 列出所有章节
void listChapter() {}

// 解析书籍获得章节列表保存在数据库
void parseBook() {}

// 从服务器上下载书籍，支持批量
void downloadBook(std::vector<std::string> bookId) {}

// 风格表(风格具体内容存milvus)风格表存在mysql

// 上传风格图片
void uploadStyleImage(std::string imagUrl) {}

// 下载风格图片
void downloadStyleImage(std::string imagUrl) {}

// 删除风格图片
void deleteStyleImage(std::string imagUrl) {}

// 列出风格图片列表
void listStyleImage() {}

// 提取风格表
void extractStycleStable(std::string imagUrl) {}

// 解析风格，把书籍文字正确排布
void parseStyle(std::string imagUrl) {}

// 检索风格，可以用一张图检索，也可以用文字检索
void searchStyle(std::string imagUrl) {}

// #include "Neo4j-cpp-driver/api.hpp"
#include <curl/curl.h>
#include <string.h>

#include <nlohmann/json.hpp>
#include <sstream>

struct Neo4jClient {
    std::string url;
    int port;
};

size_t write_data(void *ptr, size_t size, size_t nmemb, void *stream) {
    std::string data((const char *)ptr, (size_t)size * nmemb);
    *((std::stringstream *)stream) << data << std::endl;
    return size * nmemb;
}
using json = nlohmann::json;
// 给定一些请求字符串构造curl的请求的json字符串
std::string makeCypherRequest(std::vector<std::string> statements) {
    // strint '{"statements" : [ {    "statement" : "match (n) return n"  }  ]'
    std::vector<std::initializer_list<std::string>> stats;
    json req;
    for (auto statement : statements) {
        std::map<std::string, std::string> j{{"statement", statement}};
        req["statements"].emplace_back(j);
    }
    return req.dump();
}

// 创建node
void createNode(std::map<std::string, std::string> properties) {
    // 必须传入节点的名称
    if (properties.find("name") == properties.end()) {
    }

    // 硬拼请求
    std::string statement = "create ( n:" + properties["name"];
    if (properties.size() == 1) {
        statement = statement + " )";
    } else {
        // 拼上property
        int index = 0;
        statement = statement + "{";
        for (auto [first, second] : properties) {
            if (first != "name") {
                statement = statement + first + ":\"" + second + "\"";
            }

            if (index + 1 != properties.size()) {
                statement = statement + ",";
            } else {
                statement = statement + " })";
            }
            index = index + 1;
        }
    }
    struct curl_slist *headers = NULL;
    headers = curl_slist_append(headers, "Content-Type: application/json");
    auto req = makeCypherRequest({statement});
    auto handle = curl_easy_init();
    curl_easy_setopt(handle, CURLOPT_URL, "http://192.168.1.2:7474/db/neo4j/tx/commit");
    curl_easy_setopt(handle, CURLOPT_POSTFIELDS, req.c_str());
    curl_easy_setopt(handle, CURLOPT_POSTFIELDSIZE, strlen(req.c_str()));
    curl_easy_setopt(handle, CURLOPT_HTTPHEADER, headers);

    auto success = curl_easy_perform(handle);
    std::cout << success;
}

// 解析返回的json字符串
std::vector<json> getItemsFromJson(std::string jsonStr) {
    struct curl_slist *headers = NULL;
    headers = curl_slist_append(headers, "Content-Type: application/json");
    std::stringstream out;
    auto handle = curl_easy_init();
    curl_easy_setopt(handle, CURLOPT_URL, "http://192.168.1.2:7474/db/neo4j/tx/commit");
    curl_easy_setopt(handle, CURLOPT_POSTFIELDS, jsonStr.c_str());
    curl_easy_setopt(handle, CURLOPT_POSTFIELDSIZE, strlen(jsonStr.c_str()));
    curl_easy_setopt(handle, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, write_data);
    curl_easy_setopt(handle, CURLOPT_WRITEDATA, &out);
    auto success = curl_easy_perform(handle);
    std::string result = out.str();
    // std::cout << result;
    json json_result = json::parse(result);
    std::vector<json> items;
    for (auto &item : json_result["results"][0]["data"]) {
        if (item != NULL) {
            items.push_back(item["row"][0]["name"]);
        }
    }
    return items;
}

#include <spdlog/spdlog.h>

#include <memory>
#include <unordered_map>
#include <valarray>
class neo4j_query {
    neo4j_query() { statement = ""; }

    // 查询条件
    template <class M, class... T>
    std::shared_ptr<neo4j_query> where(std::string str, M para, T... para_next) {}

    // 排序规则
    std::shared_ptr<neo4j_query> order(std::string orderBy) {}

    // 数量上限
    std::shared_ptr<neo4j_query> limit(int limit) {}

    // 返回的node和relation
    std::shared_ptr<neo4j_query> get(std::vector<std::string> labels) {
        if (labels.size() == 0) {
            return nullptr;
        }
        statement += "return ";
        for (auto &label : labels) {
            statement += label;
            statement = statement + ",";
        }
        statement = statement.substr(0, statement.size() - 1);
        statement = statement + ";";
    }

    // 匹配的字段，最终返回的结果
    std::vector<json> find(std::vector<std::string> labels) {
        using namespace std::literals;
        // 找出所有的node
        std::vector<std::string> node;
        std::vector<std::string> relation;
        std::vector<std::string> matchexpressions;
        for (auto &label : labels) {
            // 单独的节点
            if (label.substr(0, 2) == "n:") {
                node.push_back(label.substr(2));
                mapNode[label.substr(2)] = true;
                matchexpressions.push_back("(" + label.substr(2) + ")");
            } else {
                matchexpressions.push_back(label);
                // 有match关系的节点

                // 检查语法的正确性,暂不启用
                // auto index = 0;
                // enum class ParseState {
                //     NONE,
                //     START,
                //     Node,
                //     Relation,
                //     MultiJump,
                //     NodeToRelation,
                //     RelationToNode,
                //     END,
                // };
                // ParseState state = ParseState::START;
                // 记录了node或者relation的上一个位置
                // auto preIdex = 0;
                // for (auto index = 0; index < label.length(); index++) {
                //     auto letter = label[index];
                //     if (index == 0 && letter != '(') {
                //         spdlog::error("[neo4j_query::find] 语法错误");
                //         return {};
                //     }
                //     if (letter == '(' || letter == ')' || letter == '[' || letter == ']') {
                //         continue;
                //     }
                //     if (state == ParseState::START || state == ParseState::Node) {
                //         if (isalnum(letter)) {
                //             index++;
                //             continue;
                //         } else if (letter == '-' || letter == '<') {
                //             if (index + 1 < label.length()) {
                //                 if (label[index + 1] != ')') {
                //                     spdlog::error("[neo4j_query::find] 语法错误");
                //                 }
                //             } else {
                //                 spdlog::error("[neo4j_query::find] 语法错误");
                //             }
                //             index = index - 1;
                //             state = ParseState::NodeToRelation;
                //         } else {
                //             spdlog::error("[neo4j_query::find] 语法错误");
                //             return {};
                //         }
                //     } else if (state == ParseState::NodeToRelation) {
                //         if (letter == '-') {
                //             if (index + 1 < label.length()) {
                //                 if (label[index + 1] != '[') {
                //                     spdlog::error("[neo4j_query::find] 语法错误");
                //                 }
                //             } else {
                //                 spdlog::error("[neo4j_query::find] 语法错误");
                //             }
                //             state = ParseState::Relation;
                //         }
                //     } else if (state == ParseState::Relation) {
                //         if (isalnum(letter)) {
                //             index++;
                //             continue;
                //         } else if (letter == '-' || letter == '>') {
                //             if (index + 1 < label.length()) {
                //                 if (label[index + 1] != ')') {
                //                     spdlog::error("[neo4j_query::find] 语法错误");
                //                 }
                //             } else {
                //                 spdlog::error("[neo4j_query::find] 语法错误");
                //             }
                //             index = index - 1;
                //             state = ParseState::RelationToNode;
                //         } else {
                //             spdlog::error("[neo4j_query::find] 语法错误");
                //             return {};
                //         }
                //     } else if (state == ParseState::RelationToNode) {
                //     }
                // }
            }
        }

        std::string match = "match(";
        for (auto matchexpression : matchexpressions) {
            match = match + matchexpression + ",";
        }
        match = match.substr(0, match.length() - 1);
        match = match + ")";
        statement = match + statement;
    }

  private:
    std::string statement;
    std::unordered_map<std::string, bool> mapNode;
    std::unordered_map<std::string, bool> mapRelation;
};

#include <any>
#include <odb/database.hxx>
#include <odb/mysql/database.hxx>

#include "BookServer/Public/book.h"
std::pair<std::any, std::string> returntest() {
    struct {
        int i;
        int j;
    } p2{.i = 3, .j = 5};

    struct student {
        std::string name;
        int id;
    };
    auto i = student{.name = "张三", .id = 50035};
    return {p2, "一种情况"};
}

#include "aws-sdk/Public/file.h"

// 主函数
int main() {
    using namespace std;
    returntest();
    // std::cout << test1 << test2;
    try {
        std::shared_ptr<odb::mysql::database> db{new odb::mysql::database("root", "123456789adj", "shuyun", "192.168.1.2", 3306)};
        CreateBook(db, "好书", 1, 1);
    } catch (std::exception &e) {
        std::cout << e.what();
    }
    std::cout << "done";
    return 0;
    // createNode({{"name", "国家"}, {"国家名称", "中国"}});

    // auto req = makeCypherRequest({"match (n) return n"});
    // std::cout << req;
    // auto rsp = getItemsFromJson(req);
    // for (auto item : rsp) {
    //     std::cout << item << std::endl;
    // }

    // json json_data = json::parse(out.str());
    // std::cout << json_data["results"][0]["data"][0]["row"][0]["name"];

    // std::cout << output << std::endl;
    // try {
    // std::shared_ptr<neo4jDriver::Neo4j> neo4j =
    // neo4jDriver::Neo4j::getNeo4j(); neo4jDriver::Neo4jAPI neo4jAPI(neo4j,
    // "10.1.0.31", "7474", "neo4j",
    //                                "123456789adj");
    // neo4jAPI.connectDatabase()
    // } catch (const exception& e) {
    //

    //     neo4jAPI.connectDatabase();

    //     std::cout << e.what() << std::endl;
    // }
    // neo4jAPI.createNode(NULL, "你好");

    // neo4j_client_init();

    // /* use NEO$J_INSECURE when connecting to disable TLS */
    // neo4j_connection_t* connection =
    //     neo4j_connect("neo4j://user:pass@localhost:7687", NULL,
    //     NEO4J_INSECURE);
    // if (connection == NULL) {
    //     neo4j_perror(stderr, errno, "connection failed");
    //     return EXIT_FAILURE;
    // }

    // neo4j_result_stream_t* results =
    //     neo4j_run(connection, "RETURN 'hello world'", neo4j_null);
    // if (results == NULL) {
    //     neo4j_perror(stderr, errno, "Failed to run statement");
    //     return EXIT_FAILURE;
    // }

    // neo4j_result_t* result = neo4j_fetch_next(results);
    // if (result == NULL) {
    //     neo4j_perror(stderr, errno, "Failed to fetch result");
    //     return EXIT_FAILURE;
    // }

    // neo4j_value_t value = neo4j_result_field(result, 0);
    // char buf[128];
    // printf("%s\n", neo4j_tostring(value, buf, sizeof(buf)));
    // neo4j_close_results(results);
    // neo4j_close(connection);
    // neo4j_client_cleanup();
    // return EXIT_SUCCESS;
}