#include <iostream>
#include <vector>
#include<unordered_map>
#include <random>

using namespace std;

struct HashPair {

    static size_t m_hash_pair_random;

    template<class T1, class T2>
    size_t operator()(const pair<T1, T2> &p) const {

        auto hash1 = hash<T1>{}(p.first);
        auto hash2 = hash<T2>{}(p.second);

        size_t seed = 0;
        seed ^= hash1 + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        seed ^= hash2 + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        seed ^= m_hash_pair_random + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        return seed;
    }
};

size_t HashPair::m_hash_pair_random = (size_t) random_device()();

const int MAX = 256;
int var;
int n;

struct node{
    int number;
    int len;
};

vector<node> D;
vector<node> new_D;
vector<int> len;
vector<node> LML;
vector<node> RML;

unordered_map<pair<int,int>,int,HashPair> RL_par, not_RL_par;

void SimTtoG(int N, vector<int> L, vector<int> R) {
    len.assign(n,0);
    LML.assign(n,{-1,0});
    RML.assign(n,{-1,0});
    var = 0;
    n = N;

    for(int i = 0; i < n; i++) {
        D.push_back({L[i],1});
        D.push_back({R[i],1});
        len[i] = 2;
    }

    return;
}

void cal_LML_and_RML(vector<node> D, vector<node> &LML, vector<node> &RML) {
    int cur = 0;
    for(int i = 0; i < n; i++) {
        if(len[i] == 0) continue;
        int X_l = -1,X_r = -1;
        int l_idx = cur, r_idx = cur+len[i]-1;

        if(!(MAX <= D[l_idx].number && D[l_idx].number < MAX+n)) LML[i] = D[l_idx];
        if(!(MAX <= D[r_idx].number && D[r_idx].number < MAX+n)) RML[i] = D[r_idx];

        for(int j = cur; j < cur+len[i]; j++) {
            if(MAX <= D[j].number && D[j].number < MAX+n) {
                if(X_l == -1) if(LML[j].number == -1) LML[j] = LML[D[j].number-MAX];
                else if(RML[j].number == -1) RML[j] = RML[D[j].number-MAX];
            }
        }
        cur += len[i];
    }
}

void PComp(vector<node> D) {
    auto pre_D = D;
    D.clear();
    unordered_map<int,int> LorR;

    LML.assign(n,{-1,0});
    RML.assign(n,{-1,0});

    cal_LML_and_RML(D,LML,RML);

    mt19937 mt(0);
    uniform_int_distribution<int> rd(0,1);
    for(auto d : pre_D) {
        if((0 <= d.number && d.number < MAX) || (MAX+n <= d.number && d.number < MAX+n+var)) {
            if(!LorR.count(d.number)) LorR[d.number] = rd(mt);
            else continue;
        }
    }

    int cur = 0;
    for(int i = 0; i < n; i++) {
        if(len[i] == 0) continue;
        new_D.clear();
        int l_idx = cur, r_idx = cur+len[i];

        for(int j = l_idx; j < r_idx; j++) {
            auto d = pre_D[j];
            int num = d.number;
            if(j == 0) {
                if(!(MAX <= num && num < MAX+n) && LorR[num] == 1) {
                    continue;
                } else {
                    new_D.push_back(d);
                }
            } else if(j == r_idx-1) {
                if(!(MAX <= num && num < MAX+n) && LorR[num] == 0) {
                    continue;
                } else {
                    new_D.push_back(d);
                }
            } else {
                if(MAX <= num && num < MAX+n) {
                    if(LorR[LML[num-MAX].number] == 1) {
                        // LML[X_i]X_iに変換
                        if(new_D.size()) {
                            node p = new_D.back();
                            if(LorR[p.number] == 0) {
                                new_D.pop_back();
                                if(not_RL_par[{p.number,LML[num-MAX].number}]) {
                                    new_D.push_back({not_RL_par[{p.number,LML[num-MAX].number}],1});
                                } else {
                                    var++;
                                    new_D.push_back({p.number,LML[num-MAX].number});
                                    not_RL_par[{p.number,LML[num-MAX].number}] =  MAX+n+var;
                                }
                            } else {
                                new_D.push_back(LML[num-MAX]);
                            }
                        } else {
                            new_D.push_back(LML[num-MAX]);
                        }
                    }
                    new_D.push_back(d);
                    if(LorR[RML[num-MAX].number] == 0) {
                        //X_iRML[X_i]に変換
                        new_D.push_back(RML[num-MAX]);
                    }
                } else {
                    if(LorR[num] == 1) {
                        if(new_D.size()) {
                            node p = new_D.back();
                            if(LorR[p.number] == 0) {
                                new_D.pop_back();
                                if(not_RL_par[{p.number,num}]) {
                                    new_D.push_back({not_RL_par[{p.number,num}],1});
                                } else {
                                    var++;
                                    new_D.push_back({p.number,num});
                                    not_RL_par[{p.number,num}] =  MAX+n+var;
                                }
                            } else {
                                new_D.push_back(d);
                            }
                        } else {
                            new_D.push_back(d);
                        }
                    } else {
                        new_D.push_back(d);
                    }
                }
            }
        }
        cur += len[i];
        len[i] = new_D.size();
        for(auto x : new_D) D.push_back(x);
    }
}

void BComp(vector<node> D) {
    
}