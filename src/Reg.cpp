//
// Created by Randy Mei on 2020/5/11.
//

#include "Tomasulo.hpp"
#include "Reg.hpp"

Reg::Reg(int index) : index(index) {}

const char *Reg::name() const {
    char *buf = new char[8];
    sprintf(buf, "%s%d", REG_NAME, index);
    return buf;
}
