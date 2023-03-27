#include <cmath>
#include <cstdint>
#include <cstdio>
#include <format>
#include <iostream>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

namespace json {
using namespace std;

enum class Type {
    String,
    Number,
    Boolean,
    Object,
    Array,
    // Is counted as false
    Null
};

class Variant {
  public:
    Type type;
    std::variant<string, unordered_map<string, Variant>, vector<Variant>,
                 double, bool>
        value;

    Variant() {}
    Variant(Type json_type) { type = json_type; }
    Variant(Type json_type, std::variant<string, unordered_map<string, Variant>,
                                         vector<Variant>, double, bool>
                                json_value) {
        type = json_type;
        value = json_value;
    }

    friend void operator<<(Variant &variant, ostream &stream) {
        switch (variant.type) {
        case Type::Null:
            stream << "boolean";
            break;
        case Type::Number:
            stream << "number";
            break;
        case Type::Boolean:
            stream << "boolean";
            break;
        case Type::Object:
            stream << "[object Object]";
            break;
        case Type::Array:
            stream << "Array";
            break;
        case Type::String:
            stream << "string";
            break;
        }
        stream << "( ";

        string value_as_string = "";
        switch (variant.type) {
        case Type::Null:
            stream << "null";
            break;
        case Type::Number:
            stream << to_string(get<double>(variant.value));
            break;
        case Type::Boolean:
            stream << (get<bool>(variant.value) ? "true" : "false");
            break;
        case Type::Object:
            stream << "{}";
            break;
        case Type::Array:
            for (Variant &v : get<vector<Variant>>(variant.value)) {
                v << stream;
            }
            break;
        case Type::String:
            stream << '"' << get<string>(variant.value) << '"';
            break;
        }

        stream << " ); ";
    }
};

class Json {
  public:
    Type type;

    variant<unordered_map<string, Variant>, vector<Variant>> init;

    inline bool is_whitespace(const char ch) {
        return ch == ' ' || ch == '\n' || ch == '\r' || ch == '\t';
    }

    inline size_t find_whitespace_or_comma(string_view source,
                                           size_t start_index) {
        for (size_t i = start_index; i < source.size(); i++) {
            if (is_whitespace(source[i]) || source[i] == ',') {
                return i;
            }
        }

        return source.size();
    }

    // Find end of string ommiting escaped ones. "\"Let there be light\"" -> "Let there be right"
    inline size_t find_string_end(string_view& str, size_t start_index) {
        char ch = ' ';

        for (size_t i = start_index + 1; i < str.size(); i++) {
            ch = str[i];
            if (ch == '"' && str[i - 1] != '\\') {
                return i;
            }
        }

        return str.size();
    }

    // Find corresponding `]` being aware of other arrays.
    inline size_t find_array_outer_scope(string_view& source, size_t start_index) {
        char ch = ' ';
        uint32_t scope = 1;

        for (size_t i = 0; i < source.size(); i++) {
            ch = source[i];
            
            if (ch == ']')
                scope--;
            else if (ch == '[')
                scope++;
            if (scope == 0)
                return i;
        }

        return source.size();
    }

    // Extract JSON number in /-?\d+(\.\d+)?((e|E)(-|\+)\d+)?/gi format into
    // 64-bit float.
    Variant parse_number(string_view number) {
        size_t e_index = number.find('e');
        if (e_index >= number.size()) {
            e_index = number.find('E');
        }

        string_view up_to_e = number.substr(0, min(e_index, number.size()));
        double parsed_number = stod(string(up_to_e).c_str());

        if (e_index < number.size()) {
            string_view after_e = number.substr(e_index + 1, number.size());
            int exponent = stoi(string(after_e).c_str());

            // e-expnent
            if (exponent < 0)
                parsed_number /= pow(10, -exponent);
            // e+exponent
            else
                parsed_number *= pow(10, exponent);
        }

        return Variant(Type::Number, parsed_number);
    }

    // /true|false|null/gi
    Variant parse_keyword(string_view keyword) {
        if (keyword == "true") {
            return Variant(Type::Boolean, true);
        } else if (keyword == "false") {
            return Variant(Type::Boolean, false);
        } else if (keyword == "null") {
            return Variant(Type::Null);
        } else {
            throw format("Invalid keyword: `{}`.", keyword);
        }
    }

    Variant parse_string(string_view str) {
        Variant variant(Type::String);
        variant.value.emplace<string>(str);

        return variant;
    }

    Variant parse_array(string_view source) {
        vector<Variant> contents = vector<Variant>();
        size_t i = 0;

        // Loop throught array source character by character.
        while (i < source.size()) {
            const char ch = source[i];

            size_t new_index = 0;
            if (is_whitespace(ch) || ch == ',') {
                i++;

                continue;
            } else if ((ch >= '0' && ch <= '9') || ch == '-') {
                new_index = find_whitespace_or_comma(source, i);

                contents.push_back(
                    parse_number(source.substr(i, new_index - i)));
            } else if (ch == '"') {
                new_index = find_string_end(source, i + 1);

                contents.push_back(
                    parse_string(source.substr(i + 1, new_index - i - 1)));
            } else if (ch == '[') {
                new_index = source.find(']', i + 1);

                contents.push_back(
                    parse_array(source.substr(i + 1, new_index - i - 1)));
            } else {
                new_index = find_whitespace_or_comma(source, i);

                contents.push_back(
                    parse_keyword(source.substr(i, new_index - i)));
            }

            i = new_index + 1;
        }

        return Variant(Type::Array, contents);
    }

    Variant parse_object(string_view source) {
        unordered_map<string, Variant> contents =
            unordered_map<string, Variant>();
        size_t i = 0;

        while (i < source.size()) {
        }

        return Variant(Type::Object, contents);
    }

    Json(string_view source) {
        if (source[0] == '[')
            init = get<vector<Variant>>(
                parse_array(source.substr(1, find_array_outer_scope(source, 1) - 1)).value);
        else if (source[0] == '{')
            init = get<unordered_map<string, Variant>>(parse_object(source.substr(1, source.find('}') - 1)).value);
        else
            throw string("Invalid JSON format!\n First line should be an array "
                         "or an object.");
    }
};
} // namespace json
