#include "upd_pars.hpp"
#include "money.hpp"
#include "upd_model.hpp"
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
    return upd; 
}