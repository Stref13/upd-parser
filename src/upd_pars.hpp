#pragma once

#include <stdexcept>
#include <string>

#include "upd_model.hpp"

class ParseError : public std::runtime_error
{
public: 
    using std::runtime_error::runtime_error;
};

Upd parse_upd(const char* path); // Разбирает УПД из строки с xml в кодировке UTF-8