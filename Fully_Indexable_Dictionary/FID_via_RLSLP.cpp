#include <iostream>
#include <vector>
#include <algorithm>
#include <random>
#include <chrono>

#include "../RLSLP/RLSLP.h"

using namespace std;

struct FID_RLSLP : RLSLP {

    using RLSLP::RLSLP;

    vector<int> num0; //各変数における0の数

    //num0[i]を計算する
    void cal_num0() {
        num0.assign(var-MAX,0);
        for(int i = 0; i < var-MAX; i++) {
            int left_ch = Left_ch[i];
            int right_ch = Right_ch[i];

            if(is_RL[i]) {
                if(right_ch < MAX) {
                    if(right_ch == 0) num0[i] += left_ch;
                } else {
                    num0[i] += left_ch*num0[right_ch-MAX];
                }
            } else {
                if(right_ch < MAX) {
                    if(right_ch == 0) num0[i] += 1;
                } else {
                    num0[i] += num0[right_ch-MAX];
                }
                if(left_ch < MAX) {
                    if(left_ch == 0) num0[i] += 1;
                } else {
                    num0[i] += num0[left_ch-MAX];
                }
            }
        }
    }

    void print_num0() {
        for(auto c : num0) cout << c << " ";
        cout << endl;
    }

    void built() {
        for(auto &x : str) x -= 48;
        MAX = var = 2;
        StoRLSLP();
        cal_num0();
        print_length();
        print_num0();
    }

    int access(int i) {
        return get_char(i);
    }

    // int rank0(int i) {

    // }

    // int rank1(int i) {

    // }

    //左からi番目の0を探す
    int select0(int i) {
        int x = var-1;
        int l = 0, r = N;
        while(r-l > 1) {
            //cout << l << " " << r << endl;
            if(is_RL[x-MAX]) {
                int len = (Right_ch[x-MAX] < MAX ? 1 : length[Right_ch[x-MAX]-MAX]);
                int num = 
                int k = i/num;
                int prel = l;
                l = prel+k*len, r = prel+(k+1)*len;
                x = Right_ch[x-MAX];
            } else {
                int left_len = (Left_ch[x-MAX] < MAX ? 1 : length[Left_ch[x-MAX]-MAX]);
                if(i < l + left_len) r = l+left_len, x = Left_ch[x-MAX];
                else l = l+left_len, x = Right_ch[x-MAX];
            }
        }
        return x;
    }

    //左からi番目の1を探す
    // int select1(int i) {
    // }
}; 

int main() {
    string S; cin >> S;
    FID_RLSLP FID(S);
    FID.built();

    cout << FID.select0(1) << endl;
    cout << FID.select0(2) << endl;
    cout << FID.select0(3) << endl;
}