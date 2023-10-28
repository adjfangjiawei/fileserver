
#include <unicode/ustring.h>
#include <utils/util.h>

#include <jbson/Book/book.hxx>
#include <odb/core.hxx>
#include <string>
// 书籍
#pragma db object
class dbBook {
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
    ChineseBigCategory chinese_category_big;
#pragma db index
    // 分类代码,采用中国图书分类法,采用具体分类
    ChineseCategorySpecific chinese_category_specific;
    auto ToPb() {
        auto book = new JbBook{.name = utils::BookGetNameFromID(id_),
                               .display_name = name,
                               .issn = issn,
                               .isbn = isbn,
                               .chinese_category_big = static_cast<JbBook::ChineseBigCategory>(chinese_category_big),
                               .chinese_category_specific = static_cast<JbBook::ChineseCategorySpecific>(chinese_category_specific)};
        return std::shared_ptr<JbBook>(book);
    }
};

// 作者
#pragma db object
class dbAuthor {
  public:
    friend class odb::access;
#pragma db id auto
#pragma db index
    unsigned long id_;
    // 作者名
    std::string name;
    // 出生日期
    std::string birth_date;
    // 出生国家
    std::string birth_country;
    // 出生地点详细地点
    std::string birth_place_detail;
    // 去世时间
    std::string death_date;
    // 作者性别
    enum class Gender : unsigned char {
        // 未知性别
        Unknown = 1,
        // 男性
        Male,
        // 女性
        Female,
    };
    Gender gender;
    auto ToPb() {
        auto author = new JbAuthor{.name = utils::AuthorGetNameFromID(id_),
                                   .display_name = name,
                                   .birth_date = birth_date,
                                   .birth_country = birth_country,
                                   .birth_place_detail = birth_place_detail,
                                   .death_date = death_date,
                                   .gender = gender == Gender::Male ? JbAuthor::Gender::Male : JbAuthor::Gender::Female};
        return std::shared_ptr<JbAuthor>(author);
    }
};

// 出版社
#pragma db object
class dbPublisher {
  public:
    friend class odb::access;
#pragma db id auto
#pragma db index
    unsigned long id_;
    // 出版社名
    std::string name;
    // 出版社地址
    std::string address;
    // 出版社电话
    std::string phone;
    // 出版社网址
    std::string url;
    // 出版社简介
    std::string intro;
};