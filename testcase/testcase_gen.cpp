#include <iostream>
#include <cmath>
#include <cassert>
#include "../run_program.h"
using namespace std;

const double fixed_point = 1 / double(32);

float mapping[256];

void build_mapping() {
    int8_t num = -128;
    do {
        float floating_num = (int)num * fixed_point;
        mapping[num + 128] = floating_num;
    } while(num++ != 127);
}


int float_to_fixed(float fl_num) {
    // actually maps to index
    for(int i = 255; i >= 0; --i) {
        if(fl_num >= mapping[i]) 
            return i;
    }
    return 0;
}

int main() {
    build_mapping();
    /*
    for(int i = 0; i <= 255; ++i) {
        cout << i << "\t" << mapping[i] << "\t" << float_to_fixed(mapping[i]) << "\n";
    }
    */
    for(int i = 0; i <= 255; ++i) {
        float flx = mapping[i];
        float fly = flx * 7; //exp(flx);
        int fx = i - 128; 
        int fy = float_to_fixed(fly) - 128;
        assert(-128 <= fx and fx <= 127);
        assert(-128 <= fy and fy <= 127);
        if(-1 <= flx and flx <= 1 and -100 < fy and fy < 100)
            cout << fx << " " << fy << "\n";
    }
}
