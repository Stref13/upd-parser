#pragma once

//#include <optional>
#include "money.hpp"
#include <optional>
#include <string>
#include <vector>

// Участник сделки продавец или покупатель
struct Party
{
    std::string Name;  // Наименование организации
    std::string inn;   // ИННЮЛ (10 цифр) ИННФЛ (12 цифр)
    std::string kpp;   // КПП (только у организации)
    bool is_IP = false;
};

struct Item
{
    int line_number;                // НомСтр
    std::string name;               // НаимТов
    std::string unit_code;          // ОКЕИ_Тов, код единицы измерения
    std::string unit_name;          // НаимЕдИзм
    std::string quantity;           // КолТов
    std::string price;              // ЦенаТов
    Money sum_without_nds;          // СтТовБезНДС
    std::string nds_rate;           // НалСт: "22%", "10%", "без НДС"
    std::optional<Money> nds;       // СумНал; пусто, если "без НДС"
    Money sum_with_nds;             // СтТовУчНал
};

struct Totals
{
    Money without_nds;          // Итоговая сумма без НДС
    std::optional<Money> nds;  // Сумма НДС  
    Money with_nds;             // Итоговая сумма с НДС
};

struct Upd
{
    std::string file_id;        // Файл/@ИдФайл
    std::string format_version; // Файл/@ВерсФорм
    std::string function;       // Документ/@Функция: СЧФ, ДОП, СЧФДОП
    std::string number;         // СвСчФакт/@НомерДок
    std::string date;           // СвСчФакт/@ДатаДок, формат ДД.ММ.ГГГГ
    std::string currency_code;  // ДенИзм/@КодОКВ, 643 = рубль
    Party seller;               // СвПрод
    Party buyer;                // СвПокуп
    std::vector<Item> items;    // ТаблСчФакт/СведТов
    Totals totals;              // ТаблСчФакт/ВсегоОпл

};
