#include "arith_logic.h"
#include <cassert>

using namespace std;

// http://www.atmel.com/images/Atmel-0856-AVR-Instruction-Set-Manual.pdf

void print_bitform(const int8_t reg) {
    cout << (int)reg << " : ";
    for(int i = 7; i >= 0; --i) {
        cout << ((reg >> i) & 1);
    }   cout << "\n";
}
void print_flags(const bool f[8]) {
    printf("(c=%d, z=%d, n=%d, v=%d, s=%d, h=%d, t=%d, i=%d)\n",
        f[0], f[1], f[2], f[3], f[4], f[5], f[6], f[7]);
}

void run_arith_logic(state_t &state, const instr_t &instr) {
    int8_t res;
    bool z, n, h, s, v, c;
    bool done = true;
    switch(instr.opcode) {
        case ADD: {
            res = state.reg[instr.reg_d] + state.reg[instr.reg_r];
            bool Rd7 = (state.reg[instr.reg_d] >> 7) & 1;
            bool Rr7 = (state.reg[instr.reg_d] >> 7) & 1;
            bool R7 = (res >> 7) & 1;
            bool Rd3 = (state.reg[instr.reg_d] >> 3) & 1;
            bool Rr3 = (state.reg[instr.reg_r] >> 3) & 1;
            bool R3 = (res >> 3) & 1;
            v = (Rd7 & Rr7 & ~R7) | (~Rd7 & ~Rr7 & R7);
            c = (Rd7 & Rr7) | (Rd7 & ~R7) | (Rr7 & ~R7);
            h = (Rd3 & Rr3) | (Rd3 & ~R3) | (Rr3 & ~R3);
            break;
        }
        case ADC: {
            res = state.reg[instr.reg_d] + state.reg[instr.reg_r] + state.flags[C_FLAG];
            bool Rd7 = (state.reg[instr.reg_d] >> 7) & 1;
            bool Rr7 = (state.reg[instr.reg_d] >> 7) & 1;
            bool R7 = (res >> 7) & 1;
            bool Rd3 = (state.reg[instr.reg_d] >> 3) & 1;
            bool Rr3 = (state.reg[instr.reg_r] >> 3) & 1;
            bool R3 = (res >> 3) & 1;
            v = (Rd7 & Rr7 & ~R7) | (~Rd7 & ~Rr7 & R7);
            c = (Rd7 & Rr7) | (Rd7 & ~R7) | (Rr7 & ~R7);
            h = (Rd3 & Rr3) | (Rd3 & ~R3) | (Rr3 & ~R3);
            break;
        }
        // case ADIW:
        //     assert(false);
        //     cout << "not implemented\n";
        //     break;
        case SUB: {
            res = state.reg[instr.reg_d] - state.reg[instr.reg_r];
            bool Rd7 = (state.reg[instr.reg_d] >> 7) & 1;
            bool Rr7 = (state.reg[instr.reg_d] >> 7) & 1;
            bool R7 = (res >> 7) & 1;
            bool Rd3 = (state.reg[instr.reg_d] >> 3) & 1;
            bool Rr3 = (state.reg[instr.reg_r] >> 3) & 1;
            bool R3 = (res >> 3) & 1;
            v = (Rd7 & ~Rr7 & ~R7) | (~Rd7 & Rr7 & R7);
            c = (~Rd7 & Rr7) | (Rr7 & R7) | (R7 & ~Rd7);
            h = (~Rd3 & Rr3) | (Rr3 & R3) | (R3 & ~Rd3);
            break;
        }
        case SUBI: {
            assert(16 <= instr.reg_d and instr.reg_d <= 31);
            cout << "unhandled flags here\n";
            res = state.reg[instr.reg_d] - instr.K;
            break;
        }
        case SBC: {
            res = state.reg[instr.reg_d] - state.reg[instr.reg_r] - state.flags[C_FLAG];
            bool Rd7 = (state.reg[instr.reg_d] >> 7) & 1;
            bool Rr7 = (state.reg[instr.reg_d] >> 7) & 1;
            bool R7 = (res >> 7) & 1;
            bool Rd3 = (state.reg[instr.reg_d] >> 3) & 1;
            bool Rr3 = (state.reg[instr.reg_r] >> 3) & 1;
            bool R3 = (res >> 3) & 1;
            v = (Rd7 & ~Rr7 & ~R7) | (~Rd7 & Rr7 & R7);
            c = (~Rd7 & Rr7) | (Rr7 & R7) | (R7 & ~Rd7);
            h = (~Rd3 & Rr3) | (Rr3 & R3) | (R3 & ~Rd3);
            break;
        }
        case SBCI: {
            assert(16 <= instr.reg_d and instr.reg_d <= 31);
            assert(0 <= instr.K and instr.K <= 255);
            res = state.reg[instr.reg_d] - state.reg[instr.reg_r] - instr.K - state.flags[C_FLAG];
            cout << "flags not correctly set here\n";
            break;
        }
        // case SBIW:
        //     assert(false);
        //     cout << "not implemented\n";
        //     break;
        case AND:{
            res = state.reg[instr.reg_d] & state.reg[instr.reg_r];
            h = state.flags[H_FLAG];
            v = 0;
            c = state.flags[C_FLAG];
            break;
        }
        case ANDI: {
            res = state.reg[instr.reg_d] & instr.K;
            h = state.flags[H_FLAG];
            v = 0;
            c = state.flags[C_FLAG];
            break;
        }
        case OR: {
            res = state.reg[instr.reg_d] | state.reg[instr.reg_r];
            h = state.flags[H_FLAG];
            v = 0;
            c = state.flags[C_FLAG];
            break;
        }
        case ORI:  {
            res = state.reg[instr.reg_d] | instr.K;
            h = state.flags[H_FLAG];
            v = 0;
            c = state.flags[C_FLAG];
            break;
        }
        case EOR: {
            res = state.reg[instr.reg_d] ^ state.reg[instr.reg_r];
            h = state.flags[H_FLAG];
            v = 0;
            c = state.flags[C_FLAG];
            break;
        }
        case COM: {
            res = ~state.reg[instr.reg_d];
            h = state.flags[H_FLAG];
            v = 0;
            c = 1;
            break;
        }
        case NEG: {
            res = -state.reg[instr.reg_d];
            cout << "error: h is not defined properly\n";
            h = ~((bool)(state.reg[instr.reg_d]));
            v = (res = 0x80);
            c = (res != 0);
            break;
        }
        case SBR: {
            res = state.reg[instr.reg_d] | instr.K;
            h = state.flags[H_FLAG];
            v = 0;
            c = state.flags[C_FLAG];
            break;
        }
        case CBR: {
            cout << "write tests for this first\n";
            res = state.reg[instr.reg_d] & instr.K;
            h = state.flags[H_FLAG];
            v = 0;
            c = state.flags[C_FLAG];
            break;
        }
        case INC: {
            res = state.reg[instr.reg_d] + 1;
            c = state.flags[C_FLAG];
            v = (state.reg[instr.reg_d] == 0x7f);
            h = state.flags[H_FLAG];
            break;
        }
        case DEC: {
            res = state.reg[instr.reg_d] - 1;
            c = state.flags[C_FLAG];
            v = ((uint8_t)state.reg[instr.reg_d] == 0x80);
            h = state.flags[H_FLAG];
            break;
        }
        case TST: {
            res = state.reg[instr.reg_d] & state.reg[instr.reg_d];
            c = state.flags[C_FLAG];
            v = 0;
            h = state.flags[H_FLAG];
            break;
        }
        case CLR: {
            res = state.reg[instr.reg_d] ^ state.reg[instr.reg_d];
            c = state.flags[C_FLAG];
            v = 0;
            h = state.flags[H_FLAG];
            break;
        }
        case SER: {
            res = 0xff;
            c = state.flags[C_FLAG];
            v = state.flags[V_FLAG];
            h = state.flags[H_FLAG];
            break;
        }
        default:
            done = false;
            break;
    }
    if(done) {
        if(instr.opcode == SBC) {
            z = (res == 0) ? state.flags[Z_FLAG] : 0;
        } else {
            z = (res == 0);
        }
        n = (res < 0);
        s = (n xor v);
        state.reg[instr.reg_d] = res;
        state.flags[Z_FLAG] = z;
        state.flags[N_FLAG] = n;
        state.flags[S_FLAG] = s;
        state.flags[V_FLAG] = v;
        state.flags[H_FLAG] = h;
        state.flags[C_FLAG] = c;
        return;
    }
    switch(instr.opcode) {
        case MUL:
        {
            uint16_t op1 = state.reg[instr.reg_d];
            uint16_t op2 = state.reg[instr.reg_r];
            uint16_t product = op1 * op2;
            state.reg[0] = (int8_t)((product) & (0xff));
            state.reg[1] = (int8_t)((product >> 8) & (0xff));
            done = true;
            break;
        }
        case MULS:
        {
            int16_t op1 = state.reg[instr.reg_d];
            int16_t op2 = state.reg[instr.reg_r];
            int16_t product = op1 * op2;
            state.reg[0] = (int8_t) (product & 0xff);
            state.reg[1] = (int8_t) ((product >> 8) & 0xff);
            done = true;
            break;
        }
    }
    if(not done) {
        cout << "not implemented: " << instr.opcode << "\n";
        assert(false);
    }
}

void test_ser(state_t &state, instr_t &instr) {
    instr.opcode = SER;
    state.reg[instr.reg_d] = -5;
    print_bitform(state.reg[instr.reg_d]);
    run_instruction(state, instr);
    print_bitform(state.reg[instr.reg_d]);
}

void test_adc(state_t &state, instr_t &instr) {
    instr.opcode = ADC;
    state.reg[1] = 5;
    state.reg[0] = 3;
    state.flags[C_FLAG] = false;
    run_instruction(state, instr);
    cout << state.reg[0];
}

void test_asr(state_t &state, instr_t &instr) {
    // instr.opcode = ASR;
    state.reg[instr.reg_d] = +5;
    run_instruction(state, instr);
    cout << state.reg[instr.reg_d];
}

void test_and(state_t &state, instr_t &instr) {
    instr.opcode = AND;
    state.reg[instr.reg_d] = 5;
    state.reg[instr.reg_r] = 4;
    run_instruction(state, instr);
    print_bitform(state.reg[instr.reg_d]);
}

void test_com(state_t &state, instr_t &instr) {
    instr.opcode = COM;
    state.reg[instr.reg_d] = 5;
    print_bitform(state.reg[instr.reg_d]);
    run_instruction(state, instr);
    print_bitform(state.reg[instr.reg_d]);
}

void test_mul(state_t &state, instr_t &instr) {
    instr.opcode = MUL;
    instr.reg_d = 2;
    instr.reg_r = 3;
    state.reg[instr.reg_d] = 2;
    state.reg[instr.reg_r] = 3;
    run_instruction(state, instr);
    assert(state.reg[0] == 6);
    assert(state.reg[1] == 0);
}

void test_prog(state_t &state) {
    instr_t instr1, instr2;
    instr1.opcode = ADD; instr1.reg_d = 1; instr1.reg_r = 0;
    instr2.opcode = ADD; instr2.reg_d = 2; instr2.reg_r = 1;
    state.reg[0] = 120;
    state.reg[1] = 250;
    state.reg[2] = -7;
    print_flags(state.flags);
    print_bitform(state.reg[instr1.reg_d]);
    print_bitform(state.reg[instr1.reg_r]);
    run_instruction(state, instr1);
    print_bitform(state.reg[instr1.reg_d]);
    print_bitform(state.reg[instr1.reg_r]);
    // run_instruction(state, instr2);
    // assert(state.reg[0] == 4);
    // assert(state.reg[1] == 6);
    // assert(state.reg[2] == -1);
    print_flags(state.flags);
    cout << ".";
}

//
// int main() {
//     instr_t instr;
//     state_t state;
//     memset(state.reg, 0, sizeof(state.reg));
//     memset(state.flags, 0, sizeof(state.flags));
//     instr.reg_d = 0;
//     instr.reg_r = 1;
//     // test_mul(state, instr);
//     test_prog(state);
//     cout << "\n";
// }
