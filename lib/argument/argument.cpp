#include "argument.h"

#include <utility>


Argument::Argument(std::string_view key_name)
        : name_(key_name)
{

}

Argument::Argument(std::string_view key_name, std::variant<int, double, std::string, bool, std::vector<Argument>> value)
        : name_(key_name)
        , value_(std::move(value))
{

}

Argument::Argument(std::string_view key_name, Types type)
        : name_(key_name)
        , type_(type)
{

}

Argument::Argument(std::string_view key_name, std::variant<int, double, std::string, bool, std::vector<Argument>> value,
                   Types type)
        : name_(key_name)
        , value_(std::move(value))
        , type_(type)
{

}

const Argument undefined_argument_("name", 'c');

const std::string& Argument::GetName() const{
    return name_;
}

bool Argument::IsInt() const {
    return type_ == integer;
}

int Argument::AsInt() const {
    return std::get<integer>(value_);
}

int Argument::AsIntOrDefault(int n) const {
    if (type_ == integer) {
        return std::get<integer>(value_);
    } else {
        return n;
    }
}

bool Argument::IsFloat() const {
    return type_ == decimal;
}

double Argument::AsFloat() const {
    return std::get<decimal>(value_);
}

double Argument::AsFloatOrDefault(double n) const {
    if (type_ == decimal) {
        return std::get<decimal>(value_);
    } else {
        return n;
    }
}

bool Argument::IsString() const {
    return type_ == string;
}

const std::string& Argument::AsString() const {
    return std::get<string>(value_);
}

std::string Argument::AsStringOrDefault(const std::string& str) const {
    if (type_ == string) {
        return std::get<string>(value_);
    } else {
        return str;
    }
}

bool Argument::IsArray() const {
    return type_ == array;
}

const Argument& Argument::operator[](int index) const {
    auto& array = std::get<std::vector<Argument>>(value_);
    if (index > array.size() - 1) {
        return undefined_argument_;
    }

    return array[index];
}

bool Argument::IsBool() const {
    return type_ == flag;
}

bool Argument::AsBool() const {
    return std::get<flag>(value_);
}



