#include <iostream>
#include <dirent.h>
#include <vector>
#include <fstream>
using namespace std;

bool isHtmlFile(string file_name) {
    vector<string> tokens;
    string word = "";
    for (auto ch : file_name) {
        if (ch == '.') {
            tokens.push_back(word);
            word = "";
            continue;
        }
        word += ch;
    }
    tokens.push_back(word);

    if (tokens.size() == 0)
        return false;
    if (tokens[tokens.size()-1] == "html") {
        return true;
    }
    return false;
}

int main() {
    string temp_name;
    ofstream fileptr("files.txt");
    struct dirent *d;
    DIR *dr;
    dr = opendir(".");
    if (dr != NULL) {
        for (d = readdir(dr); d != NULL; d = readdir(dr)) {
            temp_name = d->d_name;
            if (isHtmlFile(temp_name))
                fileptr << temp_name << endl;
        }
        closedir(dr);
    }
    else
        fileptr << "-1";
    fileptr.close();

    return 0;
}
