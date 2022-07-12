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

        string S = "";

        for (uint64_t i = 0; i < bytesize; ++i) {
            char c;
            in_stream.read((char*) & c, sizeof(c)); // 文字を読み込む
            if(c != '\n') S += c; // 文字を書き込む
        }

        int N = S.size();

        // out_stream << N << endl << S << endl;
        cout << in_filename << " " << "size : " << N << endl;

        long long total_lce = 0;

        auto t1 = std::chrono::high_resolution_clock::now(); // 現在時刻を取得

        RLSLP RL(S);
        RL.StoRLSLP();

        auto t2 = std::chrono::high_resolution_clock::now(); // 現在時刻を取得
        double millisec = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count(); // 実行時間のミリ秒を計算
        // out_stream << "導出木の高さ : " << RL.get_hight() << " " << "文法サイズ : " << RL.get_num_var() << endl;
        cout << "導出木の高さ : " << RL.get_hight() << " " << "文法サイズ : " << RL.get_num_var() << endl;
        // out_stream << "RLSLPの構築時間 : " << millisec << " [ms]" << std::endl;
        cout << "RLSLPの構築時間 : " << millisec << " [ms]" << std::endl;

        vector<long double> ave_time1(N+1), ave_time2(N+1);
        vector<long double> lce_cnt1(N+1), lce_cnt2(N+1);
        vector<long double> cmp1(N+1), cmp2(N+1);

        auto start = std::chrono::high_resolution_clock::now();
        auto end = start;

        random_device seed;
        mt19937 mt(seed());
        uniform_int_distribution<int> dist2(0,N-1);

        while(std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() <= 30*60000) {
            int i = dist2(mt);
            int j = dist2(mt);
            if(i == j) continue;

            int l = 0;

            long double min_time = numeric_limits<long double>::max();

            auto s1 = std::chrono::high_resolution_clock::now();
            auto e1 = std::chrono::high_resolution_clock::now();
            long double m1;
            int ret;

            for(int _ = 0; _ < 10; _++) {
                s1 = std::chrono::high_resolution_clock::now();
                ret = 0;
                l = RL.LCE(i,j,ret);
                total_lce += l;
                e1 = std::chrono::high_resolution_clock::now();
                m1 = std::chrono::duration_cast<std::chrono::nanoseconds>(e1 - s1).count();
                min_time = min(min_time,m1);
            }

            if(lce_cnt1[i] >= 1000) continue;

            ave_time1[l] = ((ave_time1[l] * lce_cnt1[l]) + min_time)/(lce_cnt1[l] + 1);
            cmp1[l] = ((cmp1[l]*lce_cnt1[l] + ret))/(lce_cnt1[l] + 1);
            lce_cnt1[l]++;

            min_time = numeric_limits<long double>::max();
            
            for(int _ = 0; _ < 10; _++) {
                s1 = std::chrono::high_resolution_clock::now();
                ret = 0;
                l = RL.LCE2(i,j,ret);
                e1 = std::chrono::high_resolution_clock::now();
                m1 = std::chrono::duration_cast<std::chrono::nanoseconds>(e1 - s1).count();
                min_time = min(m1,min_time);
            }

            ave_time2[l] = ((ave_time2[l] * lce_cnt2[l]) + min_time)/(lce_cnt2[l] + 1);
            cmp2[l] = ((cmp2[l]*lce_cnt1[l] + ret))/(lce_cnt1[l] + 1);
            lce_cnt2[l]++;

            end = std::chrono::high_resolution_clock::now();
        }

        // out_stream << "total_lce is " << total_lce << endl;

        int cnt = 0;

        for(int i = 0; i < N+1; i++) if(lce_cnt1[i]) cnt++;

        //out_stream << cnt << endl;
        for(int i = 0; i < N+1; i++) {
            if(lce_cnt1[i]) {
                out_stream << i << " " << ave_time1[i] << " " << ave_time2[i] << " " << cmp1[i] << " " << cmp2[i] << endl;
            }
        }
    }

    return 1;
}

