#include <iostream>
#include <fstream>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include "../run_program.h"

typedef pair<int8_t, int8_t> testpoint;
typedef vector<testpoint> testcase_t;
double beta = 2.0;

const int REGISTER_LIMIT = 2;
const int NUM_RESTARTS = 10;

int8_t popcount(uint8_t num) {
    int8_t ans = 0;
    int8_t penalty = 1;
    while(num != 0) {
        ans += (penalty) * (num & 1);
        num >>= 1;
        penalty++;
    }
    return ans;
}

void print_instr(const instr_t &instr) {
    switch(instr.opcode) {
        case ADD:
            printf("ADD %d %d\n", instr.reg_r, instr.reg_d);
            break;
        case SUB:
            printf("SUB %d %d\n", instr.reg_r, instr.reg_d);
            break;
        case MOV:
            printf("MOV %d %d\n", instr.reg_r, instr.reg_d);
            break;
        case UNUSED:
            printf("UNUSED\n");
            break;
        default:
            printf("WTF!!! %d\n", instr.opcode);
            exit(0);
    }
}

void read_testcase(const char filename[], vector<testpoint> &testcase) {
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

int total_reg_error(const vector<testpoint> &testcase, 
        const vector<instr_t> &program) {
    state_t state;
    int error = 0;
    for(auto &tp: testcase) {
        clear_state(state);
        state.reg[0] = tp.first;
        run_program(state, program);
        error += popcount((uint8_t)(state.reg[0] ^ tp.second));
    }
    return error;
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
                    { MOV, ADD, SUB, /*ADC, SBC, AND, OR, EOR */};
                const int num_opcodes = sizeof(opcodes) / sizeof(int);
                program[index].opcode = opcodes[rand() % num_opcodes];
                break;
            }
        // TODO: add other instructoins 
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
        /* * TODO: add other instructions */
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
        int opcodes[] = {MOV, ADD, SUB, /*ADC, SBC, AND, OR, EOR,
            COM, NEG, INC, DEC, TST, CLR, SER,  
            ANDI, ORI, CBR */};
        int num_opcodes = sizeof(opcodes) / sizeof(int);
        program[index].opcode = opcodes[rand() % num_opcodes];
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
    const int MOVES = 10000;
    const int opcodes [] = {ADD, SUB, MOV};
    const int num_opcodes = sizeof(opcodes) / sizeof(int);

    for(int restarts = 0; restarts < NUM_RESTARTS; ++restarts) {
        for(auto &instr: program) {
            instr.opcode = opcodes[rand() % num_opcodes];
            instr.reg_d = rand() % REGISTER_LIMIT;
            instr.reg_r = rand() % REGISTER_LIMIT;
        }
        int cost = total_reg_error(testcase, program);
        printf("cost before = %d\n", cost);
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
        printf("\t\tcost after = %d\n", cost);
        if(cost == 0) for(auto &instr: program) {
            print_instr(instr);
        }

    }
}





int main(int argc, char* argv[]) {
    srand(time(NULL));
    vector<testpoint> testcase;
    vector<instr_t> program(5);
    read_testcase(argv[1], testcase);
    cout << argv[1] << "\n";
    mcmc(testcase, program);
}
