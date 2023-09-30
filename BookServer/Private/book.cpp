#include <BookServer/book.h>
#include <spdlog/spdlog.h>
#include <utils/util.h>

#include <BookServer/db/book-odb.hxx>
#include <BookServer/db/nbook.hxx>
#include <BookServer/jb/book.hxx>
#include <string>
using namespace std::literals::string_literals;
JbBook* CreateBook(database db, JbBook* req) {
    auto requestID = utils::GetRequestID();
    spdlog::logger logfield{"bookserver::CreateBook "s + std::to_string(requestID)};
    logfield.info("CreateBook enter,requestID {}", requestID);

    // 创建一本书的数据库记录
    dbBook book{
        .name = req->display_name,
        .issn = req->issn,
        .isbn = req->isbn,
        .chinese_category_big = static_cast<dbBook::ChineseBigCategory>(req->chinese_category_big),
        .chinese_category_specific = static_cast<dbBook::ChineseCategorySpecific>(req->chinese_category_specific),
    };

    try {
        transaction t(db->begin());
        auto bookId = db->persist(book);
        t.commit();
    } catch (std::exception& e) {
        logfield.error("failed to create book ,because of db error:{}", e.what());
        return nullptr;
    }

    //
    unsigned long authorId = 0;
    return nullptr;

    // // 创建一个出版社的Neo4j的节点
    // unsigned long publishHomeId = 0;

    // 创建Neo4j中的一本书的节点

    // 创建作者和这本书的关系,作者编写了这本书

    // 创建出版社和这本书的关系,出版社出版了这本书

    // 将书的封面图片保存在S3

    // 解析封面

    // 开始书的解析过程
    // return nullptr;
}

JbBook* GetBook(database db, JbGetBookRequest* req) {
    spdlog::logger logfield{"bookserver::GetBook"};
    logfield.info("GetBook enter");
    auto id = utils::BookGetIDFromName(req->book_name.c_str());
    if (id == 0) {
        logfield.info("CreateBook failed");
        return nullptr;
    }
    return nullptr;
}

// 创建作者
std::shared_ptr<JbAuthor> CreateAuthor(database db, JbAuthor* req) {
    spdlog::logger logfield{"bookserver::CreateAuthor"};
    logfield.info("CreateAuthor enter");
    // 校验输入
    if (req->display_name.length() == 0) {
        logfield.info("author need a name");
        return nullptr;
    }

    // 在mysql数据库中创建指定的作者
    dbAuthor author{.name = req->display_name,
                    .birth_date = req->birth_date,
                    .birth_country = req->birth_country,
                    .birth_place_detail = req->birth_place_detail,
                    .death_date = req->death_date,
                    .gender = static_cast<dbAuthor::Gender>(req->gender)};
    try {
        transaction tx(db->begin());
    } catch (std::exception& e) {
        logfield.info("CreateAuthor failed");
        return nullptr;
    }

    // 在Neo4j中创建一个作者节点
    NbBook nBook{.ID = author.id_};

    return author.ToPb();
}
// 批量创建书
// const char* BatchCreateBook() {}

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