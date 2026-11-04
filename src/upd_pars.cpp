#include "upd_pars.hpp"
#include "money.hpp"
#include "upd_model.hpp"
#include <optional>
#include <pugixml.hpp>
#include <string>
#include <string_view>

namespace {

// Путь вида "Файл/Документ/СвСчФакт" для сообщений об ошибках.
std::string path_of(pugi::xml_node node)
{
    std::string path;
    for (;node && node.type() == pugi::node_element; node = node.parent())
    {
        if(path.empty())
        {
            path = node.name();
        }
        else {
            path = std::string(node.name()) + "/" + path;
        }
    }
    return path;
}

// Дочерний элемент, который обязан быть.
pugi::xml_node required_child(pugi::xml_node parent,const char* name)
{
    pugi::xml_node child = parent.child(name);
    if(!child)
    {
        throw ParseError("Нет обязательного элемента " + path_of(parent) + "/" + name);
    }
    return child;
}

std::string required_attribute (pugi::xml_node node, const char* attr)
{
    pugi::xml_attribute attribute = node.attribute(attr);
    if(!attribute || std::string_view(attribute.value()).empty())
    {
        throw ParseError("Нет обязательного аттрибута: " + path_of(node) + "/@"+attr);
    }
    return attribute.value();
}

Money required_money_attr(pugi::xml_node node, const char* name)
{
    std::string attr = required_attribute(node, name);
    std::optional<Money> m = parse_money(attr);
    if (!m)
    {
        throw ParseError("Не сумма в " + path_of(node) + 
                    ": " + attr);
    }
    else
    {
        return *m;
    }
}

std::optional<Money> parse_nds_choice(pugi::xml_node node)
{
    if(node.child("БезНДС")) return std::nullopt;

    pugi::xml_node ch = required_child(node, "СумНал");
    auto money= parse_money(ch.text().as_string());
    if(!money)
    {
        throw ParseError("Не сумма в " + path_of(ch));
    }
    return money;

}

Party parse_party(pugi::xml_node node)
{
    pugi::xml_node id = required_child(node,"ИдСв");
    Party party;

    if(pugi::xml_node org = id.child("СвЮЛУч"))
    {
        party.Name = required_attribute(org, "НаимОрг");
        party.inn  = required_attribute(org, "ИННЮЛ");
        party.kpp  = id.attribute("КПП").value();
    }
    else if(pugi::xml_node ip = id.child("СвИП"))
    {
        party.is_IP = true;
        party.inn  = required_attribute(ip, "ИННФЛ");
        pugi::xml_node fio = required_child(ip, "ФИО");
        party.Name = "ИП" + required_attribute(fio, "Фамилия") + required_attribute(fio, "Имя");
        if (auto patronymic = fio.attribute("Отчество")) {
            party.Name += std::string(" ") + patronymic.value();
        }
    }
    else 
    {
        throw ParseError("неизвестный тип участника в " + path_of(id) + 
                            " (поддерживаются СвЮЛУч и СвИП)");

    }
    return party;
}

Item parse_item(pugi::xml_node node)
{
    Item item;

    item.line_number = node.attribute("НомСтр").as_int();
    item.name = required_attribute(node, "НаимТов");
    item.unit_code = node.attribute("ОКЕИ_Тов").value();
    item.unit_name = node.attribute("НаимЕдИзм").value();
    item.quantity = node.attribute("КолТов").value();
    item.price = node.attribute("ЦенаТов").value();
    item.sum_without_nds = required_money_attr(node, "СтТовБезНДС");
    item.nds_rate = required_attribute(node, "НалСт");
    item.sum_with_nds = required_money_attr(node, "СтТовУчНал");
    item.nds = parse_nds_choice(required_child(node, "СумНал"));

    return item;
}

} // namespace

Upd parse_upd(const char* path)
{
    // pugi::xml_document doc;
    // pugi::xml_parse_result result = doc.load_buffer(xml_utf8.data(), xml_utf8.size());
    // if(!result)
    // {
    //     throw ParseError(std::string("некорректный XML: ") + result.description() +
    //                     " (байт " + std::to_string(result.offset)+")");
    // }
    pugi::xml_document doc;
    pugi::xml_parse_result result = doc.load_file(path);

    if (!result) {
        throw ParseError(std::string("некорректный XML: ") + result.description() +
                        " (байт " + std::to_string(result.offset)+")");
    }

    pugi::xml_node file = required_child(doc, "Файл");
    pugi::xml_node document = required_child(file,"Документ");
    pugi::xml_node invoice = required_child(document,"СвСчФакт");
    pugi::xml_node table_sch = required_child(document, "ТаблСчФакт");
    pugi::xml_node total_money = required_child(table_sch, "ВсегоОпл");

    Upd upd;
    upd.file_id = required_attribute(file, "ИдФайл");
    upd.format_version = required_attribute(file, "ВерсФорм");
    upd.function = required_attribute(document, "Функция");
    upd.date = required_attribute(invoice, "ДатаДок");
    upd.number = required_attribute(invoice, "НомерДок");
    upd.currency_code = required_attribute(required_child(invoice, "ДенИзм"), "КодОКВ");
    upd.seller = parse_party(required_child(invoice,"СвПрод")); 
    upd.buyer = parse_party(required_child(invoice,"СвПокуп"));
    for(auto row : table_sch.children("СведТов"))
    {
        upd.items.push_back(parse_item(row));
    }
    upd.totals.without_nds = required_money_attr(total_money, "СтТовБезНДСВсего");
    upd.totals.with_nds = required_money_attr(total_money, "СтТовУчНалВсего");
    upd.totals.nds = parse_nds_choice(required_child(total_money, "СумНалВсего"));


    return upd; 
}