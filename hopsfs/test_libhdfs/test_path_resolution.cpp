// g++ -std=c++11 test_path_resolution.cpp -I${HADOOP_HOME}/hdfs-src/libhdfs/ -lhdfs -o test_path_resolution

#include <fcntl.h>
#include <stdio.h>

#include <algorithm>
#include <chrono>
#include <iostream>
#include <random>
#include <string>
#include <vector>

// #include <glog/logging.h>
// #include <gflags/gflags.h>

#include "hdfs.h"

using std::cout;
using std::endl;

// DEFINE_string(config, "./conf/config.json", "the cluster config.conf path");

const size_t times = 2000;

hdfsFS fs;

void Fs_Rename(const std::string old_p, const std::string new_p) {
    // printf("rename: %s, %s\n", old_p.c_str(), new_p.c_str());
    // fflush(stdout);
    int ret = hdfsRename(fs, old_p.c_str(), new_p.c_str());
    if (ret) cout << __LINE__ << " : " << ret;
}

void Fs_Mkdir(const std::string path) {
    // printf("mkdir: %s\n", path.c_str());
    // fflush(stdout);
    int ret = hdfsCreateDirectory(fs, path.c_str());
    if (ret) cout << __LINE__ << " : " << ret;
}

void Fs_Create(const std::string path) {
    // printf("create: %s\n", path.c_str());
    // fflush(stdout);
    hdfsFile f = hdfsOpenFile(fs, path.c_str(), O_WRONLY | O_CREAT, 0, 0, 0);
    if (!f) cout << __LINE__;
}

void Fs_Open(const std::string path) {
    hdfsFile f = hdfsOpenFile(fs, path.c_str(), O_WRONLY, 0, 0, 0);
    if (!f) cout << __LINE__;
}

std::string GenePath(const size_t st, const size_t ed) {
    std::string path = "";
    for (size_t i = st; i <= ed; ++i) {
        path = path + std::string("/") + std::to_string(i);
    }
    return path;
}

void TestAveLatency(const size_t rename_times, const std::string path) {
    auto st = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < times; i++) {
        Fs_Create(path + std::to_string(i));
    }
    auto ed = std::chrono::high_resolution_clock::now();
    // auto ave_latency = (t_start - t_end) / times;
    long long latency = std::chrono::duration_cast<std::chrono::microseconds>(ed - st).count();
    double ave_latency = (double)latency / times;
    printf("%lu\t%.6f\n", rename_times, ave_latency);
    fflush(stdout);
    return;
}
void MakeConPath(const std::string &P, const size_t &st, const size_t &ed) {
    std::string dir = P;
    for (size_t i = st; i <= ed; ++i) {
        dir = dir + std::string("/") + std::to_string(i);
        Fs_Mkdir(dir);
    }
    return;
}

std::string strRand(const int len) {
    std::string tmp_s;
    static const char alphanum[] =
        "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";
    tmp_s.reserve(len);
    for (int i = 0; i < len; ++i)
        tmp_s += alphanum[rand() % (sizeof(alphanum) - 1)];
    return tmp_s;
}

int main(int argc, const char **argv) {
    // google::InitGoogleLogging(argv[0]);
    // google::ParseCommandLineFlags(&argc, &argv, true);
    // DLOG(INFO) << argc << " " << argv[0];
    // FLAGS_logtostderr = 1;
    // FLAGS_colorlogtostderr = 1;
    // FLAGS_minloglevel = 0;

    // init fs
    ////////////////////////////////////////////
    fs = hdfsConnect("default", 0);

    // section A: test path resolution wo rename
    ////////////////////////////////////////////
    {
        std::vector<size_t> depths = {1, 2, 4, 8, 12, 16, 20, 24};
        std::vector<std::string> paths;
        //make tree
        std::string dir = "";
        for (size_t i = 1; i <= depths.back(); ++i) {
            dir = dir + std::string("/A") + std::to_string(i);
            Fs_Mkdir(dir);
            if (std::find(depths.begin(), depths.end(), i) != depths.end()) {
                Fs_Create(dir + std::string("/f"));
                paths.push_back(dir + std::string("/f"));
            }
        }
        // test latency
        printf("depth\ttime(us)\n");
        fflush(stdout);
        for (size_t it = 0; it < depths.size(); ++it) {
            auto st = std::chrono::high_resolution_clock::now();
            for (size_t i = 0; i < times; i++) {
                Fs_Create(paths[it] + std::to_string(i));
            }
            auto ed = std::chrono::high_resolution_clock::now();
            // auto ave_latency = (t_start - t_end) / times;
            long long latency = std::chrono::duration_cast<std::chrono::microseconds>(ed - st).count();
            double ave_latency = (double)latency / times;
            printf("%lu\t%.6f\n", depths[it], ave_latency);
            fflush(stdout);
        }
    }

    // section B: test rename impact
    ////////////////////////////////////////////
    {
        printf("Rename\ttime(us)\n");
        fflush(stdout);
        // FIXME: r_len*(R_num + 1) <= depth
        size_t depth = 24;  //0-23
        size_t R_len = 2;
        size_t R_num = 11;
        for (size_t R = 0; R <= R_num; ++R) {
            std::string RIT(std::string("/R") + std::to_string(R));
            std::string file_path = RIT + GenePath(1, depth - 1) + std::string("/f");
            Fs_Mkdir(RIT);

            std::vector<size_t> mv_p;
            for (size_t p = 1; p <= R; ++p) {
                mv_p.push_back(depth - p * R_len);
            }
            std::reverse(mv_p.begin(), mv_p.end());
            mv_p.push_back(depth);
            if (R != 0) {
                MakeConPath(RIT, 1, mv_p[0] - 1);
                for (size_t p = 0; p < R; p++) {
                    MakeConPath(RIT, mv_p[p], mv_p[p + 1] - 1);
                }
                for (size_t p = 0; p < R; p++) {
                    Fs_Rename(RIT + std::string("/") + std::to_string(mv_p[p]), RIT + GenePath(1, mv_p[p]));
                }
            } else {
                MakeConPath(RIT, 1, depth - 1);
            }
            Fs_Create(file_path);
            TestAveLatency(R, file_path);
        }
    }

    // delete fs
    ////////////////////////////////////////////
    int ret = hdfsDisconnect(fs);
    if (ret) cout << __LINE__ << " : " << ret;

    // google::ShutdownGoogleLogging();
    return 0;
}
