//
// Created by Randy Mei on 2020/5/10.
//

#include "Tomasulo.hpp"
#include "Station.hpp"

void Station::reset() {
    busy = false;
    fu->busy = false;
    fu = nullptr;
    vj = vk = issue = 0;
    qj = qk = nullptr;
    inst = nullptr;
    time = -1;
}

void Station::start(Unit *toUse, int timer) {
    exec = timer;
    fu = toUse;
    fu->busy = true;
    switch (inst->type) {
        case ADD : time = ADD_CYCLE ; break;
        case SUB : time = SUB_CYCLE ; break;
        case MUL : time = MUL_CYCLE ; break;
        case LOAD: time = LOAD_CYCLE; break;
        case JUMP: time = JUMP_CYCLE; break;
        case DIV : time = vk == 0 ? DIV0_CYCLE : DIV_CYCLE; break;
    }
}

Station::Station(int index, UnitType type) : index(index), type(type) {}

const char *Station::name() const {
    char *buf = new char[8];
    switch (type) {
        case  ADD_U: sprintf(buf, "Ars%d", index); return buf;
        case  MUL_U: sprintf(buf, "Mrs%d", index); return buf;
        case LOAD_U: sprintf(buf,  "LB%d", index); return buf;
    }
}
