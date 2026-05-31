#pragma once

#include <bits/stdc++.h>

namespace Compresion {
    using namespace std;

    /* 
    k: amount of bits allowed for representation (reason why we start from k-1 to 0)

    x >> i : shifts bits to the right of x by i positions. bits that fall off the right are discarded, 0s fill the left
    5>>3, 5 in binary is 0101
    after moving 0101 by 3 positions, we get 0000 (0101 -> 0010 -> 0001 -> 0000)

    ((x>>i) & 1) ? '1' : '0'
    "& 1", hides every bit except the rightmost one, which is originally the leftmost one which is the one we want
    "? '1' : '0'", simply makes sure we append the same symbol of the rightmost bit  */

    string toBinary(int x, int k) {
        string bin="";
        for (int i = k-1; i >= 0; i--) 
            bin += ((x >> i) & 1) ? '1' : '0'; //extracts leftmost bit, 5 in binary is 0101, we extract leftmost bit which is 0
    
        return bin;
    }

    template <typename T>
    pair <string, string> eliasFanoCodif(const vector<T> &A) {
        string low="";
        string high="";
        long long int n=(int)A.size();

        // k = log2(n) -> n = 2^k
        int k=0;
        while (1 << (k+1) <= n) k+=1; // 01 -> 010 -> 0100 -> 01000 -> 010000 ...

        /* int d = 2;
        for (int i=0; i<k; i++) d = d*2;  */

        int d = 1 << k; // 2^k
        int v, lo, hi;
        for (int i=0; i<(int)A.size(); i++) {
            v = A[i];
            lo = v%d;
            hi = v/d; //int division = floor

            low+=toBinary(lo, k);

            string hibit="";
            for (int i=0; i<hi; i++) hibit+="0";
            hibit+="10";
            high+=hibit;
        }

        cout << "low bits: " << low << endl;
        cout << "high bits: " << high << endl;

        return {low, high};
    } 
}