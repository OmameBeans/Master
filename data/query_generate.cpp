#include<iostream>
#include<string>
#include<unordered_map>
#include<vector>
#include <cstdio>
#include <string>
#include <fstream>
#include <chrono>
#include <random>

#include "../Program/cmdline.h"


using namespace std;

int main(int argc, char *argv[])
{
    int N,Q; cin >> N >> Q;
    cmdline::parser p; // parser を定義
    p.add<string>("output_file", 'o', "output file name", true);

    p.parse_check(argc, argv);
    const string out_filename = p.get<string>("output_file");
    std::ofstream out_stream(out_filename+".txt");

    mt19937 mt{random_device{}()};
    uniform_int_distribution<int> dist(0, N-1);
    uniform_int_distribution<int> character(0, 25);

    out_stream << to_string(N);
    out_stream.write((char *) & " ",sizeof(char));
    out_stream << to_string(Q);
    out_stream.write((char *) & "\n",sizeof(char));


    for(int _ = 0; _ < N; _++) {
        char c = char(character(mt) + 'a');
        out_stream.write((char *) & c, sizeof(char));
    }

    out_stream.write((char *) & "\n",sizeof(char));

    for(int _ = 0; _ < Q; _++) {
        char l = char(dist(mt) + '0');
        char r = char(dist(mt) + '0');
        if(l > r) swap(r,l);
        out_stream.write((char *) & l, sizeof(char));
        out_stream.write((char *) & " ",sizeof(char));
        out_stream.write((char *) & r,sizeof(char));
        out_stream.write((char *) & "\n",sizeof(char));
    }

    return 1;
}