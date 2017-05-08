#include <iostream>
#include <fstream>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <chrono>
#include "../run_program.h"
#include "helper.h"

using namespace std::chrono;

#define SYNTHESIS false
#define OPTIMIZATION true

typedef pair<int8_t, int8_t> testpoint;
typedef vector<testpoint> testcase_t;

// for the <a, b> ----> y
typedef pair<int8_t, int8_t> input2_t;
typedef int8_t output_t;
typedef pair<input2_t, output_t> testpt2_t;
typedef vector<testpt2_t> testcase2_t;

double beta = 1.0;

const int REGISTER_LIMIT = 2;
const int NUM_RESTARTS = 100;
const int PROGRAM_LEN = 6;
const int SCALE = 1;
bool MODE = SYNTHESIS;

const bool TWO_INPUTS = true;

inline int
popcount(uint8_t num) {
    // not just popcount, penalized popcount
    const int scale = 1;
    int ans = 0;
    for(int i=0, penalty = scale; i < 8; i++, penalty += scale) {
        ans += (penalty) * ((num >> i) & 1);
    }
    return ans;
}


inline void
read_testcase(const char filename[], vector<testpoint> &testcase) {
    ifstream fin(filename);
    int x, y;
    while(fin >> x >> y) {
        testcase.push_back({(int8_t)x, (int8_t)y});
    }
    fin.close();
}


void clear_state(state_t &state) {
    memset(state.reg, 0, sizeof(state.reg));
    return;
    memset(state.mem, 0, sizeof(state.mem));
    memset(state.flags, 0, sizeof(state.flags));
}

inline int
total_reg_error(const testcase_t &testcase,
        const vector<instr_t> &program) {
    state_t state;
    int error[REGISTER_LIMIT];
    memset(error, 0, sizeof(error));
    for(auto &tp: testcase) {
        clear_state(state);
        state.reg[0] = tp.first;
        run_program(state, program);
        for(int i = 0; i < REGISTER_LIMIT; ++i) {
            error[i] += popcount((uint8_t)(state.reg[i] ^ tp.second));
        }
    }
    int inefficency_cost = 0;
    if(MODE == OPTIMIZATION) {
        for(auto &instr: program) {
            inefficency_cost += get_cost(instr);
        }
    }
    return *min_element(error, error + REGISTER_LIMIT) + SCALE * inefficency_cost;
}

inline int
total_reg_error2(const testcase2_t &testcase,
        const vector<instr_t> &program) {
    state_t state;
    int error[REGISTER_LIMIT];
    memset(error, 0, sizeof(error));
    for(auto &tp: testcase) {
        clear_state(state);
        state.reg[0] = tp.first.first;
        state.reg[1] = tp.first.second;
        run_program(state, program);
        for(int i = 0; i < REGISTER_LIMIT; ++i) {
            error[i] += popcount((uint8_t)(state.reg[i] ^ tp.second));
        }
    }
    int inefficency_cost = 0;
    if(MODE == OPTIMIZATION) {
        for(auto &instr: program) {
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
mcmc_opcode(vector<instr_t> &program, const int oldcost,
        const vector<testpoint> &testcase) {
    const int index = rand() % program.size();
    const instr_t cached_instr = program[index];
    //
    switch(cached_instr.opcode) {
        case UNUSED:
            break;
        case MOV: case ADD: case SUB: case ADC:
        case SBC: case AND: case OR: case EOR:
            {
                const int opcodes[] =
                    { MOV, ADD, SUB, /*ADC, SBC,*/ AND, OR, EOR};
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
                {INC, DEC, /*COM, NEG, TST, CLR, SER, LSL, LSR, ASR,
                    ROL, ROR, SWAP*/};
                const int num_opcodes = sizeof(opcodes) / sizeof(int);
                program[index].opcode = opcodes[rand() % num_opcodes];
                break;
            }
        default:
            assert(false);
    }

    const int newcost = total_reg_error(testcase, program);
    if(accept_mcmc_transition(oldcost, newcost)) {
        return newcost;
    }
    program[index] = cached_instr;
    return oldcost;
}

inline int
mcmc_operand(vector<instr_t> &program, const int oldcost,
        const vector<testpoint> &testcase) {
    const int index = rand() % program.size();
    const instr_t cached_instr = program[index];

    switch(cached_instr.opcode) {
        case UNUSED:
            break;
        case MOV: case ADD: case SUB: case ADC:
        case SBC: case AND: case OR: case EOR:
            // if random number is even then change src register
            // if random number is odd then change dest register
            if(rand() & 1)
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
    if(accept_mcmc_transition(oldcost, newcost)) {
        return newcost;
    }
    program[index] = cached_instr;
    return oldcost;
}


inline int
mcmc_swap(vector<instr_t> &program, const int oldcost,
        const vector<testpoint> &testcase) {
    int index1 = rand() % program.size();
    int index2 = rand() % program.size();
    swap(program[index1], program[index2]);
    const int newcost = total_reg_error(testcase, program);
    if(accept_mcmc_transition(oldcost, newcost)) {
        return newcost;
    }
    swap(program[index1], program[index2]);
    return oldcost;
}

inline int
mcmc_instr(vector<instr_t> &program, const int oldcost,
        const vector<testpoint> &testcase) {
    const double p_unused = 0.3; // FIXME
    const int index = rand() % program.size();
    const instr_t cached_instr = program[index];
    if(rand() % 100 <= 100 * p_unused) {
        program[index].opcode = UNUSED;
    }
    else {
        int opcodes[] = {MOV, ADD, SUB, /*ADC, SBC,*/ AND, OR, EOR,/*
            COM, NEG,*/ INC, DEC, /*TST, CLR, SER,
            ANDI, ORI, CBR,*/
            /*LSL, LSR, ASR, ROL, ROR, SWAP*/
        };
        int num_opcodes = sizeof(opcodes) / sizeof(int);
        program[index].opcode = opcodes[rand() % num_opcodes];
        program[index].reg_d = num_opcodes % REGISTER_LIMIT;
        program[index].reg_r = num_opcodes % REGISTER_LIMIT;
    }
    int newcost = total_reg_error(testcase, program);
    if(accept_mcmc_transition(oldcost, newcost)) {
        return newcost;
    }
    program[index] = cached_instr;
    return oldcost;
}

void
mcmc(const vector<testpoint> &testcase, vector<instr_t> &program) {
    const int MOVES = 2000000;
    const int opcodes [] = {ADD, SUB, MOV};
    const int num_opcodes = sizeof(opcodes) / sizeof(int);
    vector<instr_t> synprog(program);
    vector<instr_t> bestprog(program);
    int bestcost;

    if(MODE == SYNTHESIS) {
        bestcost = 1e8;
    }
    else if(MODE == OPTIMIZATION) {
        bestcost = total_reg_error(testcase, synprog);
    }

    for(int restarts = 0; restarts < NUM_RESTARTS; ++restarts) {
        if(MODE == SYNTHESIS) {
            for(auto &instr: program) {
                instr.opcode = opcodes[rand() % num_opcodes];
                instr.reg_d = rand() % REGISTER_LIMIT;
                instr.reg_r = rand() % REGISTER_LIMIT;
            }
        }
        else if(MODE == OPTIMIZATION) {
            program = synprog;
        }
        int cost = total_reg_error(testcase, program);
        printf("cost before = %d\n", cost);
        // core loop
        high_resolution_clock::time_point t1 = high_resolution_clock::now();
        for(int moves = 0; moves < MOVES; ++moves) {
            const int randnum = rand() % 100;
            if(randnum < 25) {
                cost = mcmc_opcode(program, cost, testcase);
            }
            else if(randnum < 50) {
                cost = mcmc_operand(program, cost, testcase);
            }
            else if(randnum < 75) {
                cost = mcmc_swap(program, cost, testcase);
            }
            else {
                cost = mcmc_instr(program, cost, testcase);
            }
        }
        high_resolution_clock::time_point t2 = high_resolution_clock::now();
        const int duration = duration_cast<microseconds> (t2 - t1).count();
        const int exec_instrs = (MOVES * PROGRAM_LEN);
        const double i_per_second = 1e6 * ((double) exec_instrs / (double) duration);
        printf("\t\tduration = %d, instrs = %d\n", duration, exec_instrs);
        printf("\t\tinstrs/second = %f\n", i_per_second);
        printf("\t\tcost after = %d\n", cost);
        if(cost < bestcost) for(auto &instr: program) {
            print_instr(instr);
            bestcost = cost;
            bestprog = program;
        }
    }
    program = bestprog;
}


int
testcase_two(testcase_t testcase) {
    vector<instr_t> program (1);
    for(instr_t &instr: program) {
      instr.opcode = LSL;
      instr.reg_d = 0;
    }
    int error = total_reg_error(testcase, program);
    printf("error = %d\n", error);
    return 0;
}

int
testcase_p1(testcase_t testcase) {
    vector<instr_t> program (3);
    /* MOV R0 to R1 */
    /* DEC R1 */
    /* AND R1, R0 */
    program[0].opcode = MOV;
    program[0].reg_d = 1;
    program[0].reg_r = 0;

    program[1].reg_d = 1;
    program[1].opcode = DEC;

    program[2].opcode = AND;
    program[2].reg_r = 1;
    program[2].reg_d = 0;

    int error = total_reg_error(testcase, program);
    printf("p1: error = %d\n", error);
    return error;
}

int
testcase_p2(testcase_t testcase) {
    // is power of 2
    vector<instr_t> program (3);
    /* MOV R0 to R1 */
    /* INC R1 */
    /* AND R1, R0 */
    program[0].opcode = MOV;
    program[0].reg_d = 1;
    program[0].reg_r = 0;

    program[1].reg_d = 1;
    program[1].opcode = INC;

    program[2].opcode = AND;
    program[2].reg_r = 1;
    program[2].reg_d = 0;

    int error = total_reg_error(testcase, program);
    printf("p2: error = %d\n", error);
    return error;
}

int
testcase_p3(testcase_t testcase) {
    vector<instr_t> program (2);
    /* SUB R0 from R1 */
    /* AND R1, R0 */
    program[0].opcode = SUB;
    program[0].reg_d = 1;
    program[0].reg_r = 0;

    program[1].opcode = AND;
    program[1].reg_r = 1;
    program[1].reg_d = 0;

    int error = total_reg_error(testcase, program);
    printf("p3: error = %d\n", error);
    return error;
}

int
testcase_p4(testcase_t testcase) {
    // is power of 2
    vector<instr_t> program (3);
    /* MOV R0 to R1 */
    /* DEC R1 */
    /* AND R1, R0 */
    program[0].opcode = MOV;
    program[0].reg_d = 1;
    program[0].reg_r = 0;

    program[1].reg_d = 1;
    program[1].opcode = DEC;

    program[2].opcode = EOR;
    program[2].reg_r = 1;
    program[2].reg_d = 0;

    int error = total_reg_error(testcase, program);
    printf("p4: error = %d\n", error);
    return error;
}

int
testcase_p17(testcase_t testcase) {
    vector<instr_t> program (5);
    // o1 = x - 1
    // o2 = o1 | x
    // o3 = o2 + 1
    // res = o3 & x
    program[0].opcode = MOV;
    program[0].reg_d = 1;
    program[0].reg_r = 0;

    program[1].opcode = DEC;
    program[1].reg_d = 1;

    program[2].opcode = OR;
    program[2].reg_r = 0;
    program[2].reg_d = 1;

    program[3].opcode = INC;
    program[3].reg_d = 1;

    program[4].opcode = AND;
    program[4].reg_r = 1;
    program[4].reg_d = 0;
    int error = total_reg_error(testcase, program);
    printf("p17: error = %d\n", error);
    return error;
}

int main(int argc, char* argv[]) {
    srand(time(NULL));
    vector<testpoint> testcase;
    vector<instr_t> program(PROGRAM_LEN);
    read_testcase(argv[1], testcase);
    printf("%s\n", argv[1]);

    if(false) {
        testcase_p17(testcase);
        return 0;
    }

    printf("entering SYSTHESIS phase ...\n");
    MODE = SYNTHESIS;
    mcmc(testcase, program);
    printf("entering OPTIMIZATION phase ...\n");
    MODE = OPTIMIZATION;
    beta = 0.5;
    mcmc(testcase, program);
}
