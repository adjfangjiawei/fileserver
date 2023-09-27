
#include <odb/core.hxx>
#include <string>

// 书籍
#pragma db object
class Book {
  public:
    // 中国图书分类法大类分类
    enum class ChineseBigCategory : unsigned char {
#include "bookbigcategory.def"
    };

    // 中国图书分类法具体分类
    enum class ChineseCategorySpecific : unsigned short {
#include "bookspecifictype.def"
    };
    friend class odb::access;
#pragma db id auto
#pragma db index
    unsigned long id_;
    // 书名
    std::string name;
#pragma db index
    // issn编号
    unsigned long long issn;
#pragma db index
    // issn编号
    unsigned long long isbn;
    // 分类代码,采用美国国会图书馆图书分类法
#pragma db index
    // 分类代码,采用中国图书分类法,采用大类分类
    ChineseBigCategory ChineseCategoryBig;
#pragma db index
    // 分类代码,采用中国图书分类法,采用具体分类
    ChineseCategorySpecific ChineseCategorySpecific;
};
