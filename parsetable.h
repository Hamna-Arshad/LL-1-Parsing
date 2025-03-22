#ifndef PARSETABLE_H
#define PARSETABLE_H

#include <iostream>
#include <unordered_map>
#include <vector>
#include <set>
#include <iomanip>
using namespace std;

// Function to construct the LL(1) parsing table
unordered_map<string, unordered_map<string, vector<string>>> constructLL1Table(
    unordered_map<string, vector<vector<string>>> &cfg,
    unordered_map<string, vector<vector<string>>> &first_sets,
    unordered_map<string, vector<vector<string>>> &follow_sets)
{
    unordered_map<string, unordered_map<string, vector<string>>> parsingTable;
    set<string> terminals;

    // Collect all terminals
    for (auto &rule : cfg)
    {
        for (auto &production : rule.second)
        {
            for (const string &symbol : production)
            {
                if (cfg.find(symbol) == cfg.end() && symbol != "null")
                {
                    terminals.insert(symbol);
                }
            }
        }
    }
    terminals.insert("$"); // End of input marker

    for (auto &rule : cfg)
    {
        string nonTerminal = rule.first;
        for (auto &production : rule.second)
        {
            set<string> first_beta;
            bool epsilonInFirst = true;

            // Compute FIRST(β)
            for (const string &symbol : production)
            {
                if (cfg.find(symbol) == cfg.end()) // Terminal
                {
                    first_beta.insert(symbol);
                    epsilonInFirst = false;
                    break;
                }
                else // Non-terminal
                {
                    for (const auto &f : first_sets[symbol])
                    {
                        if (!f.empty() && f[0] != "null")
                            first_beta.insert(f[0]);
                    }
                    if (find(first_sets[symbol].begin(), first_sets[symbol].end(), vector<string>{"null"}) == first_sets[symbol].end())
                    {
                        epsilonInFirst = false;
                        break;
                    }
                }
            }

            // Case 1: Add production to table for each terminal in FIRST(β)
            for (const string &terminal : first_beta)
            {
                parsingTable[nonTerminal][terminal] = production;
            }

            // Case 2: If FIRST(β) contains "null", add production to table for each terminal in FOLLOW(A)
            if (epsilonInFirst)
            {
                for (const auto &f : follow_sets[nonTerminal])
                {
                    if (!f.empty())
                        parsingTable[nonTerminal][f[0]] = production;
                }
            }
        }
    }
    return parsingTable;
}



#endif