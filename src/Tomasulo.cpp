//
// Created by Randy Mei on 2020/5/10.
//

#include "tabulate/table.hpp"

#include "Util.hpp"
#include "Tomasulo.hpp"

Tomasulo::Tomasulo(const char *input) :
        ars(new StationList), mrs(new StationList), lb(new StationList), reg(new RegList), add(new UnitList), timer(1),
        mul(new UnitList), load(new UnitList), canIssue(true), inst(parseFile(input)), nextIssueIndex(0), read(nullptr),
        readOperand(false) {
    for (int i = 0; i < ARS_NUM; i++) ars->push_back(new Station(i + 1, ADD_U));
    for (int i = 0; i < MRS_NUM; i++) mrs->push_back(new Station(i + 1, MUL_U));
    for (int i = 0; i <  LB_NUM; i++)  lb->push_back(new Station(i + 1, LOAD_U));

    for (int i = 0; i <  ADD_NUM; i++)  add->push_back(new Unit(i + 1,  ADD_U));
    for (int i = 0; i <  MUL_NUM; i++)  mul->push_back(new Unit(i + 1,  MUL_U));
    for (int i = 0; i < LOAD_NUM; i++) load->push_back(new Unit(i + 1, LOAD_U));

    for (int i = 0; i < REG_NUM; i++) reg->push_back(new Reg(i));
}

void Tomasulo::step(bool print) {
    // writeback
    for (auto &station : *ars)
        if (station->busy && station->time == 0 && station->exec > 0)
            writeback(station);
    for (auto &station : *mrs)
        if (station->busy && station->time == 0 && station->exec > 0)
            writeback(station);
    for (auto &station : *lb)
        if (station->busy && station->time == 0 && station->exec > 0)
            writeback(station);

    // allocate function unit
    allocate(ADD_U);
    allocate(MUL_U);
    allocate(LOAD_U);

    // issue
    bool issued = false;
    Station *currentStation;
    Inst *nextIssue;
    if (canIssue) {
        nextIssue = inst->at(nextIssueIndex);
        switch (nextIssue->type) {
            case ADD:
            case SUB:
            case JUMP:
                for (auto &station : *ars) {
                    if (!station->busy) {
                        issued = true;
                        station->busy = true;
                        station->inst = nextIssue;
                        currentStation = station;
                        break;
                    }
                }
                break;
            case MUL:
            case DIV:
                for (auto &station : *mrs) {
                    if (!station->busy) {
                        issued = true;
                        station->busy = true;
                        station->inst = nextIssue;
                        currentStation = station;
                        break;
                    }
                }
                break;
            case LOAD:
                for (auto &station : *lb) {
                    if (!station->busy) {
                        issued = true;
                        station->busy = true;
                        station->inst = nextIssue;
                        currentStation = station;
                        break;
                    }
                }
                break;
        }
        if (issued) {
            nextIssueIndex++;
            if (currentStation->inst->type == JUMP || nextIssueIndex >= inst->size()) canIssue = false;
            if (currentStation->inst->issue == 0) currentStation->inst->issue = timer;
            currentStation->issue = timer;
            readOperand = true;
            read = currentStation;
        }
    }

    // read operand
    if (readOperand) {
        switch (read->inst->type) {
            case ADD: case SUB: case MUL: case DIV: {
                Reg *dst = reg->at(read->inst->op[0]);
                Reg *src1 = reg->at(read->inst->op[1]);
                Reg *src2 = reg->at(read->inst->op[2]);
                if (src1->station == nullptr) {
                    read->vj = src1->val;
                } else {
                    read->qj = src1->station;
                }
                if (src2->station == nullptr) {
                    read->vk = src2->val;
                } else {
                    read->qk = src2->station;
                }
                if (read->qj == nullptr && read->qk == nullptr) {
                    if (read->inst->type == ADD || read->inst->type == SUB) allocate(ADD_U); else allocate(MUL_U);
                } else read->exec = 0;
                dst->station = read;
                break;
            } case LOAD: {
                Reg *dst = reg->at(read->inst->op[0]);
                read->vk = read->inst->op[1];
                if (read->qk == nullptr) allocate(LOAD_U); else read->exec = 0;
                dst->station = read;
                break;
            } case JUMP: {
                Reg *cond = reg->at(read->inst->op[1]);
                if (cond->station == nullptr) {
                    read->vj = cond->val;
                } else {
                    read->qj = cond->station;
                }
                if (read->qj == nullptr) allocate(ADD_U); else read->exec = 0;
                break;
            }
        }
        readOperand = false;
    }

    // execute
    for (auto &station : *ars) {
        if (station->time > 0 && station->exec <= timer) {
            if (--station->time == 0) {
                if (station->inst->finish == 0) station->inst->finish = timer;
                if (station->inst->type == ADD) {
                    station->inst->val = station->vj + station->vk;
                } else if (station->inst->type == SUB) {
                    station->inst->val = station->vj - station->vk;
                } else if (station->inst->type == JUMP) {
                    if (station->vj == station->inst->op[0]) {
                        nextIssueIndex--;
                        nextIssueIndex += station->inst->op[2];
                        if (nextIssueIndex >= inst->size()) canIssue = false;
                    }
                }
            }
        }
    }
    for (auto &station : *mrs) {
        if (station->time > 0 && station->exec <= timer) {
            if (--station->time == 0) {
                if (station->inst->finish == 0) station->inst->finish = timer;
                if (station->inst->type == MUL) {
                    station->inst->val = station->vj * station->vk;
                } else if (station->inst->type == DIV) {
                    station->inst->val = station->vk == 0 ? station->vj : station->vj / station->vk;
                }
            }
        }
    }
    for (auto &station : *lb) {
        if (station->time > 0 && station->exec <= timer) {
            if (--station->time == 0) {
                if (station->inst->finish == 0) station->inst->finish = timer;
                station->inst->val = station->vk;
            }
        }
    }
    timer++;
    if (print) {
        system("clear");
        printConsole();
    }
}

void Tomasulo::allocate(UnitType type) {
    UnitList *list;
    switch (type) {
        case  ADD_U: list = add ; break;
        case  MUL_U: list = mul ; break;
        case LOAD_U: list = load; break;
    }
    for (auto &unit : *list) {
        if (!unit->busy) {
            if (nextExec(unit) == nullptr) break;
        }
    }
}

Station *Tomasulo::nextExec(Unit *toUse) {
    Station *next = nullptr;
    StationList *list;
    switch (toUse->type) {
        case  ADD_U: list = ars; break;
        case  MUL_U: list = mrs; break;
        case LOAD_U: list = lb ; break;
    }
    for (auto &station : *list) {
        if (station->busy && station->fu == nullptr && station->qj == nullptr && station->qk == nullptr) {
            if (next == nullptr) next = station;
            if (next->issue > station->issue || (next->issue == station->issue && next->inst->index > station->inst->index)) next = station;
        }
    }
    if (next != nullptr) next->start(toUse, timer + 1);
    return next;
}

void Tomasulo::writeback(Station *wb) {
    for (auto &station : *ars) {
        if (station->qj == wb) {
            station->qj = nullptr;
            station->vj = wb->inst->val;
        }
        if (station->qk == wb) {
            station->qk = nullptr;
            station->vk = wb->inst->val;
        }
    }
    for (auto &station : *mrs) {
        if (station->qj == wb) {
            station->qj = nullptr;
            station->vj = wb->inst->val;
        }
        if (station->qk == wb) {
            station->qk = nullptr;
            station->vk = wb->inst->val;
        }
    }
    for (auto &station : *lb) {
        if (station->qj == wb) {
            station->qj = nullptr;
            station->vj = wb->inst->val;
        }
        if (station->qk == wb) {
            station->qk = nullptr;
            station->vk = wb->inst->val;
        }
    }
    for (auto &r : *reg) {
        if (r->station == wb) {
            r->station = nullptr;
            r->val = wb->inst->val;
        }
    }
    if (wb->inst->type == JUMP && nextIssueIndex < inst->size()) canIssue = true;
    if (wb->inst->write == 0) wb->inst->write = timer;
    wb->reset();
}

bool Tomasulo::finish() {
    if (nextIssueIndex < inst->size()) return false;
    for (auto &station : *ars) if (station->busy) return false;
    for (auto &station : *mrs) if (station->busy) return false;
    for (auto &station : *lb) if (station->busy) return false;
    return true;
}

void Tomasulo::runAll(const char *output, bool print) {
    while (!finish()) step(print);
    printOutput(inst, output);
}

void Tomasulo::printConsole() {
    tabulate::Table cycle;
    printf("=== cycle %d ===\n", timer - 1);
    cycle.add_row({"Index", "Code", "Issue", "Finish", "Write"});
    for (int i = 0; i < inst->size(); i++) {
        char buf[4], issue[4], finish[4], writeback[4];
        sprintf(buf, "%d", i + 1);
        sprintf(issue, "%d", inst->at(i)->issue);
        sprintf(finish, "%d", inst->at(i)->finish);
        sprintf(writeback, "%d", inst->at(i)->write);
        cycle.add_row({
            int2str(i + 1),
            inst->at(i)->origin,
            inst->at(i)->issue == 0 ? "" : int2str(inst->at(i)->issue),
            inst->at(i)->finish == 0 ? "" : int2str(inst->at(i)->finish),
            inst->at(i)->write == 0 ? "" : int2str(inst->at(i)->write)
        });
    }
    cycle.format().font_align(tabulate::FontAlign::center);
    tabulate::Table rss;
    rss.add_row({"Name", "Busy", "Time", "FU", "Op", "Vj", "Vk", "Qj", "Qk"});
    for (auto &station : *ars) {
        rss.add_row({
            station->name(),
            !station->busy ? "No" : "Yes",
            !station->busy ? "" : station->time == -1 ? "" : int2str(station->time),
            !station->busy ? "" : station->fu == nullptr ? "" : station->fu->name(),
            !station->busy ? "" : station->inst == nullptr ? "" : type2str(station->inst->type),
            !station->busy ? "" : station->qj != nullptr ? "" : int2str(station->vj),
            !station->busy ? "" : station->qk != nullptr ? "" : station->inst->type == JUMP ? "" : int2str(station->vk),
            !station->busy ? "" : station->qj == nullptr ? "" : station->qj->name(),
            !station->busy ? "" : station->qk == nullptr ? "" : station->qk->name()
        });
    }
    for (auto &station : *mrs) {
        rss.add_row({
            station->name(),
            !station->busy ? "No" : "Yes",
            !station->busy ? "" : station->time == -1 ? "" : int2str(station->time),
            !station->busy ? "" : station->fu == nullptr ? "" : station->fu->name(),
            !station->busy ? "" : station->inst == nullptr ? "" : type2str(station->inst->type),
            !station->busy ? "" : station->qj != nullptr ? "" : int2str(station->vj),
            !station->busy ? "" : station->qk != nullptr ? "" : int2str(station->vk),
            !station->busy ? "" : station->qj == nullptr ? "" : station->qj->name(),
            !station->busy ? "" : station->qk == nullptr ? "" : station->qk->name()
        });
    }
    for (auto &station : *lb) {
        rss.add_row({
            station->name(),
            !station->busy ? "No" : "Yes",
            !station->busy ? "" : station->time == -1 ? "" : int2str(station->time),
            !station->busy ? "" : station->fu == nullptr ? "" : station->fu->name(),
            !station->busy ? "" : station->inst == nullptr ? "" : type2str(station->inst->type),
            "",
            !station->busy ? "" : station->qk != nullptr ? "" : int2str(station->vk),
            "",
            !station->busy ? "" : station->qk == nullptr ? "" : station->qk->name()
        });
    }
    rss.format().font_align(tabulate::FontAlign::center);
    tabulate::Table regs1;
    int mid = REG_NUM / 2;
    regs1.add_row({"Name", "Status", "Value"});
    for (int i = 0; i < mid; i++) {
        auto r = reg->at(i);
        regs1.add_row({r->name(), r->station == nullptr ? "" : r->station->name(), int2str(r->val)});
    }
    regs1.format().font_align(tabulate::FontAlign::center);
    tabulate::Table regs2;
    regs2.add_row({"Name", "Status", "Value"});
    for (int i = mid; i < REG_NUM; i++) {
        auto r = reg->at(i);
        regs2.add_row({r->name(), r->station == nullptr ? "" : r->station->name(), int2str(r->val)});
    }
    regs2.format().font_align(tabulate::FontAlign::center);
    tabulate::Table main;
    main.add_row({"cycle table", "reserve station table", "register table1", "register table2"});
    main.add_row({cycle, rss, regs1, regs2});
    main.format().font_align(tabulate::FontAlign::center);
    printf("%s\n", main.str().c_str());
    std::cin.get();
}

