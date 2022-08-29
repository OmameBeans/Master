#include <cstdio>
#include <string>
#include <fstream>
#include <chrono>
#include <algorithm>
#include <deque>
#include "cmdline.h"
#include "SimTtoG.hpp"

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

    cout << ".C bytesize is " << bytesize_c << endl;
    cout << ".R bytesize is " << bytesize_r << endl;

    vector<int> SLP_L, SLP_R;
    int SLP_n = 256;

    cout << "---.R---" << endl;

    for(uint64_t i = 0; i < bytesize_r; i += 8) {
        int tot = 0;
        for(int j = 3; j >= 0; --j) {
            char n;
            instream_r.read((char *)&n, sizeof(n));
            tot += (int)n * (1<<(4*j));
        }
        cout << tot << " ";
        SLP_L.push_back(tot);

        tot = 0;
        for(int j = 3; j >= 0; --j) {
            char n;
            instream_r.read((char *)&n, sizeof(n));
            tot += (int)n * (1<<(4*j));
        }
        SLP_R.push_back(tot);
        cout << tot << endl;

        SLP_n++;
    }

    cout << "---.C---" << endl;

    deque<int> Clist;

    for(uint64_t i = 0; i < bytesize_c; i += 4) {
        int tot = 0;
        for(int j = 3; j >= 0; --j) {
            char n;
            instream_c.read((char *)&n, sizeof(n));
            tot += (int)n * (1<<(4*j));
        }
        cout << tot << endl;
        Clist.push_back(tot);
    }

    cout << endl;

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

    for(int i = 0; i < SLP_L.size(); i++) cout << SLP_L[i] << " " << SLP_R[i] << endl;

    SimTtoG G(SLP_L.size(), SLP_L, SLP_R, 256);
    G.ReComp();
}