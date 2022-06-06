#include <iostream>
#include <vector>
#include <algorithm>
#include <random>
#include <chrono>

#include "../RLSLP/Program/RLSLP.h"

using namespace std;

int main() {
    int N,K; cin >> N >> K;
    string T = "";
    random_device seed;
    mt19937 mt(seed());
    normal_int_distribution<int> dist(0,25);
    chrono::system_clock::time_point  start, end;
    double time;

    for(int i = 0; i < N; i++) {
        T += (char)('a' + dist(mt));
    }

    string S = "";
    for(int _ = 0; _ < K; _++) S += T;
    S += '$';

    //cout << S << endl;

    start = chrono::system_clock::now();
    auto nSA = vector<int>(S.size());
    iota(nSA.begin(),nSA.end(),0);
    sort(nSA.begin(),nSA.end(),[&](int a,int b){
        return S.substr(a,N-a) < S.substr(b,N-b);
    });
    end = chrono::system_clock::now();
    time = chrono::duration_cast<std::chrono::milliseconds>(end-start).count();
    printf("SuffixArray(愚直)の構築時間 : %.10f [ms] \n", time);

}