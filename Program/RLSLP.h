#include<iostream>
#include<string>
#include<unordered_map>
#include<vector>
#include<set>

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

    //strをPairCompする O(|str|^2)
    // void PairComp() {
    //     vector<ull> L,R;
    //     unordered_map<ull,char> who;
    //     unordered_map<ull,ull> seen;
    //     unordered_map<pair<ull,ull>,ull,HashPair> freq;
    //     int N = pre_str.size();
    //     //bigramの頻度を計算
    //     for(int i = 0; i < N-1; i++) {
    //         freq[{pre_str[i],pre_str[i+1]}]++;
    //     }
    //     //各文字に'L'か'R'を割り当てる
    //     for(int i = 0; i < N; i++) {
    //         if(seen[pre_str[i]]) continue;
    //         ull a = pre_str[i];
    //         ull cnt_L = 0, cnt_R = 0;
    //         for(auto l : L) cnt_L += freq[{l,a}] + freq[{a,l}];
    //         for(auto r : R) cnt_R += freq[{r,a}] + freq[{a,r}];
    //         if(cnt_L <= cnt_R) L.emplace_back(a), who[a] = 'L';
    //         else R.emplace_back(a), who[a] = 'R';
    //         seen[pre_str[i]] = 1;
    //     }
    //     //L,Rのペアがあればまとめる
    //     for(int i = 0; i < N;) {
    //         if(i < N-1 && who[pre_str[i]] == 'L' && who[pre_str[i+1]] == 'R') {
    //             ull a = pre_str[i];
    //             ull b = pre_str[i+1];
    //             ull par;
    //             if(get_not_RL_par(a,b,par)) {
    //                 str.emplace_back(par);
    //             } else {
    //                 not_RL_par[{a,b}] = var;
    //                 str.emplace_back(var);
    //                 is_RL.emplace_back(0);
    //                 Left_ch.emplace_back(a), Right_ch.emplace_back(b);
    //                 var++;
    //             }
    //             i += 2;
    //         } else {
    //             str.emplace_back(pre_str[i]);
    //             i++;
    //         }
    //     }
    // }

    //strをPairCompする O(|w|log|w|)
    void PairComp() {
        vector<pair<ull,ull>> edge;
        int N = pre_str.size();
        for(int i = 0; i < N-1; i++) {
            edge.push_back({pre_str[i],pre_str[i+1]});
            edge.push_back({pre_str[i+1],pre_str[i]});
        }
        sort(edge.begin(),edge.end());
        long long cur = edge[0].first;
        unordered_map<ull,char> who;

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
            pre_str = str;
            str = vector<ull>();
            BlockComp();
            // if(pre_str != str) {
            //     cout << "BlockComp : ";
            //     print_str();
            // }
            pre_str = str;
            str = vector<ull>();
            PairComp();
            //cout << "PairComp : ";
            // if(pre_str != str) print_str();
            // else break;
        }
        return str.front();
    }

    ull get(int i) {
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

    // ull LCE(int i, int j) {
    //     ull x = var-1;

    // }

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