#include <map>
#include <string>
struct Neo4jClient {
    std::string url;
    int port;
};

// 创建Neo4j节点
template <class T>
extern void CreateNode(T *Node) {
    json node = Node;

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