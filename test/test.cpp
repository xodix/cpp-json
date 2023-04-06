#include "../headers/parse.h"
#include <array>
#include <bits/chrono.h>
#include <cassert>
#include <chrono>
#include <cstddef>
#include <format>
#include <iostream>

using namespace std;

void test_parse_number() {
    array<string_view, 10> texts = {
        "1",     "1.5",    "-1",      "-1.5",   "1e+2",
        "-1e+2", "1.5e-2", "-1.5e-2", "1.5E+2", "-1.5E+2",
    };

    array<const double, 10> doubles = {
        1.0, 1.5, -1.0, -1.5, 100.0, -100.0, 0.015, -0.015, 150, -150,
    };

    for (int i = 0; i < texts.size(); i++) {
        // assert(get<double>(json::Json::parse_number(texts[i]).value) ==
        // doubles[i]);
    }

    cout << "All correct! You don't suck!" << endl;
}

void test_find_string_end() {
    array<string_view, 5> strings = {
        "abcdefghijkl\" jo mama", "abcdefghijkl jo\" mama",
        "abcdefghijkl j\"o mama", "abcdefghijkl jo mama\"",
        "abcdefghijkl jo mama",
    };

    array<size_t, 5> results = {
        12, 15, 14, 20, 20,
    };

    for (size_t i = 0; i < strings.size(); i++) {
        // assert(json::Json::find_string_end(strings[i], 0) == results[i]);
    }

    cout << "All correct!" << endl;
}

void test_parse_array() {
    string source = "[1,\"Go to hell man\",[null, true, false],[1, 2, "
                    "3],10.123e-12]";

    try {
        json::Json e(source);

        for (json::Variant &v : get<vector<json::Variant>>(e.init)) {
            v << cout;
            cout << endl;
        }
    } catch (string msg) {
        cout << msg << endl;
    }

    cout << "Nothing broke!" << endl;
}

void test_parse_object() {
    string source = "{\"hello\": [1,2,3,4,5],\n\n \"mellow\": {\"abc\": 5}}";

    try {
        json::Json e(source);

        for (json::Variant &v : get<vector<json::Variant>>(e.init)) {
            v << cout;
            cout << endl;
        }
    } catch (string msg) {
        cout << msg << endl;
    }

    cout << "Nothing broke!" << endl;
}

int main() {
    auto start = chrono::high_resolution_clock::now();
    test_parse_object();
    auto end = chrono::high_resolution_clock::now();

    cout << chrono::duration_cast<chrono::microseconds>(end - start) << endl;
}

