#pragma once

#include "argument/argument.h"
#include <filesystem>
#include <istream>
#include <any>
#include <fstream>
#include <iostream>
#include <charconv>

namespace omfl {

class Parser {
private:
    void AddSection(std::string_view section_name);
    void SearchSection(std::string_view section_name);

    Node* root_;
    Node* cur_node_;

    bool valid_;
public:

    Parser();

    const Argument& Get(std::string_view key_name);

    bool valid();
    void invalid();

    Argument ValueParse(std::string_view value_str_view, std::string_view key_name_view = "");

    void SectionParse(std::string_view section);
    void KeyParse(const std::string& key_and_value);
    void AppendArgument(Argument& argument);
};

    Parser Parse(std::istream& stream);
    Parser parse(const std::filesystem::path& path);
    Parser parse(const std::string& str);
}