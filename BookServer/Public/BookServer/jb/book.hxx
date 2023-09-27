#ifndef JB_BOOK_H
#define JB_BOOK_H

#include <iostream>
#include <nlohmann/json.hpp>
#include <string>

// 书籍
class JbBook {
  public:
    // 中国图书分类法大类分类
    enum class ChineseBigCategory : unsigned char {
#include "../db/bookbigcategory.def"
    };

    // 中国图书分类法具体分类
    enum class ChineseCategorySpecific : unsigned short {
#include "../db/bookspecifictype.def"
    };

    // 书籍资源名
    std::string name;

    // 书籍显示用的名称
    std::string display_name;

    // issn编号
    unsigned long long issn;

    // issn编号
    unsigned long long isbn;

    // 分类代码,采用中国图书分类法,采用大类分类
    ChineseBigCategory chinese_category_big;

    // 分类代码,采用中国图书分类法,采用具体分类
    ChineseCategorySpecific chinese_category_specific;

    // 书籍作者资源名
    std::string author_name;

    // 书籍状态
    enum class BookStatus : unsigned char {
        // 撰写中
        WRTING,
        // 撰写完毕，待发行
        WRTING_FINISHED,
        // 已经发行，且仍在流通
        PUBLISHED,
        // 已经绝版
        END_OF_LIFE,
    };

    // 书籍的状态
    BookStatus status;

    // 书籍出版的出版社,书籍状态为发行或者绝版时有效
    std::string publisher;

    // 书籍出版的出版人的资源名,书籍状态为发行或者绝版时有效
    std::string publisher_person_name;

    // 书籍出版的出版地的资源名,书籍状态为发行或者绝版时有效
    std::string publisher_place_name;

    // 书籍在中国国内定价,状态为已发行时有效.
    double price_in_chinese;
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(JbBook, name, display_name, issn, isbn, chinese_category_big, chinese_category_specific, author_name, status, publisher, publisher_person_name, publisher_place_name,
                                   price_in_chinese);
};

// 书籍标签
class JbBookTag {
  public:
    // 标签资源名
    std::string name;

    // 标签显示用的名称
    std::string display_name;

    // 标签类型
    enum class TagType : unsigned char {
        // 普通标签
        Normal,
        // 书籍作者标签
        Smart,
    };

    // 书籍作者最小年龄，标签类型为智能时有效
    unsigned short author_age_min;

    // 书籍作者最大年龄，标签类型为智能时有效
    unsigned short author_age_max;

    // 书籍最小字数
    unsigned short min_word_count;

    // 书籍最大字数
    unsigned short max_word_count;

    // 书籍最早发行时间

    // 书籍最晚发行时间

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(JbBookTag, name, display_name, author_age_min, author_age_max, min_word_count, max_word_count);
};

// 作者
class JbAuthor {
  public:
    // 作者资源名
    std::string name;

    // 作者显示用的名称
    std::string display_name;

    // 作者性别
    enum class Gender : unsigned char {
        // 未知性别
        Unknown,
        // 男性
        Male,
        // 女性
        Female,
    };

    // 出生日期
    std::string birth_date;

    // 出生国家
    std::string birth_country;

    // 出生地点详细地点
    std::string birth_place_detail;

    // 去世时间
    std::string death_date;

    // 作者性别
    Gender gender;
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(JbAuthor, name, display_name, birth_date, birth_country, birth_place_detail, death_date, gender);
};

// 删除书籍请求
class JbDeleteBookRequest {
  public:
    // 书籍资源名
    std::string book_name;
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(JbDeleteBookRequest, book_name);
};

// 获取书籍请求
class JbGetBookRequest {
  public:
    // 书籍资源名
    std::string book_name;
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(JbGetBookRequest, book_name);
};

// 获取书籍内容请求
class JbGetBookContentRequest {
  public:
    // 书籍资源名
    std::string book_name;

    // 章节资源名,如果不填则获取所有章节
    std::string chapter_name;
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(JbGetBookContentRequest, book_name, chapter_name);
};

// 批量获取书籍请求
class JbBatchGetBooksRequest {
  public:
    // 书籍资源名列表
    std::vector<std::string> book_names;
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(JbBatchGetBooksRequest, book_names);
};

// 列出书籍请求
class JbListBooksRequest {
  public:
    // 排序规则,默认按浏览量排序
    enum class SortRule : unsigned char {};
};

// 给书籍打上标签请求
#endif