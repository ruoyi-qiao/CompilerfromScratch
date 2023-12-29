#pragma once

#include <unordered_map>
#include <string>
#include "../Calc/Id.hpp"

class Env {
private:
    std::unordered_map<Word, Id> table;
    Env* prev;

public:
    Env(Env* n) : prev(n) {}

    void put(const Word& w, const Id& i) {
        table[w] = i;
    }

    Id* get(const Word& w) {
        for (Env* e = this; e != nullptr; e = e->prev) {
            auto it = e->table.find(w);
            if (it != e->table.end()) {
                return &(it->second);
            }
        }
        return nullptr;
    }
    
    std::map<Word, Id> getEnv() {
        std::map<Word, Id> res;
        for (auto& it : table) {
            res[it.first] = it.second;
        }
        return res;
    }

    void print() {
        for (auto& it : table) {
            std::cout << it.first.toString() << " " << it.second.toString() << std::endl;
        }
    }
};