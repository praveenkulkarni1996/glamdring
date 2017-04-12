#include "isa.h"
#include <cassert>

using namespace std;

// http://www.atmel.com/images/Atmel-0856-AVR-Instruction-Set-Manual.pdf

#define ADD 1
#define ADC 2
#define ADIW 3 // not implemented
#define SUB 4
#define SUBI 5
#define SBC 6
#define SBCI 7
#define SBIW 100 // not implemented
#define AND 8
#define ANDI 9
#define OR 10
#define ORI 11
#define EOR 12
#define COM 13
#define NEG 14
#define SBR 15
#define CBR 16
#define INC 17
#define DEC 18
#define TST 19
#define CLR 20
#define SER 21
#define MUL 22  // --- not yet implemented
#define MULS 23
#define MULSU 24
#define FMUL 25
#define FMULS 26
#define FMULSU 27
#define DES 28

void print_bitform(const int8_t reg) {
    cout << (int)reg << " : ";
    for(int i = 7; i >= 0; --i) {
        cout << ((reg >> i) & 1);
    }   cout << "\n";
}

void run_instruction(state_t &state, instr_t &instr) {
    int8_t res;
    bool done = true;
    switch(instr.opcode) {
        case ADD:
            res = state.reg[instr.reg_d] + state.reg[instr.reg_r];
            break;
        case ADC:
            res = state.reg[instr.reg_d] + state.reg[instr.reg_r] + state.flags[CARRY_FLAG];
            break;
        // case ADIW:
        //     assert(false);
        //     cout << "not implemented\n";
        //     break;
        case SUB:
            res = state.reg[instr.reg_d] - state.reg[instr.reg_r];
            break;
        case SUBI:
            res = state.reg[instr.reg_d] - instr.K;
            break;
        case SBC:
            res = state.reg[instr.reg_d] - state.reg[instr.reg_r] - state.flags[CARRY_FLAG];
            break;
        case SBCI:
            res = state.reg[instr.reg_d] - state.reg[instr.reg_r] - instr.K - state.flags[CARRY_FLAG];
            break;
        // case SBIW:
        //     assert(false);
        //     cout << "not implemented\n";
        //     break;
        case AND:
            res = state.reg[instr.reg_d] & state.reg[instr.reg_r];
            break;
        case ANDI:
            res = state.reg[instr.reg_d] & instr.K;
            break;
        case OR:
            res = state.reg[instr.reg_d] | state.reg[instr.reg_r];
            break;
        case ORI:
            res = state.reg[instr.reg_d] | instr.K;
            break;
        case EOR:
            res = state.reg[instr.reg_d] ^ state.reg[instr.reg_r];
            break;
        case COM:
            res = ~state.reg[instr.reg_d];
            break;
        case NEG:
            res = -state.reg[instr.reg_d];
            break;
        case SBR:
            res = state.reg[instr.reg_d] | instr.K;
            break;
        case CBR:
            res = state.reg[instr.reg_d] & (~instr.K);
            break;
        case INC:
            res = state.reg[instr.reg_d] + 1;
            break;
        case DEC:
            res = state.reg[instr.reg_d] - 1;
            break;
        case TST:
            res = state.reg[instr.reg_d] & state.reg[instr.reg_d];
            break;
        case CLR:
            res = state.reg[instr.reg_d] ^ state.reg[instr.reg_d];
            break;
        case SER:
            res = 0xff;
            break;
        default:
            done = false;
            break;
    }
    if(done) {
        state.reg[instr.reg_d] = res;
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
    state.flags[CARRY_FLAG] = false;
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
    state.reg[0] = 5;
    state.reg[1] = 2;
    state.reg[2] = 1;
    run_instruction(state, instr1);
    run_instruction(state, instr2);
    assert(state.reg[0] == 5);
    assert(state.reg[1] == 7);
    assert(state.reg[2] == 8);
    cout << ".";
}

int main() {
    instr_t instr;
    state_t state;
    memset(state.reg, 0, sizeof(state.reg));
    instr.reg_d = 0;
    instr.reg_r = 1;
    // test_mul(state, instr);
    test_prog(state);
    cout << "\n";
}
