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

int main() {
    int N; cin >> N;
    string S = "";
    random_device seed;
    mt19937 mt(seed());
    uniform_int_distribution<int> dist(0,25);

    for(int i = 0; i < N; i++) S += (char)('a' + dist(mt));

    long long total_lce = 0;

        auto t1 = std::chrono::high_resolution_clock::now(); // 現在時刻を取得

        RLSLP RL(S);
        RL.StoRLSLP();
        RL.cal_len();

        auto t2 = std::chrono::high_resolution_clock::now(); // 現在時刻を取得
        double millisec = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count(); // 実行時間のミリ秒を計算
        cout << "導出木の高さ : " << RL.get_hight() << " " << "文法サイズ : " << RL.get_num_var() << endl;
        cout << "RLSLPでの構築時間 : " << millisec << " [ms]" << std::endl;

        vector<double> ave_time1(N+1), ave_time2(N+1);
        vector<double> lce_cnt1(N+1), lce_cnt2(N+1);

        uniform_int_distribution<int> dist2(0,N-1);

        auto start = std::chrono::high_resolution_clock::now();
        auto end = start;

        while(chrono::duration_cast<std::chrono::milliseconds>(end - start).count() <= 120000) {
            int i = dist2(mt);
            int j = dist2(mt);
            if(i == j) continue;

            int l = 0;

            auto s1 = std::chrono::high_resolution_clock::now();
            l = RL.LCE(i,j);
            total_lce += l;
            auto e1 = std::chrono::high_resolution_clock::now();
            double m1 = std::chrono::duration_cast<std::chrono::milliseconds>(e1 - s1).count();

            ave_time1[l] = ((ave_time1[l] * lce_cnt1[l]) + m1)/(lce_cnt1[l] + 1);
            lce_cnt1[l]++;

            s1 = std::chrono::high_resolution_clock::now();
            l = RL.LCE2(i,j);
            e1 = std::chrono::high_resolution_clock::now();
            m1 = std::chrono::duration_cast<std::chrono::milliseconds>(e1 - s1).count();

            ave_time2[l] = ((ave_time2[l] * lce_cnt2[l]) + m1)/(lce_cnt2[l] + 1);
            lce_cnt2[l]++;

            end = std::chrono::high_resolution_clock::now();
        }

        for(int i = 0; i < N; i++) cout << ave_time1[i] << " " << ave_time2[i] << endl;
}