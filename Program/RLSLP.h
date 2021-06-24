#include<iostream>
#include<string>
#include<unordered_map>
#include<vector>


using namespace std;
using uint = unsigned int;
using ull = unsigned long long;
const int MAX = 256;

void hash_combine(ull & seed, ull const& v) {
  seed ^= hash<ull>()(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}

ull hash_pair(ull l, ull r) {
    ull seed = 0;
    hash_combine(seed,l);
    hash_combine(seed,r);
    return seed;
}


struct RLSLP{
    ull var; //変数
    vector<ull> Left_ch, Right_ch;//左の子，右の子
    unordered_map<ull,ull> RL_par, not_RL_par; //親へのハッシュ
    vector<ull> is_RL; //ある変数の子が連長圧縮かどうか
    vector<ull> str, pre_str;
    vector<ull> length;

    RLSLP(const string S) {
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
        ull seed = hash_pair(l,r);
        if(RL_par.count(seed)) { par = RL_par[seed]; return true;}
        else return false;
    }

    bool get_not_RL_par(ull l, ull r, ull &par) {
        ull seed = hash_pair(l,r);
        if(not_RL_par.count(seed)) { par = not_RL_par[seed]; return true;}
        else return false;
    }

    //strをBlockCompする
    void BlockComp() {
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
                    RL_par[hash_pair(len,val)] = var;
                    is_RL.emplace_back(1);
                    str.emplace_back(var);
                    Left_ch.emplace_back(len), Right_ch.emplace_back(val);
                    var++;
                }
            }
        }
    }

    //strをPairCompする
    void PairComp() {
        vector<ull> L,R;
        unordered_map<ull,char> who;
        unordered_map<ull,ull> seen;
        unordered_map<ull,ull> freq;
        int N = pre_str.size();
        //bigramの頻度を計算
        for(int i = 0; i < N-1; i++) {
            freq[hash_pair(pre_str[i],pre_str[i+1])]++;
        }
        //各文字に'L'か'R'を割り当てる
        for(int i = 0; i < N; i++) {
            if(seen[pre_str[i]]) continue;
            ull a = pre_str[i];
            ull cnt_L = 0, cnt_R = 0;
            for(auto l : L) cnt_L += freq[hash_pair(l,a)] + freq[hash_pair(a,l)];
            for(auto r : R) cnt_R += freq[hash_pair(r,a)] + freq[hash_pair(a,r)];
            if(cnt_L <= cnt_R) L.emplace_back(a), who[a] = 'L';
            else R.emplace_back(a), who[a] = 'R';
            seen[pre_str[i]] = 1;
        }
        //L,Rのペアがあればまとめる
        for(int i = 0; i < N;) {
            if(i < N-1 && who[pre_str[i]] == 'L' && who[pre_str[i+1]] == 'R') {
                ull a = pre_str[i];
                ull b = pre_str[i+1];
                ull par;
                if(get_not_RL_par(a,b,par)) {
                    str.emplace_back(par);
                } else {
                    not_RL_par[hash_pair(a,b)] = var;
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
        while(str.size() > 1) {
            pre_str = str;
            str = vector<ull>();
            BlockComp();
            if(pre_str != str) print_str();
            pre_str = str;
            str = vector<ull>();
            PairComp();
            print_str();
        }
        return str.front();
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

    // //srt[i]を取得
    // ull get(int i) {

    // }
};