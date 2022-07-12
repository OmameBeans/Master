#include <iostream>
#include <string>
#include <fstream>
#include <chrono>
#include <random>
#include "RePair_Random.hpp"
#include "../RLSLP/RLSLP.h"
#include "../RLSLP/cmdline.h"

using namespace std;

int main(int argc, char *argv[]) {
    cmdline::parser p; // parser を定義
    p.add<string>("input_file",  'i', "input file name",  true);
    p.add<string>("output_file", 'o', "output file name", true);

    p.parse_check(argc, argv);
    const string in_filename = p.get<string>("input_file");
    const string out_filename = p.get<string>("output_file") + "_result.txt";

    std::ifstream in_stream(in_filename);
    std::ofstream out_stream(out_filename);

    // cout << "in_filename = " << in_filename << endl;
    // cout << "out_filename = " << out_filename << endl;
    // 入力ファイルのバイト数を計算
    const uint64_t bytesize = in_stream.seekg(0, std::ios::end).tellg();
    in_stream.clear(); // フラグを元に戻す
    in_stream.seekg(0, std::ios::beg); // 読み込み位置を先頭に戻す


    {
        cin.rdbuf(in_stream.rdbuf());

        cin.rdbuf(in_stream.rdbuf());
        //cout.rdbuf(out_stream.rdbuf());

        string S = "";
        for (uint64_t i = 0; i < bytesize; ++i) {
            char c;
            in_stream.read((char*) & c, sizeof(c)); // 文字を読み込む
            S += c; // 文字を書き込む
        }

        auto t1 = std::chrono::high_resolution_clock::now(); // 現在時刻を取得

        RLSLP RL(S);
        RL.StoRLSLP();
        RL.cal_len();

        auto t2 = std::chrono::high_resolution_clock::now(); // 現在時刻を取得
        double millisec = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count(); // 実行時間のミリ秒を計算
        out_stream << "導出木の高さ : " << RL.get_hight() << " " << "文法サイズ : " << RL.get_num_var() << endl;
        cout << "導出木の高さ : " << RL.get_hight() << " " << "文法サイズ : " << RL.get_num_var() << endl;
        out_stream << "RLSLPでの構築時間 : " << millisec << " [ms]" << std::endl;
        cout << "RLSLPでの構築時間 : " << millisec << " [ms]" << std::endl;


        t1 = std::chrono::high_resolution_clock::now(); 
        RePair SLP(S);
        SLP.Comp();
        t2 = std::chrono::high_resolution_clock::now(); // 現在時刻を取得
        millisec = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count(); // 実行時間のミリ秒を計算
        out_stream << "導出木の高さ : " << SLP.get_hight() << " " << "文法サイズ : " << SLP.get_num_n() << endl;
        cout << "導出木の高さ : " << SLP.get_hight() << " " << "文法サイズ : " << SLP.get_num_n() << endl;
        out_stream << "RLSLPでの構築時間 : " << millisec << " [ms]" << std::endl;
        cout << "RLSLPでの構築時間 : " << millisec << " [ms]" << std::endl;
    }

    return 1;
}