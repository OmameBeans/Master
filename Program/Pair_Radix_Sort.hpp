#include<iostream>
#include<vector>
#include<random>
#include <algorithm>
#include <cassert>
#include <chrono>

using namespace std;

void Bucket_Sort_First(vector<pair<int,int>> &A, int N, int dig) {
    vector<int> cnt(10);
    auto tmp = A;

    for(int i = 0; i < N; i++) {
        cnt[(tmp[i].first/dig)%10]++;
    }

    for(int i = 0; i < 9; i++) {
        cnt[i+1] += cnt[i];
    }

    for(int i = N-1; i >= 0; i--) {
        A[cnt[(tmp[i].first/dig)%10]-1] = tmp[i];
        cnt[(tmp[i].first/dig)%10]--;
    }
}

void Radix_Sort_First(vector<pair<int,int>> &A, int N, int max_value) {
    for(int dig = 1; max_value/dig > 0; dig *= 10) {
        Bucket_Sort_First(A,N,dig);
    }
}

void Bucket_Sort_Second(vector<pair<int,int>> &A, int N, int dig, int sp, int len) {
    vector<int> cnt(10);
    vector<pair<int,int>> tmp(len);

    for(int i = sp; i < N && i < sp+len; i++) {
        tmp[i-sp] = A[i];
        cnt[(A[i].second/dig)%10]++;
    }

    for(int i = 0; i < 9; i++) cnt[i+1] += cnt[i];

    for(int i = sp+len-1; i >= sp; i--) {
        A[sp+cnt[(tmp[i-sp].second/dig)%10]-1] = tmp[i-sp];
        cnt[(tmp[i-sp].second/dig)%10]--;
    }
}

void Radix_Sort_Second(vector<pair<int,int>> &A, int N, int max_value, int sp, int len) {
    if(sp+len > N) {
        cout << sp << " " << len << " " << N << endl;
        return;
    }
    if(sp < 0 || sp >= N) return;

    //cout << sp << " " << len << endl;

    for(int dig = 1; max_value/dig > 0; dig *= 10) {
        Bucket_Sort_Second(A,N,dig,sp,len);
    }
}

void Pair_Radix_Sort(vector<pair<int,int>> &A, int N, int max_value) {

    Radix_Sort_First(A,N,max_value);

    // cout << "-----------" << endl;
    // for(int i = 0; i < N; i++) cout << A[i].first << " " << A[i].second << endl;
    // cout << "-----------" << endl;

    int sp = -1; int len = 0, cur = -1;

    for(int i = 0; i < N; i++) {
        if(cur == A[i].first) {
            len++;
        } else {
            Radix_Sort_Second(A,N,max_value,sp,len);
            len = 0;
            len++;
            sp = i;
            cur = A[i].first;
        }
    }

    //Radix_Sort_Second(A,N,max_value,sp,len);
}