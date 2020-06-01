//
// Created by Randy Mei on 2020/5/10.
//

#ifndef TOMASULO_STATION_HPP
#define TOMASULO_STATION_HPP

#include <vector>

#include "Unit.hpp"
#include "Inst.hpp"

class Station {
public:
    Station(int index, UnitType type);

    void reset();

    void start(Unit *toUse, int timer);

    [[nodiscard]] const char *name() const;

    int index;
    UnitType type;
    int exec{0};
    int issue{0};
    bool busy{false};
    int time{-1};
    Station *qj{nullptr}, *qk{nullptr};
    int vj{0}, vk{0};
    Unit *fu{nullptr};
    Inst *inst{nullptr};
};

typedef std::vector<Station*> StationList;

#endif //TOMASULO_STATION_HPP
