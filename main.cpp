#include "headers/file.h"
#include "headers/parse.h"
#include <filesystem>
#include <fstream>
#include <iostream>

using namespace std;

int main(int argc, char **argv) {
    string contens = file::get_source(argc, argv);
}
