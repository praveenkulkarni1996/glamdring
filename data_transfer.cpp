#include "data_transfer.h"
#include <cassert>
using namespace std;

// #define MOV 100
// #define MOVW 101
// #define LDI 102
// #define LDS 103

// data transfer instructions
void run_dt_instruction(state_t &state, const instr_t &instr) {
    switch(instr.opcode) {
        case MOV:
            state.reg[instr.reg_d] = state.reg[instr.reg_r];
            break;
        case MOVW:
            assert(instr.reg_d % 2 == 0);
            assert(instr.reg_r % 2 == 0);
            state.reg[instr.reg_d] = state.reg[instr.reg_r];
            state.reg[instr.reg_d + 1] = state.reg[instr.reg_r + 1];
            break;
        case LDI:
            assert(16 <= instr.reg_d and instr.reg_d <= 31);
            // assert(0 <= instr.K <= 255);
            state.reg[instr.reg_d] = instr.K;
            break;
        case LDS:
            assert(0 <= instr.k and instr.k <= 2048); // not
            state.reg[instr.reg_d] = state.mem[instr.k];
            break;

    }
}
