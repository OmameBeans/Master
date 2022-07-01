#include <iostream>
#include "SimTtoG.hpp"
#include "../RePair/RePair_Random.hpp"

using namespace std;

int main() {
    string S; cin >> S;
    RePair RP(S);
    RP.Comp();

    SimTtoG RL(RP.n-RP.Alphabet_Size,RP.L,RP.R);
    RL.PComp();
}