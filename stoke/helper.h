#include "../isa.h"

void print_instr(const instr_t &instr) {
    switch(instr.opcode) {
        case UNUSED:
            printf("UNUSED\n");
            break;
        case ADD:
            printf("ADD %d %d\n", instr.reg_r, instr.reg_d);
            break;
        case ADC:
            printf("ADC %d %d\n", instr.reg_r, instr.reg_d);
            break;
        case SUB:
            printf("SUB %d %d\n", instr.reg_r, instr.reg_d);
            break;
        case MOV:
            printf("MOV %d %d\n", instr.reg_r, instr.reg_d);
            break;
        case AND:
            printf("AND %d %d\n", instr.reg_r, instr.reg_d);
            break;
        case OR:
            printf("OR %d %d\n", instr.reg_r, instr.reg_d);
            break;
        case EOR:
            printf("EOR %d %d\n", instr.reg_r, instr.reg_d);
            break;
        case TST:
            printf("TST %d %d\n", instr.reg_r, instr.reg_d);
            break;
        case INC:
            printf("INC %d\n", instr.reg_d);
            break;
        case DEC:
            printf("DEC %d\n", instr.reg_d);
            break;
        default:
            printf("WTF!!! %d\n", instr.opcode);
    }
}

inline int get_cost(const instr_t &instr) {
    if(instr.opcode == UNUSED) return 0;
    return 1;
}
