#include <bits/stdc++.h>
using namespace std;

template <typename T>
void vectorPrint(vector<T> v) {
    int i=0;
    for (; i < (int)v.size()-1; i++) cout << v[i] << ", ";
    cout << v[i] << endl;
}

vector<int> vectorGap(vector<int> v, int n) {
    vector<int> gap(n);
    gap[0] = v[0];

    for (int i = 1; i < n; ++i) 
        gap[i] = v[i] - v[i-1];

    return gap;
}

vector<int> vectorSample(vector<int> v, int n) {
    vector<int> sample;
    sample.reserve(n);
    int b = 2; //1024 or 4096 or 8192
    int i=0;
    while (i < (int)v.size()) {
        sample.push_back(v[i]);
        i+=b;
    }

    return sample;
}

bool sample_range(vector<int> &sample, int b, int x, int n, int &L, int &R) {
    if (x < sample[0]) return false;
    auto it = upper_bound(sample.begin(), sample.end(), x);
    int sample_pos = it - sample.begin() - 1;

    L = sample_pos * b;

    if (sample_pos + 1 < (int)sample.size()) R = (sample_pos+1) * b;
    else R = n;

    return true;
}

bool GC_decode(vector<int> &GC, vector<int> &sample, int x, int b, int L, int R) {
    int curr = sample[L/b]; // A[L] = sample[L/b]
    cout << "curr: " << curr << endl;
    if (curr == x) return true;

    for (int i=L+1; i <= R; ++i) {
        curr = curr + GC[i];    
    /*
    como sample[L/b] = A[L], y GC[i] = A[i] - A[i-1]
    entonces A[i] = GC[i] + A[i-1]
    por lo tanto curr = A[L] = A[i-1]
    */
        cout << "i: " << i << "     curr: " << curr << endl;
        if (curr == x) return true;
        
    }


    
    return false;

}



int main() {

    vector<int> A = {2, 7, 10, 12, 13, 14, 16};
    cout << "A: "; vectorPrint(A);

    int search_element = 13;
    int size = (int)A.size();
    int b = 2;

    vector<int> gap = vectorGap(A, size);
    cout << "gap: "; vectorPrint(gap);

    vector<int> sample = vectorSample(A, size);
    cout << "sample: "; vectorPrint(sample);

    int L, R;
    sample_range(sample, b, search_element, size, L, R);
    cout << "L_idx: " << L << "  A[L] = " << A[L] << "\nR_idx: " << R << "  A[R] = " << A[R] << endl;

    GC_decode(gap, sample, search_element, 2, L, R) ? cout << "true" << endl : cout << "false" << endl;

    return 1;
}

// g++ -std=c++17 -Wall -o test test.cpp; ./test