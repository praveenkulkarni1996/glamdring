#include "isa.h"
#include "arith_logic.h" // for print_bitform
#include <iostream>
#include <cassert>
using namespace std;


void run_bits(state_t &state, const instr_t &instr) {
    int8_t res;
    bool z, n, h, s, v, c;
#ifdef DEBUG
            assert(0 <= instr.reg_d and instr.reg_d <= 31);
#endif
    switch(instr.opcode) {
        case LSL:
            res = state.reg[instr.reg_d] << 1;
            h = (state.reg[instr.reg_d] >> 3) & 1;
            n = (res >> 7) & 1;
            z = (res == 0);
            c = (state.reg[instr.reg_d] >> 7) & 1;
            v = n xor c;
            s = n xor v;
            break;
        case LSR:
            res = 127 & (state.reg[instr.reg_d] >> 1);
            c = state.reg[instr.reg_d] & 1;
            h = state.flags[H_FLAG];
            n = false;
            v = n xor c;
            s = n xor s;
            z = (res == 0);
            break;
        case ASR:
            res = state.reg[instr.reg_d] >> 1;
            c = state.reg[instr.reg_d] & 1;
            h = state.flags[H_FLAG];
            n = (res >> 7) & 1;
            z = (res == 0);
            v = n xor c;
            s = n xor v;
            break;
        case ROL:
            res = (state.reg[instr.reg_d] << 1) | (c & 1);
            h = (state.reg[instr.reg_d] >> 3) & 1;
            c =  (state.reg[instr.reg_d] >> 7) & 1;
            n = (res < 0);
            z = (res == 0);
            v = n xor c;
            s = n xor v;
            break;
        case ROR:
            res = (0x8f & (state.reg[instr.reg_d] >> 1)) | (c << 7);
            h = state.flags[H_FLAG];
            n = (res < 0);
            z = (res == 0);
            c = state.reg[instr.reg_d] & 1;
            v = n xor c;
            s = n xor v;
            break;
        case SWAP:
            {
                int8_t lo = (state.reg[instr.reg_d] & 0x0f);
                int8_t hi = (state.reg[instr.reg_d] & 0xf0);
                res = (lo << 4) | ((hi >> 4) & 0x0f);
                print_bitform(state.reg[instr.reg_d]);
                print_bitform(lo);
                print_bitform(hi);
                print_bitform(res);
                exit(0);
                h = state.flags[H_FLAG];
                n = state.flags[N_FLAG];
                z = state.flags[Z_FLAG];
                c = state.flags[C_FLAG];
                v = state.flags[V_FLAG];
                s = state.flags[S_FLAG];
            }
            break;
    }
    state.reg[instr.reg_d] = res;
    state.flags[H_FLAG] = h;
    state.flags[N_FLAG] = n;
    state.flags[S_FLAG] = s;
    state.flags[Z_FLAG] = z;
    state.flags[V_FLAG] = v;
    state.flags[C_FLAG] = c;
}


void test_LSL() {
    state_t state;
    memset(state.reg, 0, sizeof(state.reg));
    memset(state.mem, 0, sizeof(state.reg));
    state.reg[0] = 3;
    instr_t instr;
    instr.opcode = LSL;
    instr.reg_d = 0;
    print_bitform(state.reg[0]);
    run_bits(state, instr);
    print_bitform(state.reg[0]);
}

void test_SWAP() {
    state_t state;
    memset(state.reg, 0, sizeof(state.reg));
    memset(state.mem, 0, sizeof(state.reg));
    state.reg[0] = -128;
    instr_t instr;
    instr.opcode = SWAP;
    instr.reg_d = 0;
    // print_bitform(state.reg[0]);
    run_bits(state, instr);
    // print_bitform(state.reg[0]);
}

int main() {
    test_SWAP();
}
