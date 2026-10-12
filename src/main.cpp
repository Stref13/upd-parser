#include <iostream>
#include <pugixml.hpp>
#include <string>
#include "upd_model.hpp"
#include "upd_pars.hpp"

int main() {
    const char* path = "samples/upd_example_utf8.xml";
    Upd upd = parse_upd(path);

    std::cout<<upd.date<<std::endl;

}