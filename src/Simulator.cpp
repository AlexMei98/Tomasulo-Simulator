//
// Created by Randy Mei on 2020/5/10.
//

#include "Tomasulo.hpp"
#include "Util.hpp"

const char *ID = "2016011335";
const char *inputPath = "/home/meirh/git/Tomasulo/testcase";
const char *logPath = "/home/meirh/git/Tomasulo/log";
const char **testcases = new const char *[11]{
    "0.basic",
    "1.basic",
    "2.basic",
    "3.basic",
    "4.basic",
    "Big_test",
    "Example",
    "Fabo",
    "Fact",
    "Gcd",
    "Mul",
};

int main(int argc, char **argv) {
    try {
        if (argc == 1) {
            char in[128], out[128];
            for (int i = 0; i < 11; i++) {
                sprintf(in, "%s/%s.nel", inputPath, testcases[i]);
                sprintf(out, "%s/%s_%s.log", logPath, ID, testcases[i]);
                auto *sim = new Tomasulo(in);
                sim->runAll(out, false);
            }
        } else {
            auto *sim = new Tomasulo(argv[1]);
            sim->runAll(argv[2], argc == 4);
        }
    } catch (std::exception &e) {
        printf("%s\n", e.what());
        printf("Usage: ./Tomasulo [input_path] [log_path] <step-by-step>\n");
        exit(0);
    }
    return 0;
}
