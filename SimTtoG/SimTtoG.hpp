#include <iostream>
#include <vector>
#include <algorithm>
#include<unordered_map>
#include <random>
#include "HashPair.hpp"

struct SimTtoG {

    using Production_rule = vector<vector<int>>;

    int MAX = 256; //アルファベットの最大値+1
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
    SimTtoG(int N, const vector<int> &L, const vector<int> &R, int _MAX) : n(N) {
        len.assign(n,0); 
        D.assign(n,vector<int>());
        MAX = _MAX;
        var_RLSLP = MAX+n;
        LML.assign(n,-1);
        RML.assign(n,-1);

        for(int i = 0; i < n; i++) {
            D[i].push_back(L[i]);
            D[i].push_back(R[i]);
            len[i] = D[i].size();
        }
    }

    void print_D() {
        cal_len(D);
        cal_LML_RML(D);
        cal_block_length(D);
        cout << n << endl;
        for(int i = 0; i < n; i++) {
                cout << i+MAX << " 展開長 " << len[i]  << " " << "ブロック長 : " << block_left[i] << " " << block_right[i] << " (LML : " << LML[i]<< " " << " RML : " << RML[i] <<  ") ---> ";
                for(int j = 0; j < D[i].size(); j++) cout << D[i][j] << " ";
                cout << endl;
        }
        cout << endl;
    }

    //LMLとRMLを計算する
    void cal_LML_RML(Production_rule &D) {
        for(int i = 0; i < n; i++) {
            if(D[i].size() == 0) {
                LML[i] = RML[i] = -1;
                continue;
            }
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

    void cal_len(Production_rule &D) {
        for(int i = 0; i < n; i++) {
            len[i] = 0;
            for(int j = 0; j < D[i].size(); j++) {
                int now_char = D[i][j];
                if(now_char < MAX || now_char >= MAX+n) len[i]++;
                else len[i] += len[now_char-MAX];
            }
        }
    }

    void cal_block_length(Production_rule &D) {
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

    void assign_LorR(unordered_map<int,int> &LorR, Production_rule &D) {
        random_device seed;
        mt19937 mt(seed());
        uniform_int_distribution<int> rd(0,1);

        for(int i = 0; i < n; i++) {
            for(int j = 0; j < D[i].size(); j++) {
                int x = D[i][j];

                if(x < MAX || MAX+n <= x) {
                    if(!LorR.count(x)) {
                        LorR[x] = rd(mt);
                    }
                }
            }
        }
    }

    void PopInLet(Production_rule &D, Production_rule &pre_D, unordered_map<int,int> &LorR) {
        for(int i = 0; i < n; i++) {
            for(int j = 0; j < pre_D[i].size(); j++) {
                int x = pre_D[i][j];

                if(x < MAX || MAX+n <= x) {
                    D[i].push_back(x);
                } else {
                    int lml = LML[x-MAX];
                    int rml = RML[x-MAX];

                    if(LorR[lml] == 1) {
                        D[i].push_back(lml);
                        len[x-MAX]--;
                    }

                    if(LorR[rml] == 0) {
                        len[x-MAX]--;
                    }

                    if(len[x-MAX] != 0) D[i].push_back(x);

                    if(LorR[rml] == 0) {
                        D[i].push_back(rml);
                    }
                }
            }
        }
    }

    void PopOutLet(Production_rule &D, unordered_map<int,int> &LorR) {
        for(int i = 0; i < n; i++) {
            if(i < n-1) {
                if(D[i].size() > 0) {
                    if(LorR.count(D[i].front()) && LorR[D[i].front()] == 1) {
                        cout << i+MAX << "から" << *D[i].begin() << "を消す" << endl; 
                        D[i].erase(D[i].begin());
                    }
                }
                if(D[i].size() > 0) {
                    if(LorR.count(D[i].back()) && LorR[D[i].back()] == 0) {
                    cout << i+MAX << "から" << D[i].back() << "を消す" << endl; 
                        D[i].pop_back();
                    }
                }
            }
        }
    }

    void PComp() {
        auto pre_D = D;
        unordered_map<int,int> LorR; // Left : 0, Right : 1

        cal_LML_RML(pre_D);

        D = vector<vector<int>>(n);

        assign_LorR(LorR, pre_D);
        PopInLet(D,pre_D,LorR);
        PopOutLet(D,LorR);

        //各生成規則の右辺を変換していく
        for(int i = 0; i < n; i++) {
            auto pre_Di = D[i];
            D[i].clear();

            for(int j = 0; j < pre_Di.size();) {
                if(j < pre_Di.size()-1) {
                    int a = pre_Di[j];
                    int b = pre_Di[j+1];
                    if((a < MAX || n+MAX <= a) && (b < MAX || n+MAX <= b)) {
                        if(LorR[a] == 0 && LorR[b] == 1) {
                            if(not_RL_par.count({a,b})) D[i].push_back(not_RL_par[{a,b}]);
                            else {
                                D[i].push_back(var_RLSLP);
                                not_RL_par[{a,b}] = var_RLSLP;
                                RLSLP_L.push_back(a);
                                RLSLP_R.push_back(b);
                                var_RLSLP++;
                            }

                            j += 2;

                        } else {
                            D[i].push_back(a);
                            j++;
                        }
                    } else {
                        D[i].push_back(a);
                        j++;
                    }
                } else {
                    D[i].push_back(pre_Di[j]);
                    j++;
                }
            }
        }
    }

    void BComp() {
        cal_len(D);
        cal_LML_RML(D);
        cal_block_length(D);

        for(int i = 0; i < n; i++) {
            auto pre = D[i];
            D[i].clear();
            for(int j = 0; j < pre.size(); j++) {
                int x = pre[j];
                if(x < MAX || MAX+n <= x) {
                    D[i].emplace_back(x);
                } else {
                    int lml = LML[x-MAX];
                    int rml = RML[x-MAX];
                    for(int _ = 0; _ < block_left[x-MAX]; _++) {
                        D[i].emplace_back(lml);
                    }
                    if(len[x-MAX] == block_left[x-MAX]) continue;
                    else {
                        if(len[x-MAX] != block_left[x-MAX] + block_right[x-MAX]) {
                            D[i].emplace_back(x);
                        }
                        for(int _ = 0; _ < block_right[x-MAX]; _++) {
                            D[i].emplace_back(rml);
                        }
                    }
                }
            }

            if(i < n-1) {
                int lml_i = LML[i];
                int rml_i = RML[i];

                if(D[i].size() == 0) continue;
                while(D[i].back() == rml_i) D[i].pop_back();

                if(D[i].size() == 0) continue;

                reverse(D[i].begin(),D[i].end());
                while(D[i].back() == lml_i) D[i].pop_back(); 
                reverse(D[i].begin(),D[i].end());
            }
        }

        for(int i = 0; i < n; i++) {
            auto pre_Di = D[i];
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
                if(cnt == 1) {
                    D[i].push_back(now_char);
                    j++;
                    continue;
                }
                if(RL_par.count({cnt,now_char})) D[i].push_back(RL_par[{cnt,now_char}]);
                else {
                    D[i].push_back(var_RLSLP);
                    RL_par[{cnt,now_char}] = var_RLSLP;
                    RLSLP_L.push_back(cnt);
                    RLSLP_R.push_back(now_char);
                    var_RLSLP++;
                }
                j += cnt;
            }
        }
    }

    void ReComp() {
        while(1) {
            cout << "Bcomp前" << endl;
            print_D();
            BComp();
            cout << "Bcomp後" << endl;
            print_D();
            cout << "Pcomp前" << endl;
            print_D();
            PComp();
            cout << "Pcomp後" << endl;
            print_D();

            bool fin = true;
            for(int i = 0; i < n-1; i++) if(D[i].size() != 0) fin = false;
            if(fin && D[n-1].size() == 1) break;
        }
        cout << "---RLSLP---" << endl;
        cout << RLSLP_L.size() << endl;
        for(int i = 0; i < RLSLP_L.size(); i++) {
            if(RLSLP_L[i] >= MAX+n) RLSLP_L[i] -= n;
            if(RLSLP_R[i] >= MAX+n) RLSLP_R[i] -= n;
            cout << MAX+i << " : " << RLSLP_L[i] << " " << RLSLP_R[i] << endl;
        }
    }
};