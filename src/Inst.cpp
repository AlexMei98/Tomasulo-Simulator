//
// Created by Randy Mei on 2020/5/10.
//

#include "Inst.hpp"

#include <utility>

Inst::Inst(int index, std::string origin, InstType type, int op1, int op2, int op3) :
        index(index), origin(std::move(origin)), type(type), op(new int[3]{op1, op2, op3}), issue(0), finish(0),
        write(0), val(0) {}
