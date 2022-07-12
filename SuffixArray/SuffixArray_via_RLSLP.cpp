#include<iostream>
#include<string>
#include<unordered_map>
#include<vector>
#include <cstdio>
#include <string>
#include <fstream>
#include <chrono>
#include <random>
#include <algorithm>
#include "../RLSLP/RLSLP.h"
#include "../RLSLP/cmdline.h"

using namespace std;

struct RollingHash {
    int N;
    long long B, MOD;
    vector<long long> Hash, inv;

    long long modpow(long long a, long long n, long long mod) {
        if(n == 0) return 1;
        long long t = modpow(a,n/2,mod);
        t = t*t;
        t %= mod;
        if(n & 1) {
            t *= a;
            t %= MOD;
        }
        return t;
    }
    
    RollingHash(string S, long long _B, long long _MOD) : N(S.size()), B(_B), MOD(_MOD) {
        Hash.assign(N+1,0LL);
        inv.assign(N+1,1LL);
        long long t = 1;
        long long f = modpow(B,MOD-2,MOD);
        for(int i = 0; i < N; i++) {
            Hash[i+1] = Hash[i] + (t * (long long)(S[i]))%MOD;
            Hash[i+1] %= MOD;
            inv[i+1] = inv[i]*f;
            inv[i+1] %= MOD;
            t *= B;
            t %= MOD;
        }
    }

    long long Substring(int pos, int sz) {
        long long ans = Hash[pos+sz] - Hash[pos];
        ans += MOD;
        ans %= MOD;
        ans *= inv[pos];
        ans %= MOD;
        return ans;
    }

    int LCE(int i, int j) {
        int l = -1, r = N;
        while(r-l > 1) {
            int mid = (r+l)/2;
            if(i+mid > N || j+mid > N) {
                r = mid;
                continue;
            }
            if(Substring(i,mid) == Substring(j,mid)) l = mid;
            else r = mid;
        }
        return l;
    }
};

int lce(int i, int j, string &S) {
    int ans = 0;
    while(i+ans < S.size() && j+ans < S.size() && S[i+ans] == S[j+ans]) ans++;
    return ans;
}


int main(int argc, char *argv[]) {
    cmdline::parser p; // parser を定義
    p.add<string>("input_file",  'i', "input file name",  true);
    p.add<string>("output_file", 'o', "output file name", true);

    p.parse_check(argc, argv);
    const string in_filename = p.get<string>("input_file");
    const string out_filename = p.get<string>("output_file") + "_SAffixArray.txt";

    std::ifstream in_stream(in_filename);
    std::ofstream out_stream(out_filename);

    cout << "in_filename = " << in_filename << endl;
    cout << "out_filename = " << out_filename << endl;
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
            if(c != ' ' && c != '\n' && S.size() < 500000) S += c; // 文字を書き込む
        }

        int N = S.size();

        out_stream << "in_file size is " << N << endl;
        cout << "in_file size is " << N << endl;

        chrono::system_clock::time_point  start, end;
        double time;

        long long ans = 0;
        long long query_cnt = 0;

        start = chrono::system_clock::now();
        RLSLP rlslp(S);
        rlslp.StoRLSLP();
        rlslp.cal_len();
        //rlslp.print_rules();
        end = chrono::system_clock::now();
        time = chrono::duration_cast<std::chrono::milliseconds>(end-start).count();
        out_stream << "導出木の高さ : " << rlslp.get_hight() << " " << "文法サイズ : " << rlslp.get_num_var() << endl;
        cout << "導出木の高さ : " << rlslp.get_hight() << " " << "文法サイズ : " << rlslp.get_num_var() << endl;
        out_stream << "RLSLPの構築時間 : " <<  time <<"[ms] \n";
        cout<< "RLSLPの構築時間 : " <<  time <<"[ms] \n";

        start = chrono::system_clock::now();
        vector<int> SA(S.size());
        iota(SA.begin(),SA.end(),0);
        sort(SA.begin(),SA.end(),[&](int a, int b) {
            int l = rlslp.LCE(a,b);
            query_cnt++;
            ans += l;
            return S[a+l] < S[b+l];
        });
        end = chrono::system_clock::now();
        time = chrono::duration_cast<std::chrono::milliseconds>(end-start).count();
        out_stream << "SuffixArray(RLSLP) : " <<  time <<"[ms] \n";
        cout << "SuffixArray(RLSLP) : " <<  time <<"[ms] \n";

        // {
        //     long long B = 1007, MOD = 1000000007;
        //     RollingHash RH(S,B,MOD);
        //     start = chrono::system_clock::now();
        //     vector<int> SA(S.size());
        //     iota(SA.begin(),SA.end(),0);
        //     sort(SA.begin(),SA.end(),[&](int a, int b) {
        //         int l = RH.LCE(a,b);
        //         return S[a+l] < S[b+l];
        //     });
        //     end = chrono::system_clock::now();
        //     time = chrono::duration_cast<std::chrono::milliseconds>(end-start).count();
        //     out_stream << "SuffixArray(RollingHash) : " <<  time <<"[ms] \n";
        //     cout << "SuffixArray(RollingHash) : " <<  time <<"[ms] \n";
        // }

        start = chrono::system_clock::now();
        auto nSA = vector<int>(S.size());
        iota(nSA.begin(),nSA.end(),0);
        sort(nSA.begin(),nSA.end(),[&](int a,int b){
            int l = 0;
            while(a+l < N && b+l < N && S[a+l] == S[b+l]) l++;
            return S[a+l] < S[b+l];
        });
        end = chrono::system_clock::now();
        time = chrono::duration_cast<std::chrono::milliseconds>(end-start).count();
        out_stream << "SuffixArray(愚直) : " <<  time <<"[ms] \n";
        cout<< "SuffixArray(愚直) : " <<  time <<"[ms] \n";


        out_stream << "LCEの合計 : " << ans << " LCEを呼び出した回数 : " << query_cnt << endl;
        cout << "LCEの合計 : " << ans << " LCEを呼び出した回数 : " << query_cnt << endl;
        out_stream << "LCEの平均 : " << (double)ans/query_cnt << endl;
        cout << "LCEの平均 : " << (double)ans/query_cnt << endl;
    }

    return 1;
}

// int main() {
//     string T = "";
//     chrono::system_clock::time_point  start, end;
//     double time;

//     for(int i = 0; i < N; i++) {
//         T += (char)('a' + dist(mt));
//     }

//     string S = "";
//     for(int _ = 0; _ < K; _++) S += T;
//     S += '$';

//     //cout << S << endl;

//     start = chrono::system_clock::now();
//     RLSLP rlslp(S);
//     rlslp.StoRLSLP();
//     rlslp.cal_len();
//     end = chrono::system_clock::now();
//     time = chrono::duration_cast<std::chrono::milliseconds>(end-start).count();
//     printf("RLSLPの構築時間 : %.10f [ms] \n", time);

//     start = chrono::system_clock::now();
//     vector<int> SA(S.size());
//     iota(SA.begin(),SA.end(),0);
//     sort(SA.begin(),SA.end(),[&](int a, int b) {
//         int l = rlslp.LCE(a,b);
//         return S[a+l] < S[b+l];
//     });
//     end = chrono::system_clock::now();
//     time = chrono::duration_cast<std::chrono::milliseconds>(end-start).count();
//     printf("SuffixArray(RLSLP)の構築時間 : %.10f [ms] \n", time);

//     start = chrono::system_clock::now();
//     auto nSA = vector<int>(S.size());
//     iota(nSA.begin(),nSA.end(),0);
//     sort(nSA.begin(),nSA.end(),[&](int a,int b){
//         return S.substr(a,N-a) < S.substr(b,N-b);
//     });
//     end = chrono::system_clock::now();
//     time = chrono::duration_cast<std::chrono::milliseconds>(end-start).count();
//     printf("SuffixArray(愚直)の構築時間 : %.10f [ms] \n", time);

    // for(int i = 0; i < S.size(); i++) {
    //     if(SA[i] != nSA[i]) {
    //         printf("diff at idx %d \n", i);
    //     }
    // }

// }