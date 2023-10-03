#ifndef NEO4J_CREATE_H
#define NEO4J_CREATE_H

#include <curl/curl.h>

#include <map>
#include <nlohmann/json.hpp>
#include <string>
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
void CreateNode(std::shared_ptr<T> Node) {
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
                    if (neo4j_node.value().type() == nlohmann::detail::value_t::string) {
                        statement = statement + neo4j_node.key() + ":\"" + neo4j_node.value().dump() + "\"";
                    }
                    if (neo4j_node.value().type() == nlohmann::detail::value_t::number_unsigned) {
                        statement = statement + neo4j_node.key() + ":" + neo4j_node.value().dump();
                    }
                    if (neo4j_node.value().type() == nlohmann::detail::value_t::number_integer) {
                        statement = statement + neo4j_node.key() + ":" + neo4j_node.value().dump();
                    }
                }
            }

            if (index + 2 == neo4j_nodes.size()) {
                neo4j_node++;
                if (neo4j_node.key() != "node_name") {
                    statement = statement + ",";
                    if (neo4j_node.value() != nullptr) {
                        if (neo4j_node.value().type() == nlohmann::detail::value_t::string) {
                            statement = statement + neo4j_node.key() + ":\"" + neo4j_node.value().dump() + "\"";
                        }
                        if (neo4j_node.value().type() == nlohmann::detail::value_t::number_unsigned) {
                            statement = statement + neo4j_node.key() + ":" + neo4j_node.value().dump();
                        }
                        if (neo4j_node.value().type() == nlohmann::detail::value_t::number_integer) {
                            statement = statement + neo4j_node.key() + ":" + neo4j_node.value().dump();
                        }
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
void CreateRelation(std::shared_ptr<T> node1, std::shared_ptr<U> node2, std::shared_ptr<R> relation) {
    nlohmann::json neo4j_node1 = *(node1);
    nlohmann::json neo4j_node2 = *(node2);
    nlohmann::json neo4j_relation = *(relation);
    std::string statement = "MATCH (n1:" + neo4j_node1["node_name"].get<std::string>() + "),(n2:" + neo4j_node2["node_name"].get<std::string>() + ")\n";
    auto index = 0;
    for (nlohmann::json::iterator neo4j_node = neo4j_node1.begin(); neo4j_node != neo4j_node1.end(); neo4j_node++) {
        index = index + 1;
        // where子句来对合适的顶点建立关系
        if (neo4j_node.key() != "node_name") {
            statement += "WHERE n1." + neo4j_node.key() + "=" + neo4j_node.value().get<std::string>();
        }
        if (index != neo4j_node1.size()) {
            statement += " AND ";
        }
    }

    if (neo4j_node2.size() != 0) {
        statement += " AND ";
        for (nlohmann::json::iterator neo4j_node = neo4j_node2.begin(); neo4j_node != neo4j_node2.end(); neo4j_node++) {
            index = index + 1;
            // where子句来对合适的顶点建立关系
            if (neo4j_node.key() != "node_name") {
                statement += "WHERE n2." + neo4j_node.key() + "=" + neo4j_node.value().get<std::string>();
            }
            if (index != neo4j_node2.size()) {
                statement += " AND ";
            }
        }
    }

    statement = statement + "\n";
    statement += "CREATE (n1)-[r:" + neo4j_relation["relation_name"].get<std::string>() + "->(n2)";
    return;
}
#endif