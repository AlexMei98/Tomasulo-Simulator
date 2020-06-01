//
// Created by Randy Mei on 2020/5/10.
//

#ifndef TOMASULO_INST_HPP
#define TOMASULO_INST_HPP

#include <vector>
#include <string>

enum InstType {
    ADD = 0,
    MUL = 1,
    SUB = 2,
    DIV = 3,
    LOAD = 4,
    JUMP = 5,
};

class Inst {
public:
    Inst(int index, std::string origin, InstType type, int op1, int op2, int op3);

    int issue;
    int finish;
    int write;

    int index;
    std::string origin;
    InstType type;
    int *op;

    int val;
};

typedef std::vector<Inst*> InstList;

#endif //TOMASULO_INST_HPP
