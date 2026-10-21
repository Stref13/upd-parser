#include "money.hpp"
#include <charconv>
#include <optional>
#include <string>
#include <string_view>
#include <system_error>

std::optional<Money> parse_money(std::string_view text)
{
    if (text.empty()) return std::nullopt;

    bool negative = false;
    if(text.front() == '-')
    {
        negative = true;
        text.remove_prefix(1);
    }

    const auto dot = text.find('.');
    const std::string_view int_part = text.substr(0,dot);
    const std::string_view frac_part = 
        dot == std::string_view::npos ? std::string_view{} : text.substr(dot+1);
    
    if (int_part.empty() || frac_part.size() > 2) return std::nullopt;

    std::int64_t rubles = 0;
    auto [p1,ec1] = std::from_chars(int_part.data(),int_part.data()+int_part.size(),rubles);
    if(ec1 != std::errc{} || p1 != int_part.data() + int_part.size()) return std::nullopt;

    std::int64_t kop = 0;
    if(!frac_part.empty())
    {
        auto [p2,ec2] = std::from_chars(frac_part.data(),frac_part.data()+frac_part.size(),kop);
        if(ec2 != std::errc{} || p2 != frac_part.data() + frac_part.size()) return std::nullopt;
        if(frac_part.size() == 1) kop *= 10;
    }

    std::int64_t totals = rubles*100 + kop;
    return Money{negative ? -totals : totals};
};

std::string to_string(Money m)
{
    const bool negative = m.kopeek < 0;
    const std::int64_t abs = negative ? -m.kopeek : m.kopeek;
    std::string kop = std::to_string(abs%100);
    if(kop.size() == 1 ) kop.insert(kop.begin(),'0');
    return (negative ? "-" : "") + std::to_string(abs / 100) + "." + kop;
}