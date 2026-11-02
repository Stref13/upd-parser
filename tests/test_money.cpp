#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>

#include "money.hpp"

TEST_CASE("parse_money: рубли с копейками"){
    CHECK(parse_money("4500.00")->kopeek == 450000);
    CHECK(parse_money("0.5")->kopeek == 50);
    CHECK(parse_money("-12.34")->kopeek == -1234);
}

TEST_CASE("parse_money: неправильные строки"){
    CHECK_FALSE(parse_money("12,50"));
    CHECK_FALSE(parse_money("1.234"));
    CHECK_FALSE(parse_money(""));
    CHECK_FALSE(parse_money("abc"));
}

TEST_CASE("money to_string: обраное преобразование"){
    CHECK(to_string(Money(450000)) == "4500.00");
    CHECK(to_string(Money(5)) == "0.05");
    CHECK(to_string(Money(-1234)) == "-12.34");
}