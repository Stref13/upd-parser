#include <iostream>
#include <ostream>
#include <pugixml.hpp>

int main() {
    pugi::xml_document doc;
    pugi::xml_parse_result result = doc.load_file("samples/upd_example_utf8.xml");

    if (!result) {
        std::cerr << "Ошибка XML: " << result.description() << '\n';
        return 1;
    }

    pugi::xml_node file = doc.child("Файл");
    std::cout << "Версия формата: " << file.attribute("ВерсФорм").value() << '\n';
}