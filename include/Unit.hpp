//
// Created by Randy Mei on 2020/5/11.
//

#ifndef TOMASULO_UNIT_HPP
#define TOMASULO_UNIT_HPP

#include <cstdio>
#include <vector>

enum UnitType {
    ADD_U = 0,
    MUL_U = 1,
    LOAD_U = 2,
};

class Unit {
public:
    explicit Unit(int index, UnitType type1);

    [[nodiscard]] const char *name() const;

    bool busy;
    int index;
    UnitType type;
};

typedef std::vector<Unit*> UnitList;

#endif //TOMASULO_UNIT_HPP
