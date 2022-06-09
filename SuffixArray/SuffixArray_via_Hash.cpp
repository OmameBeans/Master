#include <iostream>
#include <vector>
#include <algorithm>
#include <random>
#include <chrono>

#include "../RLSLP/RLSLP.h"

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

int main() {
    int N,K; cin >> N >> K;
    string T = "";
    random_device seed;
    mt19937 mt(seed());
    uniform_int_distribution<int> dist(0,25);
    chrono::system_clock::time_point  start, end;
    double time;

    for(int i = 0; i < N; i++) {
        T += (char)('a' + dist(mt));
    }

    string S = "";
    for(int _ = 0; _ < K; _++) S += T;
    S += '$';

    long long B = 1007, MOD = 1000000007;

    RollingHash RH(S,B,MOD);

    start = chrono::system_clock::now();
    vector<int> SA(S.size());
    iota(SA.begin(),SA.end(),0);
    sort(SA.begin(),SA.end(),[&](int a, int b) {
        int l = RH.LCE(a,b);
        return S[a+l] < S[b+l];
    });
    end = chrono::system_clock::now();
    time = chrono::duration_cast<std::chrono::milliseconds>(end-start).count();
    printf("SuffixArray(Hash)の構築時間 : %.10f [ms] \n", time);

    start = chrono::system_clock::now();
    auto nSA = vector<int>(S.size());
    iota(nSA.begin(),nSA.end(),0);
    sort(nSA.begin(),nSA.end(),[&](int a,int b){
        return S.substr(a,N-a) < S.substr(b,N-b);
    });
    end = chrono::system_clock::now();
    time = chrono::duration_cast<std::chrono::milliseconds>(end-start).count();
    printf("SuffixArray(愚直)の構築時間 : %.10f [ms] \n", time);

    for(int i = 0; i < S.size(); i++) {
        if(SA[i] != nSA[i]) {
            printf("diff at idx %d \n", i);
        }
    }
}