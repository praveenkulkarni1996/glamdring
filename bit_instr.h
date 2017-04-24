#include "isa.h"
#include "arith_logic.h" // for print_bitform
#include <iostream>
#include <cassert>
using namespace std;


void run_bits(state_t &state, const instr_t &instr) {
    int8_t res;
    bool z, n, h, s, v, c;
    switch(instr.opcode) {
        case LSL:
            assert(0 <= instr.reg_d and instr.reg_d <= 31);
            res = state.reg[instr.reg_d] << 1;
            h = (state.reg[instr.reg_d] >> 3) & 1;
            n = (res >> 7) & 1;
            z = (res == 0);
            c = (state.reg[instr.reg_d] >> 7) & 1;
            v = n xor c;
            s = n xor v;
            break;
        case LSR:
            assert(0 <= instr.reg_d and instr.reg_d <= 31);
            res = 127 & (state.reg[instr.reg_d] >> 1);
            c = state.reg[instr.reg_d] & 1;
            h = state.flags[H_FLAG];
            n = false;
            v = n xor c;
            s = n xor s;
            z = (res == 0);
            break;
        case ASR:
            assert(0 <= instr.reg_d and instr.reg_d <= 31);
            res = state.reg[instr.reg_d] >> 1;
            c = state.reg[instr.reg_d] & 1;
            h = state.flags[H_FLAG];
            n = (res >> 7) & 1;
            z = (res == 0);
            v = n xor c;
            s = n xor v;
            break;
        case ROL:
        case ROR:
        case SWAP:
            printf("not implemented\n");
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

void test_ASR() {
    state_t state;
    memset(state.reg, 0, sizeof(state.reg));
    memset(state.mem, 0, sizeof(state.reg));
    state.reg[0] = -2;
    instr_t instr;
    instr.opcode = ASR;
    instr.reg_d = 0;
    print_bitform(state.reg[0]);
    run_bits(state, instr);
    print_bitform(state.reg[0]);
}
