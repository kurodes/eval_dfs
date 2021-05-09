// g++ -std=c++11 -O2 test_subtree.cpp -I../libhdfs/ -lhdfs -o test_subtree

#include <fcntl.h>

#include <algorithm>
#include <chrono>
#include <cstdio>
#include <iostream>
#include <random>
#include <string>
#include <vector>

#include "hdfs.h"

using std::cout;
using std::endl;
using std::string;

size_t times = 10000;
hdfsFS fs;

void fs_chown_dir(const string &path, uid_t uid, gid_t gid) {
    int ret = hdfsChown(fs, path.c_str(), "owner1", "group1");
    if (ret) {
        cout << std::to_string(__LINE__) << " : " << ret;
        return;
    }
}
void fs_chown_file(const string &path, uid_t uid, gid_t gid) {
    int ret = hdfsChown(fs, path.c_str(), "owner1", "group1");
    if (ret) {
        cout << std::to_string(__LINE__) << " : " << ret;
        return;
    }
}
void fs_chmod_dir(const string &path, int32_t mode) {
    int ret = hdfsChmod(fs, path.c_str(), mode);
    if (ret) {
        cout << std::to_string(__LINE__) << " : " << ret;
        return;
    }
}
void fs_chmod_file(const string &path, int32_t mode) {
    int ret = hdfsChmod(fs, path.c_str(), mode);
    if (ret) {
        cout << std::to_string(__LINE__) << " : " << ret;
        return;
    }
}
void fs_rename_dir(const string &old_path, const string &new_path) {
    int ret = hdfsRename(fs, old_path.c_str(), new_path.c_str());
    if (ret) {
        cout << std::to_string(__LINE__) << " : " << ret;
        return;
    }
}
void fs_rename_file(const string &old_path, const string &new_path) {
    int ret = hdfsRename(fs, old_path.c_str(), new_path.c_str());
    if (ret) {
        cout << std::to_string(__LINE__) << " : " << ret;
        return;
    }
}
void Fs_Mkdir(const std::string path) {
    // printf("mkdir: %s\n", path.c_str());
    // fflush(stdout);
    int ret = hdfsCreateDirectory(fs, path.c_str());
    if (ret) {
        cout << std::to_string(__LINE__) << " : " << ret;
        return;
    }
}
void Fs_Create(const std::string path) {
    // printf("create: %s\n", path.c_str());
    // fflush(stdout);
    hdfsFile f = hdfsOpenFile(fs, path.c_str(), O_WRONLY | O_CREAT, 0, 0, 0);
    if (!f) {
        cout << std::to_string(__LINE__);
        return;
    }
}

int main() {
    fs = hdfsConnect("default", 0);

    Fs_Create("/file");
    Fs_Mkdir("/dir");
    {
        printf("Chown File\t");
        fflush(stdout);
        auto st = std::chrono::high_resolution_clock::now();
        for (size_t it = 0; it < times; it++) {
            fs_chown_file("/file", 2, 4);
        }
        auto ed = std::chrono::high_resolution_clock::now();
        long long latency = std::chrono::duration_cast<std::chrono::microseconds>(ed - st).count();
        double ave_latency = (double)latency / times;
        printf("%.6f\n", ave_latency);
        fflush(stdout);
    }
    {
        printf("Chown Dir\t");
        fflush(stdout);
        auto st = std::chrono::high_resolution_clock::now();
        for (size_t it = 0; it < times; it++) {
            fs_chown_dir("/dir", 2, 4);
        }
        auto ed = std::chrono::high_resolution_clock::now();
        long long latency = std::chrono::duration_cast<std::chrono::microseconds>(ed - st).count();
        double ave_latency = (double)latency / times;
        printf("%.6f\n", ave_latency);
        fflush(stdout);
    }
    {
        printf("Chmod File\t");
        fflush(stdout);
        auto st = std::chrono::high_resolution_clock::now();
        for (size_t it = 0; it < times; it++) {
            fs_chmod_file("/file", 0755);
        }
        auto ed = std::chrono::high_resolution_clock::now();
        long long latency = std::chrono::duration_cast<std::chrono::microseconds>(ed - st).count();
        double ave_latency = (double)latency / times;
        printf("%.6f\n", ave_latency);
        fflush(stdout);
    }
    {
        printf("Chmod Dir\t");
        fflush(stdout);
        auto st = std::chrono::high_resolution_clock::now();
        for (size_t it = 0; it < times; it++) {
            fs_chmod_dir("/dir", 0755);
        }
        auto ed = std::chrono::high_resolution_clock::now();
        long long latency = std::chrono::duration_cast<std::chrono::microseconds>(ed - st).count();
        double ave_latency = (double)latency / times;
        printf("%.6f\n", ave_latency);
        fflush(stdout);
    }
    {
        printf("Rename File\t");
        fflush(stdout);
        auto st = std::chrono::high_resolution_clock::now();
        for (size_t it = 0; it < times / 2; it++) {
            fs_rename_file("/file", "/file_renamed");
            fs_rename_file("/file_renamed", "/file");
        }
        auto ed = std::chrono::high_resolution_clock::now();
        long long latency = std::chrono::duration_cast<std::chrono::microseconds>(ed - st).count();
        double ave_latency = (double)latency / times;
        printf("%.6f\n", ave_latency);
        fflush(stdout);
    }
    {
        printf("Rename Dir\t");
        fflush(stdout);
        auto st = std::chrono::high_resolution_clock::now();
        for (size_t it = 0; it < times / 2; it++) {
            fs_rename_dir("/dir", "/dir_renamed");
            fs_rename_dir("/dir_renamed", "/dir");
        }
        auto ed = std::chrono::high_resolution_clock::now();
        long long latency = std::chrono::duration_cast<std::chrono::microseconds>(ed - st).count();
        double ave_latency = (double)latency / times;
        printf("%.6f\n", ave_latency);
        fflush(stdout);
    }

    int ret = hdfsDisconnect(fs);
    if (ret) cout << __LINE__ << " : " << ret;

    return 0;
}
