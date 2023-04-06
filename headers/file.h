#include <filesystem>
#include <fstream>
#include <iostream>

using namespace std;
namespace fs = filesystem;

namespace file {
static fs::path extract_path(int argc, char **argv) {
    string path;

    if (argc < 2) {
        cout << "Enter filename:" << endl;
        cin >> path;
    } else {
        path = argv[1];
    }

    return path;
}

string get_source(int argc, char **argv) {
    fs::path path = extract_path(argc, argv);
    if (!fs::exists(path) || fs::is_directory(path)) {
        cout << "File '" << path << "' does not exist." << endl;
        exit(1);
    }

    string contents = string();
    contents.reserve(fs::file_size(path));

    ifstream file = ifstream(path);
    string line = "";
    while (getline(file, line))
        contents += line;

    file.close();

    return contents;
}
} // namespace file
