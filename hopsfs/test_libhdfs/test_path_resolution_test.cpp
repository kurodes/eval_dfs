// g++ -O2 -std=c++11 test_pr_test.cpp -I../libhdfs/ -lhdfs -o test_pr_test
#include <sys/time.h>

#include <iostream>
#include <string>
#include <vector>

#include "hdfs.h"

using namespace std;

string GeneCreatePath(hdfsFS fs, int dep, int item) {
    string ret = {"/"};
    for (int i = 0; i < dep; ++i) {
        ret += to_string(dep) + "." + to_string(item) + "/";
        hdfsCreateDirectory(fs, ret.c_str());
    }
    return ret;
}

string GenePath(hdfsFS fs, int dep, int item) {
    string ret = {"/"};
    for (int i = 0; i < dep; ++i) {
        ret += to_string(dep) + "." + to_string(item) + "/";
    }
    return ret;
}

uint64_t NowMicro() {
    timeval tv;
    gettimeofday(&tv, NULL);
    return static_cast<uint64_t>(tv.tv_sec) * 1000 * 1000 + tv.tv_usec;
}

int main(int argc, const char **argv) {
    hdfsFS fs = hdfsConnect("default", 0);

    const int depth = 10;
    const int test_times = 100;
    vector<string> dirpaths;

    // for (int i = 0; i < test_times; ++i)
    //     dirpaths.push_back("/");
    // for (int dep = 1; dep <= depth; ++dep)
    // {
    //     for (int i = 0; i < test_times; ++i)
    //     {
    //         dirpaths.push_back(GeneCreatePath(fs, dep, i));
    //     }
    // }
    // int count = 0;
    // for (auto path : dirpaths)
    // {
    //     cout << path << endl;
    //     hdfsFile f1 = hdfsOpenFile(fs, (path + "file"+ to_string(count)).c_str(), O_WRONLY | O_CREAT, 0, 0, 0);
    //     hdfsCloseFile(fs, f1);
    //     count += 1;
    // }

    for (int i = 0; i < test_times; ++i)
        dirpaths.push_back("/");
    for (int dep = 1; dep <= depth; ++dep) {
        for (int i = 0; i < test_times; ++i) {
            dirpaths.push_back(GenePath(fs, dep, i));
        }
    }
    vector<uint64_t> Time;
    Time.push_back(NowMicro());
    int count = 0;
    for (auto path : dirpaths) {
        // cout << path << endl;
        auto ret = hdfsGetPathInfo(fs, (path + "file" + to_string(count)).c_str());
        if (NULL == ret) {
            cout << "error" << endl;
            return -1;
        }
        count += 1;
        if (count % test_times == 0) {
            Time.push_back(NowMicro());
            // cout << "time" << endl;
        }
    }
    for (int i = 0; i < depth + 1; ++i) {
        cout << (Time[i + 1] - Time[i]) / test_times << endl;
    }

    hdfsDisconnect(fs);
    return 0;
}