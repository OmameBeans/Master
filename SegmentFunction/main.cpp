#include <iostream>
#include <random>
#include "SegmentFunction_via_RLSLP.hpp"

using namespace std;

int gcd(int a, int b) {
    if(b == 0) return a;
    else return gcd(b,a%b);
}

int main() {
    auto f = [](int a, int b) -> int{return max(a,b);};
    random_device seed;
    mt19937 mt(seed());
    uniform_int_distribution<int> dist1(1,999);
    int e = 0;
    int N; cin >> N;
    vector<int> A(N);
    for(int i = 0; i < N; i++) {
        A[i] = dist1(mt);
        cout << A[i] << " ";
    }
    cout << endl;
    Segment_Function<int> Seg(A,1000,f,e);
    Seg.bulid();

    int Q; cin >> Q;
    while(Q--) {
        uniform_int_distribution<int> dist2(1,N);
        int l = dist2(mt);
        int r = dist2(mt);
        l--;
        if(l > r) swap(l,r);
        //cout << "[ " << l << " , " << r << "]" << endl;
        int ans1 = Seg.query(l,r);
        int ans2 = e;
        for(int i = l; i < r; i++) {
            ans2 = f(ans2,A[i]);
        }
        if(ans1 != ans2) {
            cout << "diff " <<  "[ " << l << " , " << r << "]" << endl;
        }
    }
}