#ifndef NEO4J_CREATE_H
#define NEO4J_CREATE_H

#include <curl/curl.h>
#include <stdlib.h>

#include <iostream>
#include <map>
#include <nlohmann/json.hpp>
#include <sstream>
#include <string>
#include <type_traits>
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
  public:
    neo4j_query() { wherestatement = ""; }

    std::shared_ptr<neo4j_query> jump(std::string str, int jumpNumber) {}

    // 查询条件
    template <class... T>
    std::shared_ptr<neo4j_query> where(std::string str, T... para_next) {
        return shared_from_this();
    }

    template <class M>
    std::shared_ptr<neo4j_query> where(std::string str, M para) {
        if (str.length() == 0) {
            return shared_from_this();
        }
        if (!alreadywhere) {
            wherestatement += " WHERE ";
            alreadywhere = true;
        }
        auto index = 0;
        for (auto &&ch : str) {
            if (ch != '?') {
                wherestatement += ch;
            } else {
                if constexpr (!std ::is_same_v<typename std::remove_reference<M>::type, const char *>) {
                    wherestatement += std::to_string(para);
                } else {
                    wherestatement += std::string(para);
                }
            }
            index = index + 1;
        }

        return shared_from_this();
    }
    template <class M, class... T>
    std::shared_ptr<neo4j_query> where(std::string str, M para, T... para_next) {
        if (str.length() == 0) {
            return shared_from_this();
        }
        if (!alreadywhere) {
            wherestatement += " WHERE ";
            alreadywhere = true;
        }
        auto index = 0;
        for (auto &&ch : str) {
            if (ch != '?') {
                wherestatement += ch;
            } else {
                if constexpr (!std ::is_same_v<typename std::remove_reference<M>::type, const char *>) {
                    wherestatement += std::to_string(para);
                } else {
                    wherestatement += std::string(para);
                }
                where(str.substr(index + 1), para_next...);
                break;
            }
            index = index + 1;
        }

        return shared_from_this();
    }

    // 排序规则
    std::shared_ptr<neo4j_query> order(std::string orderBy) { orderbystatement = " ORDER BY " + orderBy; }

    // 数量上限
    std::shared_ptr<neo4j_query> limit(int limit) {}

    template <class U, class... T>
    void getReturnStatement(U nodeOrRelation, T... nodeOrRelations) {
        using nodeOrRelationType = typename U::value_type;
        if constexpr (node_concept<nodeOrRelationType>) {
            returnstatement += "n" + std::to_string(nodeReturnIndex) + ",";
            nodeReturnIndex++;
        }
        if constexpr (relation_concept<nodeOrRelationType>) {
            returnstatement += "r" + std::to_string(relationReturnIndex) + ",";
            relationReturnIndex++;
        }
        getReturnStatement(nodeOrRelations...);
    }

    template <class U>
    void getReturnStatement(U nodeOrRelation) {
        using nodeOrRelationType = typename U::value_type;
        if constexpr (node_concept<nodeOrRelationType>) {
            returnstatement += "n" + std::to_string(nodeReturnIndex) + ",";
            nodeReturnIndex++;
        }
        if constexpr (relation_concept<nodeOrRelationType>) {
            returnstatement += "r" + std::to_string(relationReturnIndex) + ",";
            relationReturnIndex++;
        }
        returnstatement = returnstatement.substr(0, returnstatement.length() - 1);
    }

    // 返回的node和relation
    template <class U, class... T>
    std::shared_ptr<neo4j_query> get(U nodeOrRelation, T... nodeOrRelations) {
        returnstatement += "return ";
        getReturnStatement(nodeOrRelation, nodeOrRelations...);
        returnstatement = returnstatement + ";";
        // match边
        matchstatement += "match ";
        getMatchStatement(nodeOrRelation, nodeOrRelations...);
        auto req = makeCypherRequest({matchstatement + " " + wherestatement + " " + returnstatement});
        std::string jsonStr = req;
        struct curl_slist *headers = NULL;
        headers = curl_slist_append(headers, "Content-Type: application/json");
        std::stringstream out;
        auto handle = curl_easy_init();
        curl_easy_setopt(handle, CURLOPT_URL, "http://192.168.1.7:7474/db/neo4j/tx/commit");
        curl_easy_setopt(handle, CURLOPT_POSTFIELDS, jsonStr.c_str());
        curl_easy_setopt(handle, CURLOPT_POSTFIELDSIZE, strlen(jsonStr.c_str()));
        curl_easy_setopt(handle, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, write_data);
        curl_easy_setopt(handle, CURLOPT_WRITEDATA, &out);
        auto success = curl_easy_perform(handle);
        std::string result = out.str();
        std::cout << result;
        nlohmann::json json_result = nlohmann::json::parse(result);
        return shared_from_this();
    }

    template <class U, class... T>
    void getMatchStatement(U nodeOrRelation, T... nodeOrRelations) {
        using nodeOrRelationType = typename U::value_type;
        if constexpr (node_concept<nodeOrRelationType>) {
            matchstatement += "(n" + std::to_string(matchStatementIterNodeIndex) + ":" + nodeOrRelationType::node_name + ")" + ",";
            matchStatementIterNodeIndex++;
        } else if constexpr (relation_concept<nodeOrRelationType>) {
            matchstatement += "(rnb" + std::to_string(matchStatementIterRelationIndex) + ":" + nodeOrRelationType::nodeBegin + ")-[r" + std::to_string(matchStatementIterRelationIndex) + ":" +
                              nodeOrRelationType::relation_name + "]->(rne" + std::to_string(matchStatementIterRelationIndex) + ":" + nodeOrRelationType::nodeEnd + ")" + ",";
            matchStatementIterRelationIndex++;
        }

        getMatchStatement(nodeOrRelations...);
        return;
    }

    template <class U>
    void getMatchStatement(U nodeOrRelation) {
        using nodeOrRelationType = typename U::value_type;
        if constexpr (node_concept<nodeOrRelationType>) {
            matchstatement += "(n" + std::to_string(matchStatementIterNodeIndex) + ":" + nodeOrRelationType::node_name + ")" + ",";
            matchStatementIterNodeIndex++;
        } else if constexpr (relation_concept<nodeOrRelationType>) {
            matchstatement += "(rnb" + std::to_string(matchStatementIterRelationIndex) + ":" + nodeOrRelationType::nodeBegin + ")-[r" + std::to_string(matchStatementIterRelationIndex) + ":" +
                              nodeOrRelationType::relation_name + "]->(rne" + std::to_string(matchStatementIterRelationIndex) + ":" + nodeOrRelationType::nodeEnd + ")" + ",";
            matchStatementIterRelationIndex++;
        }

        matchstatement = matchstatement.substr(0, matchstatement.size() - 1);
        return;
    }

  private:
    std::string wherestatement;
    std::string matchstatement;
    std::string returnstatement;
    std::string orderbystatement;
    int nodeReturnIndex = 0;
    int relationReturnIndex = 0;
    int matchStatementIterNodeIndex = 0;
    int matchStatementIterRelationIndex = 0;
    bool alreadywhere{false};
    std::unordered_map<std::string, int> mapRelationJumpNumber;
};
#endif