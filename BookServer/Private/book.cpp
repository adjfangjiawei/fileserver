#include <BookServer/book.h>
#include <Neo4j/create.h>
#include <spdlog/spdlog.h>
#include <utils/util.h>

#include <BookServer/db/book-odb.hxx>
#include <BookServer/db/nbook.hxx>
#include <jbson/Book/book.hxx>
#include <odb/database.hxx>
#include <odb/mysql/database.hxx>
#include <odb/transaction.hxx>
#include <string>
using namespace std::literals::string_literals;
JbBook* CreateBook(database db, JbBook* req) {
    auto requestID = utils::GetRequestID();
    spdlog::logger logfield{"bookserver::CreateBook "s + std::to_string(requestID)};
    logfield.info("CreateBook enter,requestID {}", requestID);

    // 输入契约
    if constexpr (enable_contract) {
        // 书名非空
        if (req->display_name.empty()) {
            logfield.error("CreateBook failed,display_name is empty");
            return nullptr;
        }
        // 未知状态
        if (req->status == JbBook::BookStatus::UNKNOWN) {
            logfield.error("CreateBook failed,status is UNKNOWN");
            return nullptr;
        }
        logfield.info("CreateBook input contract ok");
    }

    // 创建一本书的数据库记录
    dbBook book{
        .name = req->display_name,
        .issn = req->issn,
        .isbn = req->isbn,
        .chinese_category_big = static_cast<dbBook::ChineseBigCategory>(req->chinese_category_big),
        .chinese_category_specific = static_cast<dbBook::ChineseCategorySpecific>(req->chinese_category_specific),
    };
    auto bookId = 0;
    try {
        transaction t(db->begin());
        bookId = db->persist(book);
        t.commit();
    } catch (std::exception& e) {
        logfield.error("failed to create book ,because of db error:{}", e.what());
        std::rethrow_exception(std::current_exception());
        return nullptr;
    }

    auto AuthorId = utils::AuthorGetIDFromName(req->author_name);
    if (AuthorId == 0) {
        logfield.error("invalid author ID");
        return nullptr;
    }

    NbBook nbBook{.ID = static_cast<unsigned long>(bookId)};
    NbAuthor nbAuthor{.ID = AuthorId};
    NbBookAuthorRelation nbBookAuthorRelation{};
    CreateNode(&nbBook);
    CreateRelation(&nbBook, &nbAuthor, &nbBookAuthorRelation);

    // 创建出版社和这本书的关系,出版社出版了这本书
    if (req->publisher_id != 0) {
        NbBookPublisherRelation nbBookPublisherRelation;
        NbPublisher nbPublisher{.ID = req->publisher_id};
        CreateRelation(&nbBook, &nbPublisher, &nbBookPublisherRelation);
    }

    // 输出契约
    if constexpr (enable_contract) {
        try {
            // Id查询书名
            transaction tx{db->begin()};
            auto result = db->query<dbBook>(odb::query<dbBook>::id == bookId);
            if (result.size() != 1) {
                logfield.error("empty result");
                throw "contract execute failed";
                return nullptr;
            }
            // 校验结果
            auto book = result.begin();
            if (book->name != req->display_name || book->chinese_category_big != static_cast<dbBook::ChineseBigCategory>(req->chinese_category_big) ||
                book->chinese_category_specific != static_cast<dbBook::ChineseCategorySpecific>(req->chinese_category_specific) || book->issn != req->issn || book->isbn != req->isbn ||
                book->id_ == 0) {
                logfield.error("contract failed");
                throw "contract execute failed";
                return nullptr;
            }
        } catch (std::exception& e) {
            logfield.error("failed to query book ,because of db error:{}", e.what());
            std::rethrow_exception(std::current_exception());
            return nullptr;
        }
    }
    return nullptr;

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
        db->persist(author);
        tx.commit();
    } catch (std::exception& e) {
        logfield.info("CreateAuthor failed");
        return nullptr;
    }

    // 在Neo4j中创建一个作者节点
    NbAuthor nAthor{.ID = author.id_};
    CreateNode(&nAthor);

    return author.ToPb();
}
// 批量创建书
const char* BatchCreateBook() {}

void AnalysisBook() {
    // 使用ICU分词
    // 对每一个词创建Neo4j节点,存储词

    // 使用ICU分句
    // 对每一句话创建Neo4j节点,仅存储Id,并把词编码后存入milvus

    // 使用ICU分段
}

void AnalysisPaperStycle() {
    // 对纸张检测，识别出所有横线
}