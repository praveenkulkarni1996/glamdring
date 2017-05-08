#pragma once
#include <iostream>
#include <vector>
// #include <bits/stdc++.h>

#define REGISTERS 32
#define MEMORY 2048


#define C_FLAG 0
#define Z_FLAG 1
#define N_FLAG 2
#define V_FLAG 3
#define S_FLAG 4
#define H_FLAG 5
#define T_FLAG 6
#define I_FLAG 7

struct state_t {
    int8_t reg[REGISTERS];
    bool flags[8];
    int8_t mem[MEMORY];
    // todo add memory
};

struct instr_t {
    int opcode;
    int8_t cost;
    int8_t reg_d, reg_r;
    int mode;
    int K, k;
};


// table 4-2 arithmetic and logic instructions
#define ADD 1
#define ADC 2
#define ADIW 3 // not implemented
#define SUB 4
#define SUBI 5
#define SBC 6
#define SBCI 7
#define SBIW 99 // not implemented
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
#define UNUSED 120 // --not yet implemented

// data transfer instructions
#define MOV 100
#define MOVW 101
#define LDI 102
#define LDS 103

// bit shift operstaions
#define LSL 121
#define LSR 122
#define ROL 123
#define ROR 124
#define ASR 125
#define SWAP 126

// table 4-3
