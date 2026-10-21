#pragma once

#include <string>
#include <string_view>
#include <cstdint>
#include <optional>

//Денежная сумма хранится в копейках, 
//чтобы не было ошибок округления
struct Money
{
    std::int64_t kopeek = 0;
    
    friend Money operator+ (Money a, Money b) 
    {   
        return Money{a.kopeek + b.kopeek};
    }
    Money& operator+=(Money other) 
    {
        kopeek += other.kopeek;
        return *this; 
    }
    friend auto operator<=>(const Money& a, const Money& b) = default;
};

// Разбирает строку вида "4500", "4500.5" или "4500.00".
// Возвращает std::nullopt, если строка не похожа на сумму.
std::optional<Money> parse_money(std::string_view text);

// Превращает сумму обратно в строку "4500.00".
std::string to_string(Money m);