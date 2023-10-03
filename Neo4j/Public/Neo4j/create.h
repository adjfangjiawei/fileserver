#ifndef NEO4J_CREATE_H
#define NEO4J_CREATE_H

#include <curl/curl.h>
#include <stdlib.h>

#include <map>
#include <nlohmann/json.hpp>
#include <sstream>
#include <string>
#include <vector>
struct Neo4jClient {
    std::string url;
    int port;
};

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

// 创建Neo4j节点
template <class T>
void CreateNode(T *Node) {
    nlohmann::json neo4j_nodes = *(Node);

    // 必须传入节点的名称
    try {
        neo4j_nodes.at("node_name");
    } catch (std::exception &e) {
        std::rethrow_exception(std::current_exception());
        return;
    }

    // 硬拼请求
    std::string statement = "create ( n:" + neo4j_nodes["node_name"].get<std::string>();
    if (neo4j_nodes.size() == 1) {
        statement = statement + " )";
    } else {
        // 拼上property
        int index = 0;
        statement = statement + "{";
        for (nlohmann::json::iterator neo4j_node = neo4j_nodes.begin(); neo4j_node != neo4j_nodes.end(); ++neo4j_node) {
            if (neo4j_node.key() != "node_name") {
                if (neo4j_node.value() != nullptr) {
                    statement = statement + neo4j_node.key() + ":" + neo4j_node.value().dump();
                }
            }

            if (index + 2 == neo4j_nodes.size()) {
                neo4j_node++;
                if (neo4j_node.key() != "node_name") {
                    statement = statement + ",";
                    if (neo4j_node.value() != nullptr) {
                        statement = statement + neo4j_node.key() + ":" + neo4j_node.value().dump();
                    }
                }
                break;
            } else {
                statement = statement + ",";
            }
            index = index + 1;
        }
        statement = statement + " })";
        struct curl_slist *headers = NULL;
        headers = curl_slist_append(headers, "Content-Type: application/json");
        auto req = makeCypherRequest({statement});
        auto handle = curl_easy_init();
        curl_easy_setopt(handle, CURLOPT_URL, "http://192.168.43.127:7474/db/neo4j/tx/commit");
        curl_easy_setopt(handle, CURLOPT_POSTFIELDS, req.c_str());
        curl_easy_setopt(handle, CURLOPT_POSTFIELDSIZE, strlen(req.c_str()));
        curl_easy_setopt(handle, CURLOPT_HTTPHEADER, headers);

        auto success = curl_easy_perform(handle);
        std::cout << success;
    }
}

// 创建Neo4j关系
template <class T, class U, class R>
void CreateRelation(T *node1, U *node2, R *relation) {
    nlohmann::json neo4j_node1 = *(node1);
    nlohmann::json neo4j_node2 = *(node2);
    nlohmann::json neo4j_relation = *(relation);
    std::string statement = "MATCH (n1:" + neo4j_node1["node_name"].get<std::string>() + "),(n2:" + neo4j_node2["node_name"].get<std::string>() + ")\n";
    if (neo4j_node1.size() != 1 || neo4j_node2.size() != 1) {
        statement = statement + "where ";
    }
    auto index = 0;
    for (nlohmann::json::iterator neo4j_node = neo4j_node1.begin(); neo4j_node != neo4j_node1.end(); neo4j_node++) {
        // where子句来对合适的顶点建立关系
        if (neo4j_node.key() != "node_name") {
            statement += "n1." + neo4j_node.key() + "=" + neo4j_node.value().dump();
        }
        if (index + 2 != neo4j_node1.size()) {
            statement += " AND ";
        }
        if (index + 2 == neo4j_node1.size()) {
            neo4j_node++;
            if (neo4j_node.key() != "node_name") {
                statement = statement + " AND ";
                if (neo4j_node.value() != nullptr) {
                    statement = statement + "n1." + neo4j_node.key() + "=" + neo4j_node.value().dump();
                }
            }
            break;
        }
        index = index + 1;
    }

    if (neo4j_node2.size() != 0) {
        for (nlohmann::json::iterator neo4j_node = neo4j_node2.begin(); neo4j_node != neo4j_node2.end(); neo4j_node++) {
            // where子句来对合适的顶点建立关系
            if (neo4j_node.key() != "node_name") {
                statement = statement + " AND ";
                statement += "n2." + neo4j_node.key() + "=" + neo4j_node.value().dump();
            }
            if (index + 2 == neo4j_node1.size()) {
                neo4j_node++;
                if (neo4j_node.key() != "node_name") {
                    statement = statement + " AND ";
                    if (neo4j_node.value() != nullptr) {
                        statement = statement + "n2." + neo4j_node.key() + "=" + neo4j_node.value().dump();
                    }
                }
                break;
            }
            index = index + 1;
        }
    }
    statement = statement + "\n";
    statement += "CREATE (n1)-[r:" + neo4j_relation["relation_name"].get<std::string>() + "]" + "->(n2)";
    struct curl_slist *headers = NULL;
    headers = curl_slist_append(headers, "Content-Type: application/json");
    auto req = makeCypherRequest({statement});
    auto handle = curl_easy_init();
    curl_easy_setopt(handle, CURLOPT_URL, "http://192.168.43.127:7474/db/neo4j/tx/commit");
    curl_easy_setopt(handle, CURLOPT_POSTFIELDS, req.c_str());
    curl_easy_setopt(handle, CURLOPT_POSTFIELDSIZE, strlen(req.c_str()));
    curl_easy_setopt(handle, CURLOPT_HTTPHEADER, headers);

    auto success = curl_easy_perform(handle);
    return;
}

size_t write_data(void *ptr, size_t size, size_t nmemb, void *stream) {
    std::string data((const char *)ptr, (size_t)size * nmemb);
    *((std::stringstream *)stream) << data << std::endl;
    return size * nmemb;
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

template <class T>
concept node_concept = requires(T t) {
    {t.node_name};
};

template <class T>
concept relation_concept = requires(T t) {
    {t.relation_name};
};

class neo4j_query : public std::enable_shared_from_this<neo4j_query> {
    neo4j_query() { wherestatement = ""; }

    // 查询条件
    template <class M, class... T>
    std::shared_ptr<neo4j_query> where(std::string str, M para, T... para_next) {
        if (str.length() == 0) {
            return;
        }
        if (alreadywhere) {
            wherestatement += " AND ";
        } else {
            wherestatement += " WHERE ";
            alreadywhere = true;
        }
        auto index = 0;
        for (auto &&ch : str) {
            if (ch != '?') {
                wherestatement += ch;
            } else {
                wherestatement += std::to_string(para);
                where(str.substr(index + 1), para_next...);
            }
            index = index + 1;
        }

        wherestatement += str;
        return shared_from_this();
    }

    template <class... T>
    std::shared_ptr<neo4j_query> where(std::string str, T... para_next) {}

    // 排序规则
    std::shared_ptr<neo4j_query> order(std::string orderBy) {}

    // 数量上限
    std::shared_ptr<neo4j_query> limit(int limit) {}

    template <class U, class... T>
    std::string getReturnStatement(U nodeOrRelation, T... nodeOrRelations) {
        if constexpr (node_concept<U>) {
            returnstatement = "n" + std::to_string(nodeReturnIndex) + ",";
        }
        if constexpr (relation_concept<U>) {
            returnstatement = "r" + std::to_string(nodeReturnIndex) + ",";
        }
    }

    template <class U>
    std::string getReturnStatement(U nodeOrRelation) {
        if constexpr (node_concept<U>) {
            returnstatement = "n" + std::to_string(nodeReturnIndex) + ",";
        }
        if constexpr (relation_concept<U>) {
            returnstatement = "r" + std::to_string(nodeReturnIndex) + ",";
        }
        returnstatement = returnstatement.substr(0, returnstatement.length() - 1);
    }

    // 返回的node和relation
    template <class U, class... T>
    std::shared_ptr<neo4j_query> get(U nodeOrRelation, T... nodeOrRelations) {
        returnstatement += "return ";
        getReturnStatement(nodeOrRelation, nodeOrRelations...);
        returnstatement = returnstatement + ";";
        auto req = makeCypherRequest({returnstatement});
        getItemsFromJson(req);
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
        matchstatement = match + matchstatement;
    }

  private:
    std::string wherestatement;
    std::string matchstatement;
    std::string returnstatement;
    int nodeReturnIndex = 0;
    int relationReturnIndex = 0;
    bool alreadywhere{false};
    std::unordered_map<std::string, bool> mapNode;
    std::unordered_map<std::string, bool> mapRelation;
};
#endif