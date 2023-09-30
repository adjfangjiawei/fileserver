#include <Neo4j/create.h>
#include <curl/curl.h>
#include <spdlog/spdlog.h>

#include <iostream>
#include <nlohmann/json.hpp>
#include <sstream>

size_t write_data(void *ptr, size_t size, size_t nmemb, void *stream) {
    std::string data((const char *)ptr, (size_t)size * nmemb);
    *((std::stringstream *)stream) << data << std::endl;
    return size * nmemb;
}
// 给定一些请求字符串构造curl的请求的json字符串
std::string makeCypherRequest(std::vector<std::string> statements) {
    // strint '{"statements" : [ {    "statement" : "match (n) return n"  }  ]'
    std::vector<std::initializer_list<std::string>> stats;
    nlohmann::json req;
    for (auto statement : statements) {
        std::map<std::string, std::string> j{{"statement", statement}};
        req["statements"].emplace_back(j);
    }
    return req.dump();
}

// 解析返回的json字符串
std::vector<nlohmann::json> getItemsFromJson(std::string jsonStr) {
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
    nlohmann::json json_result = nlohmann::json::parse(result);
    std::vector<nlohmann::json> items;
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
    std::vector<nlohmann::json> find(std::vector<std::string> labels) {
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