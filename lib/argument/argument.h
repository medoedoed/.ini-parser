#pragma once

#include <any>
#include <string>
#include <vector>
#include <variant>


enum Types {
    integer,
    decimal,
    string,
    flag,
    array
};


class Argument;

struct Node {
    std::string section_name_;
    std::vector<Argument> arguments_;
    std::vector<Node*> sons_;
};

class Argument {
public:
    void SetSection(Node* node);

    explicit Argument(std::string_view key_name);
    Argument(std::string_view key_name, Types type);
    Argument(std::string_view key_name, std::variant<int, double, std::string, bool, std::vector<Argument>> value);
    Argument(std::string_view key_name, std::variant<int, double, std::string, bool, std::vector<Argument>> value, Types type);
    [[nodiscard]] const std::string& GetName() const;

    [[nodiscard]] bool IsInt() const;
    [[nodiscard]] bool IsFloat() const;
    [[nodiscard]] bool IsString() const;
    [[nodiscard]] bool IsArray() const;
    [[nodiscard]] bool IsBool() const;

    [[nodiscard]] int AsInt() const;
    [[nodiscard]] double AsFloat() const;
    [[nodiscard]] const std::string& AsString() const;
    [[nodiscard]] bool AsBool() const;

    [[nodiscard]] int AsIntOrDefault(int n) const;
    [[nodiscard]] double AsFloatOrDefault(double n) const;
    [[nodiscard]] std::string AsStringOrDefault(const std::string& str) const;



    const Argument& operator[](int index) const;

    const Argument& Get(std::string_view key_address);

private:
    std::string name_;
    std::variant<int, double, std::string, bool, std::vector<Argument>> value_;
    Types type_;

    Node* cur_section_;

};

