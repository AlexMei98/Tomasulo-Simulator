//
// Created by Randy Mei on 2020/5/11.
//

#include "Unit.hpp"

const char *Unit::name() const {
    char *buf = new char[8];
    switch (type) {
        case  ADD_U: sprintf(buf,  "Add%d", index); return buf;
        case  MUL_U: sprintf(buf,  "Mul%d", index); return buf;
        case LOAD_U: sprintf(buf, "Load%d", index); return buf;
    }
}

Unit::Unit(int index, UnitType type1) : index(index), type(type1), busy(false) {}
