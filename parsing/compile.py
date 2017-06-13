import json
import pprint
from classes import ProgramLine, Instruction


def reading_config_file(filename):
    with open(filename) as configfile:
        config_data = json.load(configfile)
    pprint.pprint(config_data)
    return config_data


def reading_program_file(filename):
    programs = []
    with open(filename) as programfile:
        for line in programfile:
            programs.append(line.strip())
    return programs


def print_program(assembly_code, asm_outputfile):
    with open(asm_outputfile, 'w') as asm_out:
        for assembly_instr in assembly_code:
            print(assembly_instr, file=asm_out)


def build_mapping(config):

    # depending upon the architechture more temp variables will be needed
    # 8 bit needs 1 temp variable
    # 16 bit needs 2 temp variables
    # 32 bit needs 3 temp variables
    temp_vars_by_arch = {
        8: {'__temp__'},
        16: {'__temp0__', '__temp1__'},
        32: {'__temp0__', '__temp1__', '__temp2__', '__temp3__'}
    }

    input_vars = set(config['input'])
    output_vars = set(config['output'])
    temp_vars = temp_vars_by_arch[config['arch']]

    vars_set = input_vars | output_vars | temp_vars
    mapping = dict(zip(list(vars_set), range(len(vars_set))))
    return mapping


def compile_8bit(programline, mapping):
    assembly = []
    temp_register = mapping['__temp__']
    operation_to_opcode = {'+': 'ADD', '-': 'SUB'}
    # first move the temp register
    assembly.append(Instruction(opcode='MOV',
                                rd=temp_register,
                                rr=mapping[programline.a]))
    # apply the operation to the temp register
    assembly.append(Instruction(opcode=operation_to_opcode[programline.op],
                                rd=temp_register,
                                rr=mapping[programline.b]))
    # move the result to the destination register
    assembly.append(Instruction(opcode='MOV',
                                rd=mapping[programline.c],
                                rr=temp_register))
    return assembly


def compile16_bit(programline, mapping):
    # assembly = []
    # temp0, temp1 = mapping['__temp0__'], mapping['__temp1__']
    pass


def compile(program, mapping, arch):
    assembly_code = []
    if arch == 8:
        for line in program:
            programline = ProgramLine(line)
            programline.parse()
            assembly_code += compile_8bit(programline, mapping)
    else:
        # 16 bits and above
        raise NotImplemented
    return assembly_code


def main():
    config = reading_config_file('config.json')
    program = reading_program_file('adding.pk')
    mapping = build_mapping(config)

    # read architecture from the configfile
    arch = config['arch']
    print(arch)

    # compile the code
    assembly_code = compile(program, mapping, arch)

    print_program(assembly_code, 'adding.asm')
'''
    with open('adding.asm', 'w') as out_asm:
        for assembly_instr in assembly_code:
            print(assembly_instr, file=out_asm)
'''

if __name__ == '__main__':
    main()
