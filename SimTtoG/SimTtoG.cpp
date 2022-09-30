#include <cstdio>
#include <string>
#include <fstream>
#include <chrono>
#include <algorithm>
#include <deque>
#include "cmdline.h"
#include "SimTtoG.hpp"
#define DEBUG
const int MAX = 256;

using namespace std;

int main(int argc, char *argv[]) {
    cmdline::parser p;
    p.add<string>("input_r", 'r', "input r file name",true);
    p.add<string>("input_c", 'c', "input c file name",true);

    p.parse_check(argc,argv);
    const string in_r = p.get<string>("input_r");
    const string in_c = p.get<string>("input_c");

    std::ifstream instream_r(in_r);
    std::ifstream instream_c(in_c);

    const uint64_t bytesize_r = instream_r.seekg(0, std::ios::end).tellg();
    instream_r.clear();
    instream_r.seekg(0,std::ios::beg);

    const uint64_t bytesize_c = instream_c.seekg(0, std::ios::end).tellg();
    instream_c.clear();
    instream_c.seekg(0,std::ios::beg);

    #ifdef DEBUG
    cout << ".C bytesize is " << bytesize_c << endl;
    cout << ".R bytesize is " << bytesize_r << endl;

    cout << "---.R---" << endl;
    #endif //DEBUG

    vector<int> SLP_L, SLP_R;
    int alph_size = 0;

    for(int i = 0; i < 4; i++) {
        char n;
        instream_r.read((char * ) &n, sizeof(char));
        alph_size += (unsigned char)(n) * (1<<(4*i));
    }

    cout << alph_size << endl;

    int SLP_n = alph_size;

    vector<char> alph;

    for(int i = 0; i < alph_size; i++) {
        char c;
        instream_r.read((char *)&c, sizeof(c));
        alph.push_back(c);
    }

    for(uint64_t i = 4 + alph_size; i < bytesize_r; i += 8) {
        int tot = 0;
        for(int j = 0; j < 4; ++j) {
            char n;
            instream_r.read((char *)&n, sizeof(n));
            tot += (unsigned char)n * (1<<(4*j));
        }
        #ifdef DEBUG
        cout << tot << " ";
        #endif //DEBUG
        SLP_L.push_back(tot);

        tot = 0;
        for(int j = 0; j < 4; j++) {
            char n;
            instream_r.read((char *)&n, sizeof(n));
            tot += (unsigned char)n * (1<<(4*j));
        }
        SLP_R.push_back(tot);
        #ifdef DEBUG
        cout << tot << endl;
        #endif //DEBUG

        SLP_n++;
    }

    #ifdef DEBUG

    cout << "---.C---" << endl;

    #endif //DEBUG

    deque<int> Clist;

    for(uint64_t i = 0; i < bytesize_c; i += 4) {
        int tot = 0;
        for(int j = 0; j < 4; j++) {
            char n;
            instream_c.read((char *)&n, sizeof(n));
            tot += (unsigned char)n * (1<<(4*j));
        }
        #ifdef DEBUG
        cout << tot << " ";
        #endif //DEBUG
        Clist.push_back(tot);
    }

    #ifdef DEBUG
    cout << endl << endl;
    #endif //DEBUG

    while(Clist.size() > 1) {
        auto a = Clist.front();
        Clist.pop_front();
        auto b = Clist.front();
        Clist.pop_front();

        SLP_L.push_back(a);
        SLP_R.push_back(b);

        Clist.push_front(SLP_n);
        SLP_n++;
    }

    for(int i = 0; i < SLP_L.size(); i++) {
        if(SLP_L[i] < alph_size) SLP_L[i] = (int)(alph[SLP_L[i]]);
        else SLP_L[i] = SLP_L[i] - alph_size + MAX;

        if(SLP_R[i] < alph_size) SLP_R[i] = (int)(alph[SLP_R[i]]);
        else SLP_R[i] = SLP_R[i] - alph_size + MAX;
    }

    #ifdef DEBUG
    for(int i = 0; i < SLP_L.size(); i++) cout << SLP_L[i] << " " << SLP_R[i] << endl;
    #endif //DEBUG

    SimTtoG G(SLP_L.size(), SLP_L, SLP_R, MAX);
    G.ReComp();
}