

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