#include <iostream>
#include <map>
#include <string>
#include "dofile.cpp"

using namespace std;

int main() {
    DoFile<string> dofile;
    dofile.ReadFile("file.txt");
    auto data = dofile.run();

    cout << "\n=== Typed data access (no manual conversion) ===" << endl;

    if (auto it = data.find("x"); it != data.end()) {
        int x = it->second;
        cout << "  x (int) + 5 = " << (x + 5) << endl;
    }

    if (auto it = data.find("name"); it != data.end()) {
        string name = it->second;
        cout << "  name (string) = " << name << endl;
    }

    if (auto it = data.find("greeting"); it != data.end()) {
        string greeting = it->second;
        cout << "  greeting (string) = " << greeting << endl;
    }

    if (auto it = data.find("words"); it != data.end()) {
        cout << "  words (raw collection) = " << it->second.getRawValue() << endl;
    }

    if (auto it = data.find("ages"); it != data.end()) {
        cout << "  ages (raw collection) = " << it->second.getRawValue() << endl;
    }

    return 0;
}
