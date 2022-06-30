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

struct SimTtoG {

    using Production_rule = vector<vector<int>>;

    int MAX = 256;
    int Var_SLP; //現在最大の変数
    int var_RLSLP;
    int n; //SLPの変数の数
    vector<int> RLSLP_L, RLSLP_R;
    Production_rule D; //SLPの生成規則
    vector<int> len; //規則の長さ
    vector<int> LML, RML;
    /*
        LML[i] : 規則iを根とする導出部分木の左端葉
        RML[i] : 規則iを根とする導出部分木の右端葉
    */
    vector<int> block_right, block_left; // 各変数の展開文字列の右/左からのRunLength
    unordered_map<pair<int,int>,int,HashPair> RL_par, not_RL_par;
    /*
        RL_par : 
        not_RL_par : 
    */

    /*
            コンストラクタ
            SLPの生成規則の右辺はMAX以上を想定
    */
    SimTtoG(int N, const vector<int> &L, const vector<int> &R) : n(N) {
        len.assign(n,0); 
        D.assign(n,vector<int>());
        Var_SLP = MAX;
        var_RLSLP = MAX+n;

        for(int i = 0; i < n; i++) {
            D[i].push_back(L[i]);
            D[i].push_back(R[i]);
            len[i] = D[i].size();
        }
    }

    void print_D() {
        cout << n << endl;
        for(int i = 0; i < n; i++) {
                cout << i+MAX << " LML : " << LML[i]<< " " << " RML : " << RML[i] <<  "---> ";
                for(int j = 0; j < D[i].size(); j++) cout << D[i][j] << " ";
                cout << endl;
        }
        cout << endl;
    }

    //LMLとRMLを計算する
    void cal_LML_RML() {
        LML.assign(n,-1);
        RML.assign(n,-1);
        for(int i = 0; i < n; i++) {
            if(D[i].size() == 0) continue;
            int most_left_letter = D[i].front();
            int most_right_letter = D[i].back();

            if(most_left_letter < MAX || MAX + n <= most_left_letter) {
                LML[i] = most_left_letter;
            } else {
                LML[i] = LML[most_left_letter-MAX];
            }

            if(most_right_letter < MAX || MAX + n <= most_right_letter) {
                RML[i] = most_right_letter;
            } else {
                RML[i] = RML[most_right_letter-MAX];
            }
        }
    }

    void cal_len() {
        for(int i = 0; i < n; i++) {
            for(int j = 0; j < D[i].size(); j++) {
                int now_char = D[i][j];
                if(now_char < MAX || now_char >= MAX+Var_SLP) len[i]++;
                else len[i] += len[now_char-MAX];
            }
        }
    }

    void cal_block_length() {
        block_left.assign(n,0);
        block_right.assign(n,0);

        for(int i = 0; i < n; i++) {
            int lml = LML[i];

            for(int j = 0; j < D[i].size(); j++) {
                int now_char = D[i][j];
                if(now_char < MAX || n+MAX <= now_char) {
                    if(now_char == lml) block_left[i]++;
                    else break;
                } else {
                    if(LML[now_char-MAX] == lml) {
                        block_left[i] += block_left[now_char-MAX];
                        if(block_left[now_char-MAX] != len[now_char-MAX]) break;
                    } else {
                        break;
                    }
                }
            }
        }

        for(int i = 0; i < n; i++) {
            int rml = RML[i];

            for(int j = D[i].size()-1; j >= 0; j--) {
                int now_char = D[i][j];
                if(now_char < MAX || n+MAX <= now_char) {
                    if(now_char == rml) block_right[i]++;
                    else break;
                } else {
                    if(RML[now_char-MAX] == rml) {
                        block_right[i] += block_right[now_char-MAX];
                        if(block_right[now_char-MAX] != len[now_char-MAX]) break;
                    } else {
                        break;
                    }
                }
            }
        }
    }

    void PComp() {
        auto pre_D = D;
        unordered_map<int,int> LorR; // Left : 0, Right : 1

        random_device seed;
        mt19937 mt(seed());
        uniform_int_distribution<int> rd(0,1);

        cal_LML_RML();

        //各生成規則の右辺を変換していく
        for(int i = 0; i < n; i++) {
            D[i].clear();
            for(int j = 0; j < pre_D[i].size(); j++) {

                auto x = pre_D[i][j];

                // ペアの左か右かを割り当てる
                if(x < MAX || MAX+n <= x) {
                    if(!LorR.count(x)) {
                        LorR[x] = rd(mt);
                        cout << x << " " << "LorR " << LorR[x] << endl;
                    }
                }

                if(j == 0) {
                    //PoPoutLet
                    if(LorR.count(x) && LorR[x] == 1) {
                        len[i]--;
                        continue;
                    } else D[i].push_back(x);

                } else if(j == pre_D[i].size()-1) {
                    //PoPoutLet
                    if(LorR.count(x) && LorR[x] == 0) {
                        len[i]--;
                        continue;
                    } else D[i].push_back(x);

                } else {
                    if(x < MAX || MAX+n <= x) {
                        if(D[i].size() == 0) D[i].push_back(x);
                        else {
                            if(!LorR.count(D[i].back()) || !LorR.count(x)) D[i].push_back(x);
                            else {
                                if(LorR[D[i].back()] == 0 && LorR[x] == 1) {
                                    int db = D[i].back();
                                    D[i].pop_back();
                                    if(not_RL_par.count({db,x})) D[i].push_back(not_RL_par[{db,x}]);
                                    else {
                                        D[i].push_back(var_RLSLP);
                                        not_RL_par[{db,x}] = var_RLSLP;
                                        var_RLSLP++;
                                    }
                                }
                            }
                        }
                    } else {
                        int lml = LML[x-MAX];
                        int rml = RML[x-MAX];

                        if(LorR[lml] == 1) {
                            if(D[i].size() == 0) D[i].push_back(lml);
                            else {
                                int db = D[i].back();
                                if(!LorR.count(db) || !LorR.count(lml)) D[i].push_back(lml);
                                else {
                                    if(LorR[db] == 0 && LorR[lml] == 1) {
                                        D[i].pop_back();
                                        if(not_RL_par.count({db,lml})) D[i].push_back(not_RL_par[{db,lml}]);
                                        else {
                                            D[i].push_back(var_RLSLP);
                                            not_RL_par[{db,lml}] = var_RLSLP;
                                            RLSLP_L.push_back(db);
                                            RLSLP_R.push_back(lml);
                                            var_RLSLP++;
                                        }
                                    }
                                }
                            }
                        }

                        D[i].push_back(x);

                        if(LorR[rml] == 0) D[i].push_back(rml);  
                    }
                }
            }
        }
        print_D();
    }

    void BComp() {
        cal_len();
        cal_LML_RML();
        cal_block_length();

        for(int i = 0; i < n; i++) {
            auto pre_Di = D[i];
            D[i].clear();
            for(int j = 0; j < pre_Di.size(); j++) {
                int now_char = pre_Di[j];
                if(now_char < MAX || MAX+n <= now_char) D[i].push_back(now_char);
                else {
                    if(block_left[now_char-MAX] != len[now_char-MAX]) {
                        for(int _ = 0; _ < block_left[now_char-MAX]; _++) {
                            D[i].push_back(LML[now_char-MAX]);
                        }
                        D[i].push_back(now_char);
                        for(int _ = 0; _ < block_right[now_char-MAX]; _++) {
                            D[i].push_back(RML[now_char-MAX]);
                        }
                    } else {
                        for(int _ = 0; _ < block_left[now_char-MAX]; _++) {
                            D[i].push_back(LML[now_char-MAX]);
                        }
                    }
                }
            }

            pre_Di = D[i];
            D[i].clear();

            for(int j = 0; j < pre_Di.size();) {
                int now_char = pre_Di[j];
                int cnt = 1;
                if(MAX <= now_char && now_char < MAX+n) {
                    D[i].push_back(now_char);
                    j++;
                    continue;
                }
                while(j+cnt < pre_Di.size() && now_char == pre_Di[j+cnt]) {
                    cnt++;
                }
                if(RL_par.count({cnt,now_char})) D[i].push_back(not_RL_par[{cnt,now_char}]);
                else {
                    D[i].push_back(var_RLSLP);
                    RL_par[{cnt,now_char}] = var_RLSLP;
                    RLSLP_L.push_back(cnt);
                    RLSLP_R.push_back(now_char);
                    var_RLSLP++;
                }
            }
        }
        print_D();
    }

    void ReComp() {
        int cnt = 0;
        while(cnt < 10) {
            BComp();
            PComp();
            cnt++;
        }
    }
};