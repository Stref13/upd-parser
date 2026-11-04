#include <iostream>
#include <ostream>
#include <pugixml.hpp>
#include <string>
#include <vector>
#include "money.hpp"
#include "upd_model.hpp"
#include "upd_pars.hpp"

int main() {
    const char* path = "samples/upd_example_utf8.xml";
    Upd upd = parse_upd(path);
    std::vector<Item> items;
    for(auto item : upd.items)
    {
        std::cout<<to_string(item.nds.value())<<std::endl;
    }
    std::cout<<to_string(upd.totals.nds.value())<<std::endl;
}