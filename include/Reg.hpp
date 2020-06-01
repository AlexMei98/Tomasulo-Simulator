//
// Created by Randy Mei on 2020/5/11.
//

#ifndef TOMASULO_REG_HPP
#define TOMASULO_REG_HPP

#include <vector>
#include "Station.hpp"

class Reg {
public:
    explicit Reg(int index);

    [[nodiscard]] const char *name() const;

    int index;
    Station *station{nullptr};
    int val{0};
};

typedef std::vector<Reg*> RegList;

#endif //TOMASULO_REG_HPP
