#include<iostream>
#include<string>
#include<unordered_map>
#include<vector>
#include<set>
#include<stack>
#include<tuple>
#include <chrono>
#include <random>

#include "Pair_Radix_Sort.hpp"

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


struct RLSLP{
    int var; //変数
    int N; //|S|
    int MAX = 256; //アルファベットサイズ
    vector<int> Left_ch, Right_ch;//左の子，右の子
    unordered_map<pair<int,int>,int,HashPair> RL_par, not_RL_par; //親へのハッシュ
    vector<int> is_RL; //ある変数の子が連長圧縮かどうか
    vector<int> str, pre_str;
    vector<int> length;

    RLSLP(const string S) : N(S.size()){
        var = MAX;
        for(auto c : S) str.emplace_back((int)c);
    }

    void print_str() {
        for(auto c : str) cout << c << " ";
        cout << endl;
    }

    void print_length() {
        for(auto c : length) cout << c << " ";
        cout << endl;
    }

    void print_rules() {
        cout << "規則集合 X -> A B の形式" << endl;
        for(int i = 0; i < var-MAX; i++) {
            cout << i+MAX << " -> " << Left_ch[i] << " " << Right_ch[i] << " 長さは" << length[i]<< endl;
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

    //strをBlockCompする O(|str|)
    void BlockComp() {
        //pre_str:変換前の文字列
        vector<pair<int,int>> block;
        pre_str = str;
        str = vector<int>();
        int N = pre_str.size();
        int idx = 0;
        while(idx < N) {
            int cnt = 1;
            auto val = pre_str[idx];
            while(idx+cnt < N && val == pre_str[idx+cnt]) cnt++;
            idx += cnt;
            block.push_back({val,cnt});
        }

        for(auto pr : block) {
            auto val = pr.first;
            auto len = pr.second;
            if(len == 1) str.emplace_back(val);
            else {
                int par;
                if(get_RL_par(len,val,par)) str.emplace_back(par);
                else {
                    RL_par[{len,val}] = var;
                    is_RL.emplace_back(1);
                    str.emplace_back(var);
                    Left_ch.emplace_back(len), Right_ch.emplace_back(val);
                    var++;
                }
            }
        }
    }

    //strをPairCompする O(|w|log|w|)
    void PairComp() {
        pre_str = str;
        str = vector<int>();
        vector<pair<int,int>> edge;
        int N = pre_str.size();

        for(int i = 0; i < N-1; i++) {
            edge.push_back({pre_str[i],pre_str[i+1]});
            edge.push_back({pre_str[i+1],pre_str[i]});
        }
        Pair_Radix_Sort(edge,edge.size(),var+10);
        long long cur = edge[0].first;

        unordered_map<int,int> who;

        int left_cnt = 0, right_cnt = 0;
        for(int i = 0; i < 2*(N-1);) {
            if(cur == edge[i].first) {
                int b = edge[i].second;
                if(who[b] == 0) left_cnt++;
                else if(who[b] == 1) right_cnt++;
                i++;
            } else {
                if(left_cnt <= right_cnt) {
                    who[cur] = 0;
                } else who[cur] = 1;
                cur = edge[i].first;
                left_cnt = right_cnt = 0;
            }
        }
        if(left_cnt <= right_cnt) {
            who[cur] = 0;
        } else who[cur] = 1;

        int LR = 0, RL = 0;
        for(int i = 0; i < N-1; i++) {
            if(who[pre_str[i]] == 0 && who[pre_str[i+1]] == 1) LR++;
            else if(who[pre_str[i]] == 1 && who[pre_str[i+1]] == 0) RL++;
        }

        bool rev;
        rev = (LR < RL);
        if(rev) swap(LR,RL);

        //cout << "元の文字列から" << (double)(pre_str.size()-LR)/pre_str.size() << "小さくなりました" << endl;

        //L,Rのペアがあればまとめる
        for(int i = 0; i < N;) {
            if(i < N-1 && who[pre_str[i]] == (rev ? 1 : 0) && who[pre_str[i+1]] == (rev ? 0 : 1)) {
                int a = pre_str[i];
                int b = pre_str[i+1];
                int par;
                if(get_not_RL_par(a,b,par)) {
                    str.emplace_back(par);
                } else {
                    not_RL_par[{a,b}] = var;
                    str.emplace_back(var);
                    is_RL.emplace_back(0);
                    Left_ch.emplace_back(a), Right_ch.emplace_back(b);
                    var++;
                }
                i += 2;
            } else {
                str.emplace_back(pre_str[i]);
                i++;
            }
        }       
    }

    void PairComp2() {
        pre_str = str;
        str = vector<int>();
        unordered_map<int,int> who, visited;
        int N = pre_str.size();

        random_device seed;
        mt19937 mt(seed());
        uniform_int_distribution<int> dist(0,1);

        for(int i = 0; i < N; i++) {
            if(visited[pre_str[i]]) continue;
            visited[pre_str[i]] = 1;
            who[pre_str[i]] = dist(mt);
        }

        // cout << "-------------" << endl;
        // for(int i = 0; i < N; i++) cout << who[pre_str[i]] << " ";
        // cout << endl;

        int LR = 0, RL = 0;
        for(int i = 0; i < N-1; i++) {
            if(who[pre_str[i]] == 0 && who[pre_str[i+1]] == 1) LR++;
            else if(who[pre_str[i]] == 1 && who[pre_str[i+1]] == 0) RL++;
        }

        bool rev;
        rev = (LR < RL);
        if(rev) swap(LR,RL);

        //cout << "元の文字列から" << (double)(pre_str.size()-LR)/pre_str.size() << "小さくなりました" << endl;

        //L,Rのペアがあればまとめる
        for(int i = 0; i < N;) {
            if(i < N-1 && who[pre_str[i]] == (rev ? 1 : 0) && who[pre_str[i+1]] == (rev ? 0 : 1)) {
                int a = pre_str[i];
                int b = pre_str[i+1];
                int par;
                if(get_not_RL_par(a,b,par)) {
                    str.emplace_back(par);
                } else {
                    not_RL_par[{a,b}] = var;
                    str.emplace_back(var);
                    is_RL.emplace_back(0);
                    Left_ch.emplace_back(a), Right_ch.emplace_back(b);
                    var++;
                }
                i += 2;
            } else {
                str.emplace_back(pre_str[i]);
                i++;
            }
        }       
    }

    void cal_len() {
        length = vector<int>(var);
        for(int i = 0; i < var-MAX; i++) {
            int a = Left_ch[i], b = Right_ch[i];
            if(is_RL[i]) {
                if(b < MAX) length[i] += a; else length[i] += a*length[b-MAX];
            } else {
                if(a < MAX) length[i] += 1; else length[i] += length[a-MAX];
                if(b < MAX) length[i] += 1; else length[i] += length[b-MAX];
            }
        }
        // cout << var << endl;
        // for(int i = 0; i < var-MAX; i++) cout << length[i] << " ";
        // cout << endl;
    }

    //SをRLSLPに変換して，開始記号を返す．
    int StoRLSLP() {
        while(str.size() > 1) {
            BlockComp();
            if(str.size() < 2) break;
            PairComp2();
        }
        cal_len();
        return str.front();
    }

    int get_char(int i) {
        int x = var-1;
        int l = 0, r = N;
        while(r-l > 1) {
            //cout << l << " " << r << endl;
            if(is_RL[x-MAX]) {
                int len = (Right_ch[x-MAX] < MAX ? 1 : length[Right_ch[x-MAX]-MAX]);
                int k = (i-l)/len;
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

    //左端のindexがiになっている変数or文字を返す
    void getPath(tuple<int,int,int,int,int> start, int i, stack<tuple<int,int,int,int,int>> &path) {
        int x = get<0>(start); //root
        int l = get<1>(start), r = get<2>(start);
        int pre_x = -1, pre_r = -1;
        while(1) {
            path.push({x,l,r,pre_x,pre_r});
            pre_x = x;
            pre_r = r;
            if(l == i) break;
            // cout << l << " " << r << endl;
            if(is_RL[x-MAX]) {
                int len = (Right_ch[x-MAX] < MAX ? 1 : length[Right_ch[x-MAX]-MAX]);
                int k = (i-l)/len;
                int prel = l;
                l = prel+k*len, r = prel+(k+1)*len;
                x = Right_ch[x-MAX];
            } else {
                int left_len = (Left_ch[x-MAX] < MAX ? 1 : length[Left_ch[x-MAX]-MAX]);
                if(i < l + left_len) r = l+left_len, x = Left_ch[x-MAX];
                else l = l+left_len, x = Right_ch[x-MAX];
            }
        }
        return;
    }

    void getPath2(tuple<int,int,int,int,int> start, int i, stack<tuple<int,int,int,int,int>> &path) {
        int x = get<0>(start); //root
        int l = get<1>(start), r = get<2>(start);
        int pre_x = -1, pre_r = -1;
        while(1) {
            path.push({x,l,r,pre_x,pre_r});
            pre_x = x;
            pre_r = r;
            if(l == i && x < MAX) break;
            // cout << l << " " << r << endl;
            if(is_RL[x-MAX]) {
                int len = (Right_ch[x-MAX] < MAX ? 1 : length[Right_ch[x-MAX]-MAX]);
                int k = (i-l)/len;
                int prel = l;
                l = prel+k*len, r = prel+(k+1)*len;
                x = Right_ch[x-MAX];
            } else {
                int left_len = (Left_ch[x-MAX] < MAX ? 1 : length[Left_ch[x-MAX]-MAX]);
                if(i < l + left_len) r = l+left_len, x = Left_ch[x-MAX];
                else l = l+left_len, x = Right_ch[x-MAX];
            }
        }
        return;
    }

    void downPath(stack<tuple<int,int,int,int,int>> &path) {
        auto n = path.top();
        int x = get<0>(n);
        int l = get<1>(n), r = get<2>(n);
        int pre_x = x, pre_r = r;
        if(is_RL[x-MAX]) {
                int len = (Right_ch[x-MAX] < MAX ? 1 : length[Right_ch[x-MAX]-MAX]);
                int prel = l;
                l = prel, r = prel+len;
                x = Right_ch[x-MAX];
            } else {
                int left_len = (Left_ch[x-MAX] < MAX ? 1 : length[Left_ch[x-MAX]-MAX]);
                r = l+left_len, x = Left_ch[x-MAX];
        }
        path.push({x,l,r,pre_x,pre_r});
    }

    //l <= pos < rとなる変数を探す．
    void upPath(int pos, stack<tuple<int,int,int,int,int>> &path) {
        int l,r;
        while(1) {
            auto n = path.top();
            l = get<1>(n), r = get<2>(n);
            if(l <= pos && pos < r) break;
            else path.pop();
        }
    }


    string getString(int i, int l) {
        stack<tuple<int,int,int,int,int>> p;
        getPath2({var-1,0,N,-1,-1},i,p);
        string ans = "";
        int cnt = 0;

        while(1) {
            auto n = p.top();
            auto v = get<0>(n);
            ans += (char)(v);
            i++;
            cnt++;
            if(cnt == l) return ans;
            upPath(i,p);
            getPath2(p.top(),i,p);
        }
    }

    int LCE(int i, int j) {
        stack<tuple<int,int,int,int,int>> p1,p2;

        getPath({var-1,0,N,-1,-1},i,p1);
        getPath({var-1,0,N,-1,-1},j,p2);

        int l = 0;

        while(1) {
            // cout << 1 << endl;
            auto n1 = p1.top();
            auto n2 = p2.top();
            auto v1 = get<0>(n1),v2 = get<0>(n2);
            // cout << v1 << " " << v2 << endl;
            // cout << l << endl;
            if(v1 == v2) {
                int pv1,pv2,r1,r2;
                pv1 = get<3>(n1), pv2 = get<3>(n2);
                r1 = get<4>(n1), r2 = get<4>(n2);
                if(is_RL[pv1-MAX] && is_RL[pv2-MAX]) {
                    l += min(r1-(i+l),r2-(j+l));
                } else {
                    if(v1 < MAX) l += 1;
                    else l += length[v1-MAX];
                }

                // if(v1 < MAX) l += 1;
                // else l += length[v1-MAX];

                if(i+l >= N || j+l >= N) break;

                upPath(i+l,p1);
                upPath(j+l,p2);

                getPath(p1.top(),i+l,p1);
                getPath(p2.top(),j+l,p2);
            } else {
                auto sz1 = (v1 < MAX ? 1 : length[v1-MAX]);
                auto sz2 = (v2 < MAX ? 1 : length[v2-MAX]);
                if(sz1 == 1 && sz2 == 1) break;
                if(sz1 > sz2) {
                    downPath(p1);
                } else if(sz1 < sz2) {
                    downPath(p2);
                } else {
                    downPath(p1);
                    downPath(p2);
                }
            }
        }

        return l;
    }

    int LCE2(int i, int j) {
        stack<tuple<int,int,int,int,int>> p1,p2;

        getPath2({var-1,0,N,-1,-1},i,p1);
        getPath2({var-1,0,N,-1,-1},j,p2);

        int l = 0;

        while(1) {
            auto n1 = p1.top();
            auto n2 = p2.top();
            auto v1 = get<0>(n1),v2 = get<0>(n2);
            // cout << v1 << " " << v2 << endl;
            // cout << l << endl;
            if(v1 == v2) {
                int pv1,pv2,r1,r2;
                pv1 = get<3>(n1), pv2 = get<3>(n2);
                r1 = get<4>(n1), r2 = get<4>(n2);
                if(is_RL[pv1-MAX] && is_RL[pv2-MAX]) {
                    l += min(r1-(i+l),r2-(j+l));
                } else {
                    if(v1 < MAX) l += 1;
                    else l += length[v1-MAX];
                }

                // if(v1 < MAX) l += 1;
                // else l += length[v1-MAX];

                if(i+l >= N || j+l >= N) break;

                upPath(i+l,p1);
                upPath(j+l,p2);

                getPath2(p1.top(),i+l,p1);
                getPath2(p2.top(),j+l,p2);
            }
        }

        return l;
    }

    vector<int> Compress() {
        vector<int> bit;
        int g = var;

        for(int i = 0; i < 64; i++) {
            if(N & (1<<i)) bit.push_back(1);
            else bit.push_back(0);
        }

        int sz = log2(N);
        for(int i = 0; i < sz; i++) {
            if((var-MAX) & (1<<i)) bit.push_back(1);
            else bit.push_back(0);
        }
        for(int i = 0; i < var-MAX; i++) {
            for(int i = 0; i < sz; i++) {
                if(Left_ch[i] & (1<<i)) bit.push_back(1);
                else bit.push_back(0);
            }
            if(is_RL[i]) bit.push_back(1);
            else bit.push_back(0);
            for(int i = 0; i < sz; i++) {
                if(Right_ch[i] & (1<<i)) bit.push_back(1);
                else bit.push_back(0);
            }
        }
        return bit;
    }

    void print_node(int x, string &ans) {
        if(x < MAX) ans += (char)x;
        else {
            if(is_RL[x-MAX]) {
                int a = Left_ch[x-MAX];
                for(int _ = 0; _ < a; _++) {
                    print_node(Right_ch[x-MAX],ans);
                }
            } else {
                print_node(Left_ch[x-MAX],ans);
                print_node(Right_ch[x-MAX],ans);
            }
        }
    }

    string RLSLPtoS() {
        string ans = "";
        print_node(var-1,ans);
        return ans;
    }
};
