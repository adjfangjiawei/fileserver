#ifndef SERVER_BOOK_H
#define SERVER_BOOK_H
#include <gateway/gateway.h>

#include <BookServer/jb/book.hxx>
#include <odb/database.hxx>
#include <odb/mysql/database.hxx>
#include <odb/transaction.hxx>
#include <string>
#include <unordered_map>

using database = std::shared_ptr<odb::mysql::database>;
using transaction = odb::core::transaction;
// 书籍,书籍阅读位置，书籍切分，书籍风格

// 创建书籍
extern JbBook* CreateBook(database db, JbBook* req);

// 获取书籍
extern JbBook* GetBook(database db, JbGetBookRequest* req);

// 创建作者
extern JbAuthor* CreateAuthor(database db, JbAuthor* req);

// // 设置软件的语言
// void setLanguage();

// // 设置书籍翻页方式
// void setBookMode() {}

// // 设置书籍封面，分为手工上传与自动匹配
// void setBookCover() {}

// // 用户服务,创建一个用户，暂不考虑用户共享
// void createuser() {}

// // 标签服务，用于方便探索书籍中的对象的关系，像人物，地点，分析书籍之间对象的联系

// // 用于创建一个标签，存入mysql中，作为neo4j的结点名
// void createTag() {}

// // 绑定标签,创建一个节点,节点名即为tag,并且拥有content的property
// void bindTag(std::string tag, std::string property, std::string content) {}

// // 删除标签,删除一个节点,节点名即为tag,这会删除mysql的表以及所有的这个tag的节点，以及与这个节点有关的连接
// void deleteTag(std::string tag) {}

// // 将一个节点连接到另一个节点，比如林黛玉是红楼梦里的人物就是有个连接,connectproperty是连接属性，上面的例子connectproperty即为人物
// void connectTag(std::string tag1, std::string tag2, std ::string connectproperty) {}

// // 获得与此节点的所有连接的节点，排序规则待定
// void getConnectedTag(std::string tag, std::vector<std::string> properties) {}

// // 笔记服务，云笔记

// // 预取书籍，流式从服务器上拉取书籍观看
// void prefetchBook(std::string bookId) {}

// // 删除书籍
// void deleteBook(std::string bookId) {}

// // 列出书籍
// // 支持过滤选项
// // 最近n天打开过
// // 最近n天的打开次数
// // 所属的用户名，支持多选，不提供则检索所有
// // 返回值
// // 书籍的名称 封面图片的url 上一次打开时的stycle 上一次打开的位置
// void listBook() {}

// // 书籍内容，书籍章节

// // 列出所有章节
// void listChapter() {}

// // 解析书籍获得章节列表保存在数据库
// void parseBook() {}

// // 从服务器上下载书籍，支持批量
// void downloadBook(std::vector<std::string> bookId) {}

// // 风格表(风格具体内容存milvus)风格表存在mysql

// // 上传风格图片
// void uploadStyleImage(std::string imagUrl) {}

// // 下载风格图片
// void downloadStyleImage(std::string imagUrl) {}

// // 删除风格图片
// void deleteStyleImage(std::string imagUrl) {}

// // 列出风格图片列表
// void listStyleImage() {}

// // 提取风格表
// void extractStycleStable(std::string imagUrl) {}

// // 解析风格，把书籍文字正确排布
// void parseStyle(std::string imagUrl) {}

// // 检索风格，可以用一张图检索，也可以用文字检索
// void searchStyle(std::string imagUrl) {}
#endif