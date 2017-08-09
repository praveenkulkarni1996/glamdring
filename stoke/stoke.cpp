// Copyright 2016 Praveen Kulkarni

#include <iostream>
#include <fstream>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <chrono>
#include "../arch/run_program.h"
#include "helper.h"

#define SYNTHESIS false
#define OPTIMIZATION true

//  configuration
//  REGISTER_LIMIT : number of registers to use
//  NUM_RESTARTS : number of restarts to use
//  PROGRAM_LEN : length of program
//  SCALE : multiplying factor for instruction cost
//  MOVES : number of moves before restarting
#ifndef REGISTER_LIMIT
    #define REGISTER_LIMIT 4
#endif

#ifndef NUM_RESTARTS
    #define NUM_RESTARTS 10
#endif

#ifndef PROGRAM_LEN
    #define PROGRAM_LEN 8
#endif

#ifndef SCALE
    #define SCALE 1
#endif

#ifndef MOVES
    #define MOVES 500000
#endif

namespace stoke {
using namespace std::chrono;

typedef pair<int8_t, int8_t> testpoint;
typedef vector<testpoint> testcase_t;

// for the <a, b> ----> y
typedef pair<int8_t, int8_t> input2_t;
typedef int8_t output_t;
typedef pair<input2_t, output_t> testpt2_t;
typedef vector<testpt2_t> testcase2_t;

double beta = 1.0;



bool MODE = SYNTHESIS;

#define TWO_INPUTS false

testcase2_t glob_2testcase;
vector<testpoint> testcase;

inline int
popcount(uint8_t num) {
    // not just popcount, penalized popcount
    const int scale = 1;
    int ans = 0;
    for (int i=0, penalty = scale; i < 8; i++, penalty += scale) {
        ans += (penalty) * ((num >> i) & 1);
    }
    return ans;
}


inline void
read_testcase(const char filename[], vector<testpoint> &testcase) {
    ifstream fin(filename);
    int x, y;
    while (fin >> x >> y) {
        testcase.push_back({(int8_t)x, (int8_t)y});
    }
    fin.close();
}

inline void
read_2input_testcase(const char filename[]) {
    ifstream fin(filename);
    glob_2testcase.clear();
    int x, y, z;
    while (fin >> x >> y >> z) {
        glob_2testcase.push_back({{(int8_t)x, (int8_t)y}, (int8_t)z});
    }
    fin.close();
}


inline void
clear_state(state_t &state) {
    memset(state.reg, 0, sizeof(state.reg));
    return;
    memset(state.mem, 0, sizeof(state.mem));
    memset(state.flags, 0, sizeof(state.flags));
}

inline void load_state(state_t &state) {
  state.reg[3] = 1;
  state.reg[2] = 3;
}

inline int
total_reg_error(const testcase_t &testcase,
                const vector<instr_t> &program) {
    state_t state;
    int error[REGISTER_LIMIT];
    memset(error, 0, sizeof(error));


    if (TWO_INPUTS) {
        for (auto &tp : glob_2testcase) {
            clear_state(state);
            load_state(state);
            state.reg[0] = tp.first.first;
            state.reg[1] = tp.first.second;
            run_program(state, program);
            for (int i = 0; i < REGISTER_LIMIT; ++i) {
                error[i] += popcount((uint8_t)(state.reg[i] ^ tp.second));
            }
        }
        int inefficency_cost = 0;
        if (MODE == OPTIMIZATION) {
            for (auto &instr : program) {
                inefficency_cost += get_cost(instr);
            }
        }
        return *min_element(error, error + REGISTER_LIMIT)
            + SCALE * inefficency_cost;
    }


    for (auto &tp : testcase) {
      // cout << "tp: " << (int)tp.first << " " << (int)tp.second << "\n";
        clear_state(state);
        load_state(state);
        state.reg[0] = tp.first;
        run_program(state, program);
        for (int i = 0; i < REGISTER_LIMIT; ++i) {
            error[i] += popcount((uint8_t)(state.reg[i] ^ tp.second));
        }
    }
    int inefficency_cost = 0;
    if (MODE == OPTIMIZATION) {
        for (auto &instr : program) {
            inefficency_cost += get_cost(instr);
        }
    }
    return *min_element(error, error + REGISTER_LIMIT)
        + SCALE * inefficency_cost;
}

inline int
total_reg_error2(const testcase2_t &testcase,
        const vector<instr_t> &program) {
    state_t state;
    int error[REGISTER_LIMIT];
    memset(error, 0, sizeof(error));
    for (auto &tp : testcase) {
        clear_state(state);
        state.reg[0] = tp.first.first;
        state.reg[1] = tp.first.second;
        run_program(state, program);
        for (int i = 0; i < REGISTER_LIMIT; ++i) {
            error[i] += popcount((uint8_t)(state.reg[i] ^ tp.second));
        }
    }
    int inefficency_cost = 0;
    if (MODE == OPTIMIZATION) {
        for (auto &instr : program) {
            inefficency_cost += get_cost(instr);
        }
    }
    return *min_element(error, error + REGISTER_LIMIT) + SCALE * inefficency_cost;
}


inline bool
accept_mcmc_transition(const int prevcost, const int newcost) {
    float accept_prob = min(1.0, exp(-beta * newcost / prevcost));
    return (rand() % 100000 <= accept_prob * 100000);
}

inline int
mcmc_opcode(vector<instr_t> &program, const int oldcost) {
    const int index = rand() % program.size();
    const instr_t cached_instr = program[index];
    switch (cached_instr.opcode) {
        case UNUSED:
            break;
        case MOV: case ADD: case SUB: case ADC:
        case SBC: case AND: case OR: case EOR:
        case MULS:
            {
                const int opcodes[] =
                    { MOV, ADD, /*SUB,*/ MULS/*ADC, SBC,*/ /*AND, OR, EOR*/};
                const int num_opcodes = sizeof(opcodes) / sizeof(int);
                program[index].opcode = opcodes[rand() % num_opcodes];
                break;
            }
        case INC: case DEC: case COM: case NEG:
        case TST: case CLR: case SER:
        case LSL: case LSR: case ASR:
        case ROL: case ROR: case SWAP:
            {
                const int opcodes[] =
                {INC, DEC, /*COM, NEG, TST, CLR, SER, *//*LSL, LSR, ASR,*/
                    /*ROL, ROR, SWAP*/};
                const int num_opcodes = sizeof(opcodes) / sizeof(int);
                program[index].opcode = opcodes[rand() % num_opcodes];
                break;
            }
        default:
            assert(false);
    }

    const int newcost = total_reg_error(testcase, program);
    if (accept_mcmc_transition(oldcost, newcost)) {
        return newcost;
    }
    program[index] = cached_instr;
    return oldcost;
}

inline int
mcmc_operand(vector<instr_t> &program, const int oldcost) {
    const int index = rand() % program.size();
    const instr_t cached_instr = program[index];

    switch(cached_instr.opcode) {
        case UNUSED:
            break;
        case MOV: case ADD: case SUB: case ADC:
        case SBC: case AND: case OR: case EOR:
        case MULS: // FIXME
            // if random number is even then change src register
            // if random number is odd then change dest register
            if (rand() & 1)
                program[index].reg_r = rand() % REGISTER_LIMIT;
            else
                program[index].reg_d = rand() % REGISTER_LIMIT;
            break;
        case INC: case DEC: case COM: case NEG:
        case TST: case CLR: case SER:
        case LSL: case LSR: case ASR:
        case ROL: case ROR: case SWAP:
            program[index].reg_d = rand() % REGISTER_LIMIT;
            break;
        default:
            assert(false);
    }
    const int newcost = total_reg_error(testcase, program);
    if (accept_mcmc_transition(oldcost, newcost)) {
        return newcost;
    }
    program[index] = cached_instr;
    return oldcost;
}


inline int
mcmc_swap(vector<instr_t> &program, const int oldcost) {
    int index1 = rand() % program.size();
    int index2 = rand() % program.size();
    swap(program[index1], program[index2]);
    const int newcost = total_reg_error(testcase, program);
    if (accept_mcmc_transition(oldcost, newcost)) {
        return newcost;
    }
    swap(program[index1], program[index2]);
    return oldcost;
}

inline int
mcmc_instr(vector<instr_t> &program, const int oldcost) {
    const double p_unused = 0.3;  // FIXME
    const int index = rand() % program.size();
    const instr_t cached_instr = program[index];
    if (rand() % 100 <= 100 * p_unused) {
        program[index].opcode = UNUSED;
    } else {
        int opcodes[] = {MOV, ADD, /*SUB,*/ MULS/*ADC, SBC,*//* AND, OR, EOR,*/
            /* COM, NEG,*/ /*INC, DEC, *//*TST, CLR, SER,\
            ANDI, ORI, CBR,*/
            /*LSL, LSR, ASR, ROL, ROR, SWAP*/
        };
        int num_opcodes = sizeof(opcodes) / sizeof(int);
        program[index].opcode = opcodes[rand() % num_opcodes];
        program[index].reg_d = num_opcodes % REGISTER_LIMIT;
        program[index].reg_r = num_opcodes % REGISTER_LIMIT;
    }
    int newcost = total_reg_error(testcase, program);
    if (accept_mcmc_transition(oldcost, newcost)) {
        return newcost;
    }
    program[index] = cached_instr;
    return oldcost;
}

void
mcmc(vector<instr_t> &program) {
    const int opcodes [] = {ADD, /*SUB,*/ MOV, MULS};
    const int num_opcodes = sizeof(opcodes) / sizeof(int);
    vector<instr_t> synprog(program);
    vector<instr_t> bestprog(program);
    int bestcost;

    if (MODE == SYNTHESIS) {
        bestcost = 1e8;
    } else if (MODE == OPTIMIZATION) {
        bestcost = total_reg_error(testcase, synprog);
    }

    for (int restarts = 0; restarts < NUM_RESTARTS; ++restarts) {
        if (MODE == SYNTHESIS) {
            for (auto &instr : program) {
                instr.opcode = opcodes[rand() % num_opcodes];
                instr.reg_d = rand() % REGISTER_LIMIT;
                instr.reg_r = rand() % REGISTER_LIMIT;
            }
        } else if (MODE == OPTIMIZATION) {
            program = synprog;
        }
        int cost = total_reg_error(testcase, program);
        printf("cost before = %d\n", cost);
        // core loop
        auto t1 = high_resolution_clock::now();
        for (int moves = 0; moves < MOVES; ++moves) {
            const int randnum = rand() % 100;
            if (randnum < 25) {
                cost = mcmc_opcode(program, cost);
            } else if (randnum < 50) {
                cost = mcmc_operand(program, cost);
            } else if (randnum < 75) {
                cost = mcmc_swap(program, cost);
            } else {
                cost = mcmc_instr(program, cost);
            }
        }
        auto t2 = high_resolution_clock::now();
        const int duration = duration_cast<microseconds>(t2 - t1).count();
        const int64_t exec_instrs = (MOVES * 1LL * PROGRAM_LEN);
        const double i_per_second = 1e6 * (static_cast<double>(exec_instrs) / static_cast<double>(duration));
        printf("\t\tduration = %d,instrs = %lld", duration, exec_instrs);
        printf("\t\tinstrs/second = %f\n", i_per_second);
        printf("\t\tcost after = %d\n", cost);
        if (cost < bestcost) {
            for (auto &instr : program) {
                print_instr(instr);
                bestcost = cost;
                bestprog = program;
            }
        }
    }
    program = bestprog;
}
}  // namespace stoke

using namespace stoke;

void test() {
  instr_t one, two;
  one.opcode = ADD; one.reg_r = 3; one.reg_d = 0;
  two.opcode = MULS; two.reg_r = 0; two.reg_d = 0;
  vector<instr_t> prog;
  prog.push_back(one);
  prog.push_back(two);
  int error = total_reg_error(testcase, prog);
  cout << "error " << error << "\n";
}

int main(int argc, char* argv[]) {
    srand(time(NULL));
    vector<instr_t> program(PROGRAM_LEN);
    printf("%s\n", argv[1]);

    if (TWO_INPUTS) {
        read_2input_testcase(argv[1]);
    } else {
        read_testcase(argv[1], testcase);
    }
//     test();

    printf("entering SYSTHESIS phase ...\n");
    MODE = SYNTHESIS;
    mcmc(program);
    printf("entering OPTIMIZATION phase ...\n");
    MODE = OPTIMIZATION;
    beta = 0.5;
    mcmc(program);
}
