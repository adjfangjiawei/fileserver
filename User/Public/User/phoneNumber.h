#ifndef PHONE_NUMBER_H
#define PHONE_NUMBER_H
// #include <phonenumbers/phonenumberutil.h>

// #include <iostream>
// #include <string>
// using i18n::phonenumbers::PhoneNumber;
// using i18n::phonenumbers::PhoneNumberUtil;
// int test() {
//     PhoneNumberUtil *phoneUtil = PhoneNumberUtil::GetInstance();  //初始化一个电话号码
//     PhoneNumber *pn = new PhoneNumber();
//     pn->set_country_code(86);
//     pn->set_national_number(13478808311);                                       //判断电话号码是否有效
//     std::cout << std::boolalpha << phoneUtil->IsValidNumber(*pn) << std::endl;  //判断电话号码所在的地区
//     std::string *region = new std::string();
//     phoneUtil->GetRegionCodeForNumber(*pn, region);
//     std::cout << *region << std::endl;
//     std::string *name = new std::string();
//     phoneUtil->GetNationalSignificantNumber(*pn, name);
//     std::cout << *name << std::endl;  //获取某个国家的国字区号
//     PhoneNumber *example = new PhoneNumber();
//     phoneUtil->GetInvalidExampleNumber("CN", example);
//     std::cout << example->country_code() << std::endl;  //释放内存
//     delete pn;
//     pn = nullptr;
//     delete region;
//     region = nullptr;
//     delete name;
//     name = nullptr;
//     delete example;
//     example = nullptr;

//     system("pause");
//     return 0;
// }
#endif