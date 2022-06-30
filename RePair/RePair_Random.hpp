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

struct RePair {
    int n; //規則の数
    vector<int> L, R;
    vector<int> str;
    unordered_map<pair<int,int>,int,HashPair> parent;
    const int Alphabet_Size = 256;

    RePair(string S_) {
        n = Alphabet_Size;
        for(int i = 0; i < S_.size(); i++) {
            str.push_back((int)S_[i]);
        }
    }

    void make_pair() {
        auto pre_str = str;
        str.clear();

        random_device seed;
        mt19937 mt(seed());
        uniform_int_distribution<int> rd(0,pre_str.size()-2);

        int pos = rd(mt);
        pair<int,int> now_pair = {pre_str[pos], pre_str[pos+1]};

        L.push_back(now_pair.first);
        R.push_back(now_pair.second);

        for(int i = 0; i < pre_str.size(); i++) {
            int now_char = pre_str[i];
            if(str.size() == 0) str.push_back(now_char);
            else {
                int pre_char = str.back();
                if(now_pair == pair<int,int>({pre_char,now_char})) {
                    str.pop_back();
                    str.push_back(n);
                } else {
                    str.push_back(now_char);
                }
            }
        }

        n++;
    }

    void Comp() {
        while(str.size() > 1) {
            for(auto s : str) cout << s << " ";
            cout << endl;
            make_pair();
        }

        for(auto s : str) cout << s << " ";
        cout << endl;

        cout << "生成規則" << endl;
        for(int i = 0; i < n-Alphabet_Size; i++) cout << i+Alphabet_Size << " ---> "<< L[i] << " " << R[i] << endl;
    }
};