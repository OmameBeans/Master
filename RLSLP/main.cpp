#include<iostream>
#include<string>
#include<unordered_map>
#include<vector>
#include <cstdio>
#include <string>
#include <fstream>
#include <chrono>
#include <random>

#include "cmdline.h"
#include "RLSLP.h"


using namespace std;
using uint = unsigned int;
using ull = unsigned long long;

int lce(int i, int j, string S) {
    int ans = 0;
    for(int cnt = 0; j+cnt < S.size(); cnt++) {
        if(S[i+cnt] != S[j+cnt]) break;
        ans++;
    }
    return ans;
}


int main(int argc, char *argv[]) {
    cmdline::parser p; // parser を定義
    p.add<string>("input_file",  'i', "input file name",  true);
    // p.add<string>("output_file", 'o', "output file name", true);

    p.parse_check(argc, argv);
    const string in_filename = p.get<string>("input_file");
    // const string out_filename = p.get<string>("output_file") + ".bin";

    std::ifstream in_stream(in_filename);

    // cout << "in_filename = " << in_filename << endl;
    // cout << "out_filename = " << out_filename << endl;
    // 入力ファイルのバイト数を計算
    const uint64_t bytesize = in_stream.seekg(0, std::ios::end).tellg();
    in_stream.clear(); // フラグを元に戻す
    in_stream.seekg(0, std::ios::beg); // 読み込み位置を先頭に戻す
    // cout << "in_bytesize = " << bytesize << endl;

    // const string out_file = p.get<string>("output_file");

    // string out_file_byte = string(out_file).append(".bin");
    // {
    //     std::ofstream out_stream(out_file_byte);
    // }

    {
        // std::ofstream out_stream(out_file_byte);


        cin.rdbuf(in_stream.rdbuf());

        int N,Q; cin >> N >> Q;
        string S; cin >> S;

        cout << "文字列の長さ : " << N << endl << "クエリ数 : " << Q << endl; 

        vector<int> a(Q), b(Q), l(Q), r(Q);

        auto t1 = std::chrono::high_resolution_clock::now(); // 現在時刻を取得

        RLSLP RL(S);
        RL.StoRLSLP();

        for(int q = 0; q < Q; q++) {
            int i,j; cin >> i >> j;
            l[q] = i, r[q] = j;
            a[q] = RL.LCE(i,j);
        }

        auto t2 = std::chrono::high_resolution_clock::now(); // 現在時刻を取得
        double millisec = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count(); // 実行時間のミリ秒を計算
        std::cout << "RLSLPでのLCE : " << millisec << " ms" << std::endl;

        t1 = std::chrono::high_resolution_clock::now(); // 現在時刻を取得


        for(int q = 0; q < Q; q++) {
            b[q] = lce(l[q],r[q],S);
        }

        t2 = std::chrono::high_resolution_clock::now(); // 現在時刻を取得
        millisec = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count(); // 実行時間のミリ秒を計算
        std::cout << "愚直にLCE : " << millisec << " ms" << std::endl;

        for(int i = 0; i < Q; i++) {
            if(a[i] != b[i]) {
                cout << "Diff" << endl;
                cout << l[i] << " " << r[i] << " " << a[i] << " " << b[i] << endl;
            }
        }


        // vector<int> out = RL.Compress();
        // for(auto p : out) cout << p;
        // cout << endl;
        // for(auto b : out) {
        //     out_stream.write((char*) & b, sizeof(char));
        // }
        // const uint64_t out_size = out_stream.seekp(0, std::ios::end).tellp();
        // cout << "out_bytesize = " << out_size/8.0 << endl;
    }

    return 1;
}

