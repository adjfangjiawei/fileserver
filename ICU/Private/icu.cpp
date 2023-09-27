#include <unicode/basictz.h>
#include <unicode/brkiter.h>
#include <unicode/errorcode.h>
#include <unicode/locid.h>
#include <unicode/rbbi.h>
#include <unicode/translit.h>
#include <unicode/unistr.h>
#include <unicode/ustring.h>
#include <unicode/utypes.h>

#include <iostream>

// 获得词的发音
void getPronounce() {
    UErrorCode err = U_ZERO_ERROR;
    icu_74::Transliterator* myTrans = icu_74::Transliterator::createInstance("zh-Latin", UTRANS_FORWARD, err);
    icu_74::UnicodeString str;
    str.setTo("直接输出html代码而不是作为函数返回值代后处理");
    myTrans->transliterate(str);
    std::string st;
    str.toUTF8String(st);
    std::cout << st << std::endl;
}

// 对词进行分词
void listWordBoundaries(const icu_74::UnicodeString& s) {
    UErrorCode status = U_ZERO_ERROR;
    icu_74::BreakIterator* bi = icu_74::BreakIterator::createWordInstance(icu_74::Locale::getUS(), status);
    bi->setText(s);
    int32_t p = bi->first();
    while (p != icu_74::BreakIterator::DONE) {
        printf("Boundary at position %d\n", p);
        p = bi->next();
    }
    delete bi;
}

// 这个函数通常用来双击选词，可以找到这个词的词尾
void wordContaining(icu_74::BreakIterator& wordBrk, int32_t idx, const icu_74::UnicodeString& s, int32_t& start, int32_t& end) {
    // this function is written to assume that we have an
    // appropriate BreakIterator stored in an object or a
    // global variable somewhere-- When possible, programmers
    // should avoid having the create() and delete calls in
    // a function of this nature.
    if (s.isEmpty()) return;
    wordBrk.setText(s);
    start = wordBrk.preceding(idx + 1);
    end = wordBrk.next();
    // NOTE: for this and similar operations, use preceding() and next()
    // as shown here, not following() and previous(). preceding() is
    // faster than following() and next() is faster than previous()
    // NOTE: By using preceding(idx + 1) above, we're adopting the convention
    // that if the double-click comes right on top of a word boundary, it
    // selects the word that _begins_ on that boundary (preceding(idx) would
    // instead select the word that _ends_ on that boundary).
}

// 判断一个词语是否是完整的词语
UBool isWholeWord(icu_74::BreakIterator& wordBrk, const icu_74::UnicodeString& s, int32_t start, int32_t end) {
    if (s.isEmpty()) return false;
    wordBrk.setText(s);
    if (!wordBrk.isBoundary(start)) return false;
    return wordBrk.isBoundary(end);
}

// 词数统计
int32_t containsLetters(icu_74::RuleBasedBreakIterator& bi, const icu_74::UnicodeString& s, int32_t start) {
    bi.setText(s);
    int32_t count = 0;
    while (start != icu_74::BreakIterator::DONE) {
        int breakType = bi.getRuleStatus();
        if (breakType != UBRK_WORD_NONE) {
            // Exclude spaces, punctuation, and the like.
            // A status value UBRK_WORD_NONE indicates that the boundary does
            // not start a word or number.
            //
            ++count;
        }
        start = bi.next();
    }
    return count;
}

#include <unicode/numberformatter.h>
#include <unicode/numberrangeformatter.h>
// 距离格式化
void distanceFormat() {
    UErrorCode status;
    // en-GB 英国
    // zh-CN 简体中文
    auto formatString = icu::number::NumberRangeFormatter::with()
                            //.identityFallback(UNUM_IDENTITY_FALLBACK_APPROXIMATELY)
                            .numberFormatterFirst(icu::number::NumberFormatter::with().adoptUnit(icu::MeasureUnit::createMeter(status)))
                            .numberFormatterSecond(icu::number::NumberFormatter::with().adoptUnit(icu::MeasureUnit::createKilometer(status)))
                            .locale("en-GB")
                            .formatFormattableRange(1200.00000000001, 1.2, status)
                            .toString(status);
    std::string utf8String;
    formatString.toUTF8String(utf8String);
    std::cout << utf8String << std::endl;
}

#include <unicode/datefmt.h>
#include <unicode/dtfmtsym.h>
#include <unicode/dtptngen.h>
#include <unicode/smpdtfmt.h>
// 日期操作
void dateFormat() {
    std::string utf8String = "";
    UErrorCode status;
    auto formattedNumber = icu::number::NumberFormatter::with().adoptUnit(icu::MeasureUnit::createSecond(status)).locale("zh-CN").formatInt(100, status).toString(status);
    std::cout << formattedNumber.toUTF8String(utf8String) << std::endl;

    /* SHORT is numeric, such as 12/13/52 or 3:30pm
     MEDIUM is longer, such as Jan. 12, 1952
     LONG is longer, such as January 12, 1952 or 3:30:32pm
     FULL is completely specified, such as Tuesday, April 12, 1952 AD or 3:30:42pm PST*/
    icu_74::DateFormat* df = icu_74::DateFormat::createDateInstance(icu_74::DateFormat::LONG, icu::Locale::getChinese());
    UDate myDateArr[] = {0.0, 100000000.0, 2000000000.0};
    icu::UnicodeString myString;
    for (int32_t i = 0; i < 3; ++i) {
        myString.remove();
        df->format(myDateArr[i], myString);
        utf8String = "";
        myString.toUTF8String(utf8String);
        std::cout << utf8String << std::endl;
    }
    status = U_ZERO_ERROR;
#define U_SHOW_CPLUSPLUS_API 1
    icu::DateTimePatternGenerator* generator = icu::DateTimePatternGenerator::createInstance(icu::Locale::getChinese(), status);
    if (U_FAILURE(status)) {
        return;
    }
    // UNICODE LOCALE DATA MARKUP LANGUAGE
    // get a pattern for an abbreviated month and day
    icu::UnicodeString pattern = generator->getBestPattern(icu::UnicodeString("YYYMMd"), status);
    icu::SimpleDateFormat* formatter = new icu_74::SimpleDateFormat(pattern, icu::Locale::getChinese(), status);
    // use it to format (or parse)
    icu::UnicodeString formatted;
    formatted = formatter->format(icu::Calendar::getNow(), formatted, status);
    // for French, the result is "13 sept."
    utf8String = "";
    formatted.toUTF8String(utf8String);
    std::cout << utf8String << std::endl;
}

#include <unicode/gregocal.h>
// 日历表
void calender() {
    UErrorCode status;
    // calendar
    icu_74::DateFormat* df = icu_74::DateFormat::createDateInstance(icu_74::DateFormat::LONG, icu::Locale::getChinese());
    auto timeZone = icu::TimeZone::createTimeZone("Asia/Shanghai");
    icu::Calendar* calendarFormatter = new icu::GregorianCalendar(timeZone, status);
    calendarFormatter->setFirstDayOfWeek(UCAL_SUNDAY);
    calendarFormatter->set(2001, calendarFormatter->MARCH, 15);
    // month是0到11范围的
    std::cout << calendarFormatter->get(UCAL_YEAR, status) << " " << calendarFormatter->get(UCAL_MONTH, status) + 1 << " " << calendarFormatter->get(UCAL_DATE, status) << std::endl;
    std::cout << calendarFormatter->get(UCAL_DAY_OF_WEEK, status) << std::endl;
    auto birthDay = calendarFormatter->getTime(status);
    icu::UnicodeString birthDayUnicodeText;
    df->format(birthDay, birthDayUnicodeText);
    std::string birthDayUtf8String;
    birthDayUnicodeText.toUTF8String(birthDayUtf8String);
    std::cout << birthDayUtf8String << std::endl;
    calendarFormatter->add(UCAL_DATE, 10, status);
    auto tenDayAfterBirthDay = calendarFormatter->getTime(status);
    birthDayUnicodeText.remove();
    df->format(tenDayAfterBirthDay, birthDayUnicodeText);
    birthDayUtf8String = "";
    birthDayUnicodeText.toUTF8String(birthDayUtf8String);
    std::cout << birthDayUtf8String << std::endl;
}

// 资源bundle
void resourceBundle() {
    UErrorCode status;
    UResourceBundle* icuRoot = ures_open("H:/ICU学习/ICU/ICU/myapplication", "en", &status);
    auto y = ures_getByKey(icuRoot, "helpTopics", NULL, &status);
    int32_t length;
    UResourceBundle* currentZone1 = NULL;
    std::string yyy;
    auto z = ures_getStringByKey(y, "udata", &length, &status);

    icu::UnicodeString p = icu::UnicodeString(z);
    p.toUTF8String(yyy);
    std::cout << yyy;
}
// 字符串操作

// 编码转换