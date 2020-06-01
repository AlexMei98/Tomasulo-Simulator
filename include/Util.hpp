//
// Created by Randy Mei on 2020/5/10.
//

#ifndef TOMASULO_UTIL_HPP
#define TOMASULO_UTIL_HPP

#include <string>
#include <cstdint>

#include "Inst.hpp"

typedef uint64_t hash_t;

constexpr hash_t prime = 0x100000001B3ull;
constexpr hash_t basis = 0xCBF29CE484222325ull;

constexpr hash_t hashInstType(char const *str, hash_t last_value = basis);

constexpr unsigned long long operator "" _hash(char const *p, hash_t);

hash_t hash_(const std::string &str);

int dec2int(const std::string &str);

int hex2int(const std::string &str);

bool isEmpty(const std::string &str);

const char *type2str(InstType type);

InstList *parseFile(const char *path);

Inst *parseLine(int index, std::string &str);

void printOutput(InstList *list, const char *path = "");

const char *int2str(int n);

#endif //TOMASULO_UTIL_HPP
