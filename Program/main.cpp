#include<iostream>
#include<string>
#include<unordered_map>
#include<vector>
#include <cstdio>
#include <string>
#include <fstream>
#include <chrono>

#include "cmdline.h"
#include "RLSLP.h"


using namespace std;
using uint = unsigned int;
using ull = unsigned long long;


int main(int argc, char *argv[]) {
    string S;
    cmdline::parser p; // parser を定義
    p.add<string>("input_file",  'i', "input file name",  true);

    p.parse_check(argc, argv);
    const string in_filename = p.get<string>("input_file");

    auto t1 = std::chrono::high_resolution_clock::now(); // 現在時刻を取得
    std::ifstream in_stream(in_filename);

    cout << "in_filename = " << in_filename << endl;
    // 入力ファイルのバイト数を計算
    const uint64_t bytesize = in_stream.seekg(0, std::ios::end).tellg();
    in_stream.clear(); // フラグを元に戻す
    in_stream.seekg(0, std::ios::beg); // 読み込み位置を先頭に戻す
    cout << "bytesize = " << bytesize << endl;

    {
        for (uint64_t i = 0; i < bytesize; ++i) {
            char c;
            in_stream.read((char*) & c, sizeof(c)); // 文字を読み込む
            S += c;
        }
    }

    RLSLP RL(S);
    RL.print_str();
    RL.StoRLSLP();
    RL.print_rules();

    auto t2 = std::chrono::high_resolution_clock::now(); // 現在時刻を取得
    double millisec = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count(); // 実行時間のミリ秒を計算
    std::cout << millisec << " ms" << std::endl;

    return 1;
}
