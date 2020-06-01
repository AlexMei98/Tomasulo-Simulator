//
// Created by Randy Mei on 2020/5/10.
//

#include <cstring>
#include <fstream>

#include "Util.hpp"

constexpr hash_t hashInstType(char const *str, hash_t last_value) {
    return *str ? hashInstType(str + 1, (((hash_t) *str) ^ last_value) * prime) : last_value;
}

constexpr unsigned long long operator "" _hash(char const *p, hash_t) {
    return hashInstType(p);
}

hash_t hash_(const std::string &str) {
    hash_t ret{basis};

    for (char c : str) {
        ret ^= (hash_t) c;
        ret *= prime;
    }

    return ret;
}

int dec2int(const std::string &str) {
    int x = 0, tmp;
    for (char c : str) {
        tmp = c - '0';
        x = x * 10 + tmp;
    }
    return x;
}

int hex2int(const std::string &str) {
    int x = 0, tmp;
    for (char c : str.substr(2)) {
        if ((c > '9' || c < '0') && (c > 'Z' || c < 'A')) continue;
        tmp = (c <= '9' && c >= '0') ? (c - '0') : (c - 'A' + 10);
        x = x * 16 + tmp;
    }
    return x;
}

bool isEmpty(const std::string& str) {
    if (str.empty()) return true;
    for (char c : str) if (!(c == '\t' || c == '\n' || c == ' ')) return false;
    return true;
}

const char *type2str(InstType type) {
    switch (type) {
        case InstType::LOAD: return "LD"  ;
        case InstType::JUMP: return "JUMP";
        case InstType::ADD : return "ADD" ;
        case InstType::SUB : return "SUB" ;
        case InstType::MUL : return "MUL" ;
        case InstType::DIV : return "DIV" ;
        default: fprintf(stderr, "Unknown inst: `%d`\n", type); exit(0);
    }
}

InstList *parseFile(const char *path) {
    std::fstream fin(path, std::ios::in);
    if (!fin.is_open()) {
        fprintf(stderr, "Error: NEL file `%s` not exist!\n", path);
        exit(0);
    }
    auto *list = new std::vector<Inst*>();
    std::string temp;
    int index = 1;
    while (getline(fin, temp)) {
        if (isEmpty(temp)) continue;
        list->push_back(parseLine(index++, temp));
    }
    fin.close();
    return list;
}

Inst *parseLine(int index, std::string &line) {
    std::string origin(line);
    line.erase(0, line.find_first_not_of(' '));
    line.erase(line.find_last_not_of(' ') + 1);
    try {
        InstType type;
        int type_index = line.find(',');
        std::string type_str = line.substr(0, type_index);
        switch (hash_(type_str)) {
            case  "ADD"_hash: type = InstType::ADD ; break;
            case  "SUB"_hash: type = InstType::SUB ; break;
            case  "MUL"_hash: type = InstType::MUL ; break;
            case  "DIV"_hash: type = InstType::DIV ; break;
            case   "LD"_hash: type = InstType::LOAD; break;
            case "JUMP"_hash: type = InstType::JUMP; break;
            default: fprintf(stderr, "Unknown inst: `%s`\n", line.substr(0, type_index).c_str()); exit(0);
        }
        if (type == InstType::LOAD) {
            int op1_index = line.find(',', type_index + 1);
            int op1 = dec2int(line.substr(type_index + 2, op1_index - type_index - 2));
            int op2 = hex2int(line.substr(op1_index + 1));
            return new Inst{index, origin, type, op1, op2, 0};
        } else if (type == InstType::JUMP) {
            int op1_index = line.find(',', type_index + 1);
            int op2_index = line.find(',', op1_index + 1);
            int op1 = hex2int(line.substr(type_index + 1, op1_index - type_index - 1));
            int op2 = dec2int(line.substr(op1_index + 2, op2_index - op1_index - 2));
            int op3 = hex2int(line.substr(op2_index + 1));
            return new Inst{index, origin, type, op1, op2, op3};
        } else {
            int op1_index = line.find(',', type_index + 1);
            int op2_index = line.find(',', op1_index + 1);
            int op1 = dec2int(line.substr(type_index + 2, op1_index - type_index - 2));
            int op2 = dec2int(line.substr(op1_index + 2, op2_index - op1_index - 2));
            int op3 = dec2int(line.substr(op2_index + 2));
            return new Inst{index, origin, type, op1, op2, op3};
        }
    } catch (std::exception &e) {
        fprintf(stderr, "Unknown inst format\n");
        fprintf(stderr, "%s\n", e.what());
        exit(0);
    }
}

void printOutput(InstList *list, const char *path) {
    if (strlen(path) == 0) {
        for (auto &item : *list) {
            printf("%d %d %d\n", item->issue, item->finish, item->write);
        }
    } else {
        std::fstream fout(path, std::ios::out);
        if (!fout.is_open()) {
            fprintf(stderr, "Error: Open `%s` failed!\n", path);
            exit(0);
        }
        char buf[64];
        for (auto &item : *list) {
            sprintf(buf, "%d %d %d\n", item->issue, item->finish, item->write);
            fout.write(buf, strlen(buf));
        }
        fout.close();
    }
}

const char *int2str(int n) {
    char *buf = new char[8];
    sprintf(buf, "%d", n);
    return buf;
}
