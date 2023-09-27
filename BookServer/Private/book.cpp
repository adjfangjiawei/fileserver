#include <spdlog/spdlog.h>

#include "../Public/book.h"
#include "sql/book-odb.hxx"

const char* CreateBook(database db, std::string bookName, unsigned long authorID, unsigned long publishHomeID) {
    // 创建一个作者的Neo4j的节点
    unsigned long authorId = 0;

    // 创建一个出版社的Neo4j的节点
    unsigned long publishHomeId = 0;

    // 创建一本书的数据库记录
    Book book{.name = bookName};
    try {
        transaction t(db->begin());
        auto bookId = db->persist(book);
        t.commit();
    } catch (std::exception& e) {
        spdlog::error("failed to create book ,because of db error:{}", e.what());
        return e.what();
    }

    // 创建Neo4j中的一本书的节点

    // 创建作者和这本书的关系,作者编写了这本书

    // 创建出版社和这本书的关系,出版社出版了这本书

    // 将书的封面图片保存在S3

    // 解析封面

    // 开始书的解析过程
}

// 批量创建书
const char* BatchCreateBook() {}

// void AnalysisBook() {
//     // 使用ICU分词
//     // 对每一个词创建Neo4j节点,存储词

//     // 使用ICU分句
//     // 对每一句话创建Neo4j节点,仅存储Id,并把词编码后存入milvus

//     // 使用ICU分段
// }

// void AnalysisPaperStycle() {
//     // 对纸张检测，识别出所有横线
// }