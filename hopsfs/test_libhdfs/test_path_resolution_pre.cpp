// g++ -O2 -std=c++11 test_pr_pre.cpp -I../libhdfs/ -lhdfs -o test_pr_pre
#include <iostream>
#include <string>
#include <vector>

#include "hdfs.h"

using namespace std;

string GenePath(hdfsFS fs, int dep, int item) {
    string ret = {"/"};
    for (int i = 0; i < dep; ++i) {
        ret += to_string(dep) + "." + to_string(item) + "/";
        hdfsCreateDirectory(fs, ret.c_str());
    }
    return ret;
}

int main() {
    hdfsFS fs = hdfsConnect("default", 0);

    const int depth = 10;
    const int test_times = 10;
    vector<string> dirpaths;

    for (int i = 0; i < test_times; ++i)
        dirpaths.push_back("/");
    for (int dep = 1; dep <= depth; ++dep) {
        for (int i = 0; i < test_times; ++i) {
            dirpaths.push_back(GenePath(fs, dep, i));
        }
    }

    for (auto ch : dirpaths) {
        cout << ch << endl;
        hdfsFile f1 = hdfsOpenFile(fs, (ch + "file").c_str(), O_WRONLY | O_CREAT, 0, 0, 0);
        hdfsCloseFile(fs, f1);
    }

    // int ret;
    // stat = hdfsGetPathInfo(o->fs, path);
    // if (stat == NULL)
    // {
    //     return 1;
    // }

    hdfsDisconnect(fs);
    return 0;
}