//
//  main.cpp
//  Bitcount
//
//  Created by Michele Costabile on 04/01/15.
//  Copyright (c) 2015 Michele Costabile. All rights reserved.
//

#include <ctime>
#include <iostream>
#include <vector>
using namespace std;

static int BIT_COUNT[256];
static int bitCountOfByte(unsigned int value) {
    return BIT_COUNT[value & 0xFF];
}

unsigned int bitcount_with_ands(unsigned int u)
{
    unsigned int uCount;
    
    uCount = u - ((u >> 1) & 033333333333) - ((u >> 2) & 011111111111);
    return ((uCount + (uCount >> 3)) & 030707070707) % 63;
}

unsigned int other_bitcount_with_ands(unsigned int i)
{
    i = i - ((i >> 1) & 0x55555555);
    i = (i & 0x33333333) + ((i >> 2) & 0x33333333);
    return ((i + (i >> 4) & 0xF0F0F0F) * 0x1010101) >> 24;
}

unsigned int bitcount_by_lookup(unsigned int value) {
    return bitCountOfByte(value)
    + bitCountOfByte(value >> 8)
    + bitCountOfByte(value >> 16)
    + bitCountOfByte(value >> 24);
}

unsigned int bitcount_kerningham(unsigned int n) {
    unsigned int c; // c accumulates the total bits set in v
    for (c = 0; n; c++) {
        n &= n - 1; // clear the least significant bit set
    }
    
    return c;
}

unsigned int bitcount_vanilla(unsigned int v) {
    unsigned int c; // c accumulates the total bits set in v
    
    for (c = 0; v; v >>= 1) {
        c += v & 1;
    }
    
    return c;
}

unsigned int bitcount_vanilla2(unsigned int v) {
    unsigned int c = 0; // c accumulates the total bits set in v
    
    for (int i = 0; i < 32; i++) {
        c += v & 1;
        v >>= 1;
    }
    
    return c;
}

unsigned int bitcount_builtin(unsigned int v) {
    return __builtin_popcount(v);
}

#define ITERATIONS 1000000

void benchmark_function(unsigned int (*method_under_test)(unsigned int param), unsigned int value)
{
    clock_t start, stop;
    
    start = clock();
    long count = 0;
    
    for (unsigned int i = 0; i < ITERATIONS; i++) {
        count = method_under_test(value);
    }
    
    stop = clock();
    
    double ns = ((double)stop - (double)start)*1000.0/CLOCKS_PER_SEC;
    
    cout << "elapsed -->  " << ns << "ns, value= " << std::hex << value;
    cout << " count = " << std::dec << count << endl;
}

void initialize_count() {
    for (unsigned int i = 0; i < sizeof(BIT_COUNT)/sizeof(unsigned int); i++) {
        BIT_COUNT[i] = bitcount_vanilla(i);
    }
}

int main() {
    initialize_count();
    srand(7);
    
    vector<unsigned int> values = {0, 0xa, 0x00a000f0, 0Xaaaaaaaa, 0xdeadcafe, 0xffffffff};
    
    for (int j=0; j< values.size(); j++) {
        unsigned int value_tested = values[j]; // il numero di cui calcolare il peso
        
        cout << "-- new run --" << endl;
        cout << "vanilla:  ";
        benchmark_function(bitcount_vanilla, value_tested);
        cout << "vanilla2:  ";
        benchmark_function(bitcount_vanilla2, value_tested);
        cout << "lookup:  ";
        benchmark_function(bitcount_by_lookup, value_tested);
        cout << "ands:  ";
        benchmark_function(bitcount_with_ands, value_tested);
        cout << "more ands:  ";
        benchmark_function(other_bitcount_with_ands, value_tested);
        cout << "kerningham:  ";
        benchmark_function(bitcount_kerningham, value_tested);
        cout << "__builtin_popcount:  ";
        benchmark_function(bitcount_builtin, value_tested);
        cout << endl;
    }
    return 0;
}
