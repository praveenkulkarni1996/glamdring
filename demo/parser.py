class Command(object):
    def __init__(self, tokens):
        self.command = None
        if len(tokens) == 3:
            assert tokens[1] == '='
            self.command = ("ASSIGN", tokens[0], int(tokens[2]))
        if len(tokens) == 5 and tokens[3] == '+':
            assert tokens[1] == '='
            self.command = ("ADD", tokens[0], tokens[2], tokens[4])
        if len(tokens) == 5 and tokens[3] == '-':
            assert tokens[1] == '='
            self.command = ("SUB", tokens[0], tokens[2], tokens[4])
            pass

    def __str__(self):
        return self.command.__str__()

    def get_vars(self):
        if self.command[0]:
            if self.command[0] == 'ASSIGN':
                return set(self.command[1])
            if self.command[0] in ['ADD', 'SUB']:
                return set(self.command[1:])

    def convert(self, mapping):
        if self.command[0] == 'ASSIGN':
            cmd1 = 'LDI 16, {}'.format(self.command[2])
            cmd2 = 'MOV {}, 16'.format(mapping[self.command[1]], 16)
            return '\n'.join([cmd1, cmd2])
        if self.command[0] == 'ADD':
            cmd1 = 'MOV {}, {}'.format(mapping[self.command[1]], mapping[self.command[2]])
            cmd2 = 'ADD {}, {}'.format(mapping[self.command[1]], mapping[self.command[3]])
            return '\n'.join([cmd1, cmd2])
        if self.command[0] == 'SUB':
            cmd1 = 'MOV {}, {}'.format(mapping[self.command[1]], mapping[self.command[2]])
            cmd2 = 'SUB {}, {}'.format(mapping[self.command[1]], mapping[self.command[3]])
            return '\n'.join([cmd1, cmd2])


def convert_to_fixed_pt(num):
    return



with open("l_program.txt") as fin:
    cmds = [Command(line.strip().split()) for line in fin]

all_vars = list(set().union(*[cmd.get_vars() for cmd in cmds]))

assert len(all_vars) <= 16

mapping = dict(zip(all_vars, range(len(all_vars))))

# print mapping
outprog = '\n'.join(cmd.convert(mapping) for cmd in cmds)
print outprog
