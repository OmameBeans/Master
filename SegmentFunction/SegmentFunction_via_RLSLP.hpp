#include<iostream>
#include<string>
#include<unordered_map>
#include<vector>
#include<set>
#include<stack>
#include<tuple>
#include <chrono>
#include <random>
#include <functional>
#include "HashPair.hpp"

using namespace std;
using ll = long long;

template<typename T>
struct Segment_Function {
    using Fx = function<T(T,T)>;
    ll var;
    int str_size;
    T MAX;
    vector<T> Left_ch,Right_ch,is_RL,str,pre_str,length, val;
    unordered_map<pair<T,T>,T,HashPair> RL_par, not_RL_par;

    Fx f;
    T e;

    Segment_Function(vector<T> _str, T _MAX, Fx _f, T _e) : str(_str), MAX(_MAX), f(_f), e(_e) , var(_MAX), str_size(_str.size()){
    }

    void cal_len() {
        length = vector<int>(var);
        for(int i = 0; i < var-MAX; i++) {
            T a = Left_ch[i], b = Right_ch[i];
            if(is_RL[i]) {
                if(b < MAX) length[i] += a; else length[i] += a*length[b-MAX];
            } else {
                if(a < MAX) length[i] += 1; else length[i] += length[a-MAX];
                if(b < MAX) length[i] += 1; else length[i] += length[b-MAX];
            }
        }
    }

    bool get_RL_par(int l, int r, int &par) {
        if(RL_par.count({l,r})) { par = RL_par[{l,r}]; return true;}
        else return false;
    }

    bool get_not_RL_par(int l, int r, int &par) {
        if(not_RL_par.count({l,r})) { par = not_RL_par[{l,r}]; return true;}
        else return false;
    }

    void BlockComp() {
        pre_str = str;
        str = vector<T>();
        int N = pre_str.size();
        int idx = 0;
        while(idx < N) {
            ll cnt = 1;
            auto val = pre_str[idx];
            while(idx+cnt < N && val == pre_str[idx+cnt]) cnt++;
            idx += cnt;

            if(cnt == 1) str.emplace_back(val);
            else {
                int par;
                if(get_RL_par(cnt,val,par)) str.emplace_back(par);
                else {
                    RL_par[{cnt,val}] = var;
                    is_RL.emplace_back(1);
                    str.emplace_back(var);
                    Left_ch.emplace_back(cnt), Right_ch.emplace_back(val);
                    var++;
                }
            }
        }
    }

    void PairComp() {
        pre_str = str;
        str = vector<T>();
        unordered_map<T,int> who, visited;
        int N = pre_str.size();

        random_device seed;
        mt19937 mt(seed());
        uniform_int_distribution<int> dist(0,1);

        for(int i = 0; i < N; i++) {
            if(!visited[pre_str[i]]) {
                visited[pre_str[i]] = 1;
                who[pre_str[i]] = dist(mt);
            }

            auto now_char = pre_str[i];

            if(str.size() == 0) str.emplace_back(now_char);
            else {
                auto pre_char = str.back();
                if(who[pre_char] == 0 && who[now_char] == 1) {
                    str.pop_back();
                    int par;
                    if(get_not_RL_par(pre_char,now_char,par)) {
                        str.emplace_back(par);
                    } else {
                        not_RL_par[{pre_char,now_char}] = var;
                        str.emplace_back(var);
                        is_RL.emplace_back(0);
                        Left_ch.emplace_back(pre_char), Right_ch.emplace_back(now_char);
                        var++;
                    }
                } else {
                    str.emplace_back(now_char);
                }
            }
        }
    }

    void cal_val() {
        val.assign(var-MAX+1,e);
        for(int i = 0; i < val.size(); i++) {
            T l = Left_ch[i];
            T r = Right_ch[i];
            if(is_RL[i]) {
                if(r < MAX) {
                    val[i] = r;
                } else {
                    val[i] = val[r-MAX];
                }
            } else {
                if(l >= MAX) l = val[l-MAX];
                if(r >= MAX) r = val[r-MAX];
                val[i] = f(l,r);
            }
        }
    }

    void bulid() {
        while(str.size() > 1) {
            BlockComp();
            for(auto x : str) cout << x << " ";
            cout << endl;
            PairComp();
            for(auto x : str) cout << x << " ";
            cout << endl;
        }
        cal_len();
        cal_val();

        for(int i = 0; i < Left_ch.size(); i++) {
            cout << i+MAX << " " << val[i] << " ---> " << Left_ch[i] << " " << Right_ch[i] << endl;
        }
    }

    T query(int a, int b) {
        return query_sub(a,b,0,str_size,var-1);
    }

    T query_sub(int a, int b, int l, int r, T cur) {
        if(a >= b) return e;
        if(r <= a || b <= l) return e;
        else if(a <= l && r <= b) {
            if(cur < MAX) return cur;
            else return val[cur-MAX];
        } else {
            T v1,v2;
            if(cur < MAX) return cur;
            if(is_RL[cur-MAX]) {
                return query_sub(a,b,l,r,Right_ch[cur-MAX]);
            } else {
                int left_len;
                if(Left_ch[cur-MAX] < MAX) left_len = 1;
                else left_len = length[Left_ch[cur-MAX]-MAX];
                v1 = query_sub(a,b,l,l+left_len,Left_ch[cur-MAX]);
                v2 = query_sub(a,b,l+left_len,r,Right_ch[cur-MAX]);
                return f(v1,v2);
            }
        }
    }
};