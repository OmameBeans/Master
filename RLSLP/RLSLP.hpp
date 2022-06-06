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
using uint = unsigned int;
using ull = unsigned long long;
const int MAX = 256;

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
    ull var; //変数
    ull N; //|S|
    vector<ull> Left_ch, Right_ch;//左の子，右の子
    unordered_map<pair<ull,ull>,ull,HashPair> RL_par, not_RL_par; //親へのハッシュ
    vector<ull> is_RL; //ある変数の子が連長圧縮かどうか
    vector<ull> str, pre_str;
    vector<ull> length;

    RLSLP(const string S) : N(S.size()){
        var = 256;
        for(auto c : S) str.emplace_back((ull)c);
    }

    void print_str() {
        for(auto c : str) cout << c << " ";
        cout << endl;
    }

    void print_rules() {
        cout << "規則集合 X -> A B の形式" << endl;
        for(int i = 0; i < var-256; i++) {
            cout << i+256 << " -> " << Left_ch[i] << " " << Right_ch[i] << " 長さは" << length[i]<< endl;
        }
    }

    bool get_RL_par(ull l, ull r, ull &par) {
        if(RL_par.count({l,r})) { par = RL_par[{l,r}]; return true;}
        else return false;
    }

    bool get_not_RL_par(ull l, ull r, ull &par) {
        if(not_RL_par.count({l,r})) { par = not_RL_par[{l,r}]; return true;}
        else return false;
    }

    //strをBlockCompする O(|str|)
    void BlockComp() {
        //pre_str:変換前の文字列
        vector<pair<ull,ull>> block;
        pre_str = str;
        str = vector<ull>();
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
                ull par;
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
        str = vector<ull>();
        vector<pair<int,int>> edge;
        int N = pre_str.size();

        for(int i = 0; i < N-1; i++) {
            edge.push_back({pre_str[i],pre_str[i+1]});
            edge.push_back({pre_str[i+1],pre_str[i]});
        }
        Pair_Radix_Sort(edge,edge.size(),var+10);
        long long cur = edge[0].first;

        // int l = 0, r = 0;
        unordered_map<ull,char> who;
        // while(abs(r-l) > 1 || l == 0 || r == 0) {
        //     who.clear();
        //     mt19937 mt{random_device{}()};
        //     uniform_int_distribution<int> dist(0, 1);
        //     for(auto c : pre_str) {
        //         if(who[c]) continue;
        //         else {
        //             if(dist(mt) == 0) who[c] = 'L', l++;
        //             else who[c] = 'R', r++;
        //         }
        //     }
        // }

        int left_cnt = 0, right_cnt = 0;
        for(int i = 0; i < 2*(N-1);) {
            if(cur == edge[i].first) {
                ull b = edge[i].second;
                if(who[b] == 'L') left_cnt++;
                else if(who[b] == 'R') right_cnt++;
                i++;
            } else {
                if(left_cnt <= right_cnt) {
                    who[cur] = 'L';
                } else who[cur] = 'R';
                cur = edge[i].first;
                left_cnt = right_cnt = 0;
            }
        }
        if(left_cnt <= right_cnt) {
            who[cur] = 'L';
        } else who[cur] = 'R';

        // cout << "            ";
        // for(int i = 0; i < N; i++) cout << who[pre_str[i]] << " ";
        // cout << endl;

        int LR = 0, RL = 0;
        for(int i = 0; i < N-1; i++) {
            if(who[pre_str[i]] == 'L' && who[pre_str[i+1]] == 'R') LR++;
            else if(who[pre_str[i]] == 'R' && who[pre_str[i+1]] == 'L') RL++;
        }

        bool rev;
        rev = (LR < RL);
        if(rev) swap(LR,RL);

        //cout << "元の文字列から" << (double)(pre_str.size()-LR)/pre_str.size() << "小さくなりました" << endl;

        //L,Rのペアがあればまとめる
        for(int i = 0; i < N;) {
            if(i < N-1 && who[pre_str[i]] == (rev ? 'R' : 'L') && who[pre_str[i+1]] == (rev ? 'L' : 'R')) {
                ull a = pre_str[i];
                ull b = pre_str[i+1];
                ull par;
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

    //SをRLSLPに変換して，開始記号を返す．
    ull StoRLSLP() {
        // cout << "入力文字列";
        // print_str();
        while(str.size() > 1) {
            BlockComp();
            if(pre_str != str) {
                //cout << "BlockComp : ";
                //print_str();
                // cout << str.size() << endl;
            }
            if(str.size() < 2) break;
            PairComp();
            //cout << "PairComp : ";
            //if(pre_str != str) print_str();
            //else break;
            // cout << str.size() << endl;
        }
        return str.front();
    }

    ull get_char(int i) {
        ull x = var-1;
        ull l = 0, r = N;
        while(r-l > 1) {
            //cout << l << " " << r << endl;
            if(is_RL[x-256]) {
                ull len = (Right_ch[x-256] < 256 ? 1 : length[Right_ch[x-256]-256]);
                ull k = (i-l)/len;
                ull prel = l;
                l = prel+k*len, r = prel+(k+1)*len;
                x = Right_ch[x-256];
            } else {
                ull left_len = (Left_ch[x-256] < 256 ? 1 : length[Left_ch[x-256]-256]);
                if(i < l + left_len) r = l+left_len, x = Left_ch[x-256];
                else l = l+left_len, x = Right_ch[x-256];
            }
        }
        return x;
    }

    //左端のindexがiになっている変数or文字を返す
    void getPath(tuple<ull,int,int,ull,int> start, int i, stack<tuple<ull,int,int,ull,int>> &path) {
        ull x = get<0>(start); //root
        ull l = get<1>(start), r = get<2>(start);
        ull pre_x = -1, pre_r = -1;
        while(1) {
            path.push({x,l,r,pre_x,pre_r});
            pre_x = x;
            pre_r = r;
            if(l == i) break;
            // cout << l << " " << r << endl;
            if(is_RL[x-256]) {
                ull len = (Right_ch[x-256] < 256 ? 1 : length[Right_ch[x-256]-256]);
                ull k = (i-l)/len;
                ull prel = l;
                l = prel+k*len, r = prel+(k+1)*len;
                x = Right_ch[x-256];
            } else {
                ull left_len = (Left_ch[x-256] < 256 ? 1 : length[Left_ch[x-256]-256]);
                if(i < l + left_len) r = l+left_len, x = Left_ch[x-256];
                else l = l+left_len, x = Right_ch[x-256];
            }
        }
        return;
    }

    void getPath2(tuple<ull,int,int,ull,int> start, int i, stack<tuple<ull,int,int,ull,int>> &path) {
        ull x = get<0>(start); //root
        ull l = get<1>(start), r = get<2>(start);
        ull pre_x = -1, pre_r = -1;
        while(1) {
            path.push({x,l,r,pre_x,pre_r});
            pre_x = x;
            pre_r = r;
            if(l == i && x < 256) break;
            // cout << l << " " << r << endl;
            if(is_RL[x-256]) {
                ull len = (Right_ch[x-256] < 256 ? 1 : length[Right_ch[x-256]-256]);
                ull k = (i-l)/len;
                ull prel = l;
                l = prel+k*len, r = prel+(k+1)*len;
                x = Right_ch[x-256];
            } else {
                ull left_len = (Left_ch[x-256] < 256 ? 1 : length[Left_ch[x-256]-256]);
                if(i < l + left_len) r = l+left_len, x = Left_ch[x-256];
                else l = l+left_len, x = Right_ch[x-256];
            }
        }
        return;
    }

    void downPath(stack<tuple<ull,int,int,ull,int>> &path) {
        auto n = path.top();
        ull x = get<0>(n);
        int l = get<1>(n), r = get<2>(n);
        ull pre_x = x, pre_r = r;
        if(is_RL[x-256]) {
                ull len = (Right_ch[x-256] < 256 ? 1 : length[Right_ch[x-256]-256]);
                ull prel = l;
                l = prel, r = prel+len;
                x = Right_ch[x-256];
            } else {
                ull left_len = (Left_ch[x-256] < 256 ? 1 : length[Left_ch[x-256]-256]);
                r = l+left_len, x = Left_ch[x-256];
        }
        path.push({x,l,r,pre_x,pre_r});
    }

    //l <= pos < rとなる変数を探す．
    void upPath(int pos, stack<tuple<ull,int,int,ull,int>> &path) {
        int l,r;
        while(1) {
            auto n = path.top();
            l = get<1>(n), r = get<2>(n);
            if(l <= pos && pos < r) break;
            else path.pop();
        }
    }

    // bool parent_is_RL(stack<tuple<ull,int,int>> path, ull &r) {
    //     if(path.size() == 1) return 0;
    //     path.pop();
    //     auto n = path.top();
    //     auto v = get<0>(n);
    //     auto ri = get<2>(n);
    //     if(v < 256) return 0;
    //     else {
    //         r = ri;
    //         return is_RL[v-256];
    //     }
    // }

    string getString(int i, int l) {
        stack<tuple<ull,int,int,ull,int>> p;
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

    ull LCE(int i, int j) {
        stack<tuple<ull,int,int,ull,int>> p1,p2;

        getPath({var-1,0,N,-1,-1},i,p1);
        getPath({var-1,0,N,-1,-1},j,p2);

        ull l = 0;

        while(1) {
            // cout << 1 << endl;
            auto n1 = p1.top();
            auto n2 = p2.top();
            auto v1 = get<0>(n1),v2 = get<0>(n2);
            // cout << v1 << " " << v2 << endl;
            // cout << l << endl;
            if(v1 == v2) {
                ull pv1,pv2,r1,r2;
                pv1 = get<3>(n1), pv2 = get<3>(n2);
                r1 = get<4>(n1), r2 = get<4>(n2);
                if(is_RL[pv1-256] && is_RL[pv2-256]) {
                    l += min(r1-(i+l),r2-(j+l));
                } else {
                    if(v1 < 256) l += 1;
                    else l += length[v1-256];
                }

                // if(v1 < 256) l += 1;
                // else l += length[v1-256];

                if(i+l >= N || j+l >= N) break;

                upPath(i+l,p1);
                upPath(j+l,p2);

                getPath(p1.top(),i+l,p1);
                getPath(p2.top(),j+l,p2);
            } else {
                auto sz1 = (v1 < 256 ? 1 : length[v1-256]);
                auto sz2 = (v2 < 256 ? 1 : length[v2-256]);
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

    ull LCE2(int i, int j) {
        stack<tuple<ull,int,int,ull,int>> p1,p2;

        getPath2({var-1,0,N,-1,-1},i,p1);
        getPath2({var-1,0,N,-1,-1},j,p2);

        ull l = 0;

        while(1) {
            auto n1 = p1.top();
            auto n2 = p2.top();
            auto v1 = get<0>(n1),v2 = get<0>(n2);
            // cout << v1 << " " << v2 << endl;
            // cout << l << endl;
            if(v1 == v2) {
                ull pv1,pv2,r1,r2;
                pv1 = get<3>(n1), pv2 = get<3>(n2);
                r1 = get<4>(n1), r2 = get<4>(n2);
                if(is_RL[pv1-256] && is_RL[pv2-256]) {
                    l += min(r1-(i+l),r2-(j+l));
                } else {
                    if(v1 < 256) l += 1;
                    else l += length[v1-256];
                }

                // if(v1 < 256) l += 1;
                // else l += length[v1-256];

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
        ull g = var;

        for(int i = 0; i < 64; i++) {
            if(N & (1<<i)) bit.push_back(1);
            else bit.push_back(0);
        }

        int sz = log2(N);
        for(int i = 0; i < sz; i++) {
            if((var-256) & (1<<i)) bit.push_back(1);
            else bit.push_back(0);
        }
        for(int i = 0; i < var-256; i++) {
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

    void print_node(ull x, string &ans) {
        if(x < 256) ans += (char)x;
        else {
            if(is_RL[x-256]) {
                ull a = Left_ch[x-256];
                for(int _ = 0; _ < a; _++) {
                    print_node(Right_ch[x-256],ans);
                }
            } else {
                print_node(Left_ch[x-256],ans);
                print_node(Right_ch[x-256],ans);
            }
        }
    }

    string RLSLPtoS() {
        string ans = "";
        print_node(var-1,ans);
        return ans;
    }

    void cal_len() {
        length = vector<ull>(var);
        for(int i = 0; i < var-256; i++) {
            ull a = Left_ch[i], b = Right_ch[i];
            if(is_RL[i]) {
                if(b < MAX) length[i] += a; else length[i] += a*length[b-MAX];
            } else {
                if(a < MAX) length[i] += 1; else length[i] += length[a-MAX];
                if(b < MAX) length[i] += 1; else length[i] += length[b-MAX];
            }
        }
    }
};
