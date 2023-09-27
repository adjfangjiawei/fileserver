
#include <odb/database.hxx>
#include <odb/mysql/database.hxx>
#include <odb/transaction.hxx>
#include <string>

using database = std::shared_ptr<odb::mysql::database>;
using transaction = odb::core::transaction;
// 书籍,书籍阅读位置，书籍切分，书籍风格
extern const char* CreateBook(database db, std::string bookName,
                              unsigned long authorID,
                              unsigned long publishHomeID);