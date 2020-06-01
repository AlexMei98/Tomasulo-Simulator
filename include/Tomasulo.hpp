//
// Created by Randy Mei on 2020/5/10.
//

#ifndef TOMASULO_TOMASULO_HPP
#define TOMASULO_TOMASULO_HPP

#include "Inst.hpp"
#include "Station.hpp"
#include "Unit.hpp"
#include "Reg.hpp"

constexpr int LOAD_CYCLE = 3;
constexpr int JUMP_CYCLE = 1;
constexpr int  ADD_CYCLE = 3;
constexpr int  SUB_CYCLE = 3;
constexpr int  MUL_CYCLE = 4;
constexpr int  DIV_CYCLE = 4;
constexpr int DIV0_CYCLE = 1;

constexpr int  ADD_NUM = 3;
constexpr int  MUL_NUM = 2;
constexpr int LOAD_NUM = 2;

constexpr int ARS_NUM = 6;
constexpr int MRS_NUM = 3;
constexpr int  LB_NUM = 3;
constexpr int REG_NUM = 32;

constexpr const char* REG_NAME = "R";

class Tomasulo {
public:
    explicit Tomasulo(const char *input);

    void step(bool print);

    void runAll(const char *output, bool printOutput);

    void allocate(UnitType type);

    Station *nextExec(Unit *toUse);

    void writeback(Station *wb);

    bool finish();

    void printConsole();

private:
    InstList *inst;
    StationList *ars;
    StationList *mrs;
    StationList *lb;
    UnitList *add;
    UnitList *mul;
    UnitList *load;
    RegList *reg;

    bool canIssue;
    bool readOperand;
    Station *read;
    int nextIssueIndex;
    int timer;
};

#endif //TOMASULO_TOMASULO_HPP
