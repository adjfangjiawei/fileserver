#include <Neo4j/create.h>
#include <gateway/gateway.h>
#include <gtest/gtest.h>

#include <BookServer/db/nbook.hxx>
#include <memory>
// int myfunctions() { return 0; }
// int add(int a, int b) { return a + b; }
// TEST(myfunctions, add) { GTEST_ASSERT_EQ(add(10, 22), 32); }

int main(int argc, char* argv[]) {
    auto query = std::make_shared<neo4j_query>();
    std::vector<NbBook> book;
    query->get(book);
    //   clientMain();
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
