#include "parser.h"

namespace omfl {

    size_t NextIdx(size_t first_idx, std::string_view path) {
        if (path.find('.', first_idx) == std::string::npos) {
            return path.size();
        } else {
            return path.find('.', first_idx);
        }
    }

    void Parser::SectionParse(std::string_view section_view) {
        cur_node_ = root_;

        if (section_view.empty()) {
            invalid();
        }

        size_t first_idx = 0;
        size_t second_idx;

        do {
            second_idx = NextIdx(first_idx, section_view);

            if (second_idx != first_idx) {
                AddSection(section_view.substr(first_idx, second_idx - first_idx));
            } else {
                invalid();
            }

            first_idx = second_idx + 1;

        } while (second_idx != section_view.size());
    }

    void Parser::AddSection(std::string_view section_name) {
        for (auto & sec : cur_node_->sons_) {
            if (sec->section_name_ == section_name) {
                cur_node_ = sec;
                return;
            }
        }

        Node* node = new Node();
        node->section_name_ = section_name;
        cur_node_->sons_.emplace_back(node);
        cur_node_ = node;
    }


    std::string_view DeleteSpaces(std::string_view str) {
        while (isspace(str.front())) {
            str.remove_prefix(1);
        }

        while (isspace(str.back())) {
            str.remove_suffix(1);
        }

        return str;
    }

    bool IsNumber(std::string_view str) {
        size_t number_of_dots = 0;
        uint8_t offset = 0;
        if (str[0] == '+' || str[0] == '-') {
            offset = 1;

            if (str.size() == 1) {
                return false;
            }
        }

        if (str[0 + offset] == '.' || str[str.size() - 1] == '.') {
            return false;
        } else {
            for (size_t i = 1; i < str.size(); i++) {
                if (!(isdigit(str[i]) || str[i] == '.')) {
                    return false;
                }
                if (str[i] == '.') {
                    number_of_dots++;
                }
            }

            if (number_of_dots > 1) {
                return false;
            }
        }

        return true;
    }

    Argument Parser::ValueParse(std::string_view value_str_view, std::string_view key_name_view) {
        if (value_str_view[0] == '[' && value_str_view[value_str_view.size() - 1] == ']') {
            std::vector<Argument> value;
            size_t element_start_idx = 1;
            size_t element_end_idx;

            bool in_string = false;
            bool in_array = false;

            for (size_t i = 1; i < value_str_view.size(); i++) {
                if (value_str_view[i] == '[' && !in_string) {
                    in_array = true;
                }

                if (in_array) {
                    while (value_str_view[i] != ']') {
                        i++;
                    }

                    in_array = false;
                }

                if (value_str_view[i] == '"' && !in_string) {
                    in_string = true;
                } else if (value_str_view[i] == '"' && in_string) {
                    in_string = false;
                }

                if ((value_str_view[i] == ',' || i == value_str_view.size() - 1) && !in_string) {
                    element_end_idx = i;
                    std::string_view sub_value = DeleteSpaces(
                            value_str_view.substr(element_start_idx, element_end_idx - element_start_idx));
                    if (!sub_value.empty()) {
                        value.emplace_back(ValueParse(sub_value));
                        element_start_idx = element_end_idx + 1;
                    }
                }
            }

            Argument argument(key_name_view, value, array);
            return argument;

        } else if (value_str_view[0] == '"' && value_str_view[value_str_view.size() - 1] == '"' &&
                   value_str_view.size() > 1) {


            if (value_str_view.find('"', 1) != value_str_view.size() - 1) {
                invalid();
            }

            std::string value(value_str_view.substr(1, value_str_view.size() - 2));
            Argument argument(key_name_view, value, string);

            return argument;
        } else if (value_str_view[0] == 't' || value_str_view[0] == 'f') {
            if (value_str_view == "true") {
                Argument argument(key_name_view, true, flag);
                return argument;
            } else if (value_str_view == "false") {
                Argument argument(key_name_view, false, flag);
                return argument;
            } else {
                invalid();
            }
        } else if ((value_str_view[0] == '+' || value_str_view[0] == '-' || isdigit(value_str_view[0])) &&
                   IsNumber(value_str_view)) {
            bool is_double = false;

            for (char i: value_str_view) {
                if (i == '.') {
                    is_double = true;
                }
            }

            if (is_double) {
                std::string str(value_str_view);
                Argument argument(key_name_view, std::stod(str), decimal);
                return argument;
            } else {
                std::string str(value_str_view);
                Argument argument(key_name_view, std::stoi(str), integer);
                return argument;
            }
        } else {
            invalid();
        }

        Argument argument(key_name_view);
        return argument;
    }

    bool KeyValidity(std::string_view name) {
        if (name.empty()) {
            return false;
        }

        for (char i: name) {
            if (!(isalnum(i) || i == '-' || i == '_')) {
                return false;
            }
        }

        return true;
    }

    void Parser::KeyParse(const std::string& key_and_value) {
        std::string_view key_and_value_view = key_and_value;
        size_t equality_idx = key_and_value_view.find('=', 0);
        size_t value_end_idx = key_and_value_view.size();

        if (equality_idx == value_end_idx) {
            invalid();
        } else {
            std::string_view key_name = DeleteSpaces(key_and_value.substr(0, equality_idx - 1));

            if (!KeyValidity(key_name)) {
                invalid();
            }
            bool in_string = false;

            for (size_t i = equality_idx + 1; i < key_and_value_view.size(); i++) {
                if (key_and_value_view[i] == '"' && !in_string) {
                    in_string = true;
                } else if (key_and_value_view[i] == '"' && in_string) {
                    in_string = false;
                }

                if (key_and_value_view[i] == '#' && !in_string) {
                    value_end_idx = i - 1;
                    break;
                }
            }

            Argument argument = ValueParse(
                    DeleteSpaces(key_and_value_view.substr(equality_idx + 1, value_end_idx - equality_idx)),
                    key_name);

            AppendArgument(argument);
        }
    }

    void Parser::AppendArgument(Argument& argument) {
        for (auto& i: cur_node_->arguments_) {
            if (i.GetName() == argument.GetName()) {
                invalid();
            }
        }

        if (valid()) {
            argument.SetSection(cur_node_);
            cur_node_->arguments_.emplace_back(argument);

        }
    }

    enum Status {
        ready_to_work,
        comment_started,
        read_section,
        read_key
    };


    Parser Parse(std::istream& stream) {
        Parser parser;
        std::string sections;
        std::string key_and_value;
        std::string line;

        while (std::getline(stream, line)) {
            std::string_view line_view = line;

            Status status = ready_to_work;

            for (auto& c: line_view) {
                if (status == ready_to_work) {
                    if (c == '#') {
                        status = comment_started;
                    } else if (c == '[') {
                        sections.clear();
                        status = read_section;
                    } else if (!(isspace(c))) {
                        status = read_key;
                        key_and_value += c;
                    }
                } else if (status == comment_started) {
                    if (c == '\n') {
                        status = ready_to_work;
                    }
                } else if (status == read_section) {
                    sections += c;
                    if (c == ']') {
                        std::string_view sections_view = sections;
                        parser.SectionParse(sections_view.substr(0, sections_view.size() - 1));
                            status = ready_to_work;
                    } else if (c == '\n') {
                        parser.invalid();
                        status = ready_to_work;
                    } else if (c == '#') {
                        parser.invalid();
                        status = comment_started;
                    }
                } else {
                    key_and_value += c;
                }
            }

            if (status == read_key) {
                parser.KeyParse(key_and_value);
                key_and_value.clear();
            }
        }

        return parser;
    }


    Parser parse(const std::string& str) {
        std::istringstream stream(str);
        return Parse(stream);
    }

    Parser parse(const std::filesystem::path& path) {
        std::ifstream omfl(path);
        std::istream& stream = omfl;
        return Parse(stream);
    }

    Parser::Parser()
            : valid_(true) {
        Node* node = new Node();
        root_ = node;
        cur_node_ = node;
    }

    bool Parser::valid() {
        return valid_;
    }

    void Parser::invalid() {
        valid_ = false;
    }

    const Argument& Parser::Get(std::string_view key_address) {
        cur_node_ = root_;

        size_t first_idx = 0;
        size_t second_idx;

        do {
            second_idx = NextIdx(first_idx, key_address);

            if (second_idx != first_idx) {
                if (second_idx != key_address.size()) {
                    SearchSection(key_address.substr(first_idx, second_idx - first_idx));
                } else {
                    for (auto & arg : cur_node_->arguments_) {
                        if (arg.GetName() == key_address.substr(first_idx, second_idx - first_idx)) {
                            return arg;
                        }
                    }
                }
            } else {
                throw std::runtime_error("Wrong address");
            }

            first_idx = second_idx + 1;

        } while (second_idx != key_address.size());
    }

    void Parser::SearchSection(std::string_view section_name) {
        for (auto & sec : cur_node_->sons_) {
            if (sec->section_name_ == section_name) {
                cur_node_ = sec;
            }
        }
    }

}







