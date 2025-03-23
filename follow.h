#ifndef FOLLOW_H
#define FOLLOW_H

#include <iostream>
#include <unordered_map>
#include <vector>
#include <string>
#include <algorithm>
using namespace std;

unordered_map<string, vector<vector<string>>> follow(
    unordered_map<string, vector<vector<string>>> cfg,
    unordered_map<string, vector<vector<string>>> first_sets)
{
    unordered_map<string, vector<vector<string>>> followSet;
    
    // Initialize empty follow set for each non-terminal
    for (auto &lhsRule : cfg)
    {
        followSet[lhsRule.first] = {};
    }

    // Add '$' to the start symbol's follow set
    string start_symbol = cfg.begin()->first;
    followSet[start_symbol].push_back({"$"});

    bool changed = true;
    while (changed)
    {
        changed = false;
        
        // For each production A -> α
        for (auto &lhsRule : cfg)
        {
            string A = lhsRule.first;
            
            for (auto &production : lhsRule.second)
            {
                // For each symbol in the production
                for (size_t i = 0; i < production.size(); i++)
                {
                    string B = production[i];
                    
                    // Only process non-terminals
                    if (cfg.find(B) != cfg.end())
                    {
                        // Compute FIRST(beta) where beta = production[i+1 ... end]
                        bool beta_nullable = true; // assume beta is nullable initially
                        for (size_t j = i + 1; j < production.size() && beta_nullable; j++)
                        {
                            beta_nullable = false; // will set true if FIRST of current symbol contains "null"
                            
                            string symbol = production[j];
                            // If symbol is a terminal, add it to FOLLOW(B) and break.
                            if (cfg.find(symbol) == cfg.end())
                            {
                                // Add terminal symbol directly to FOLLOW(B)
                                bool exists = false;
                                for (const auto &follow_B_list : followSet[B])
                                {
                                    if (find(follow_B_list.begin(), follow_B_list.end(), symbol) != follow_B_list.end())
                                    {
                                        exists = true;
                                        break;
                                    }
                                }
                                if (!exists)
                                {
                                    followSet[B].push_back({symbol});
                                    changed = true;
                                }
                                // Since terminal can't be nullable, we break.
                                beta_nullable = false;
                                break;
                            }
                            else // symbol is a non-terminal
                            {
                                // Add FIRST(symbol) - { "null" } to FOLLOW(B)
                                for (const auto &first_term_list : first_sets[symbol])
                                {
                                    for (const string &first_term : first_term_list)
                                    {
                                        if (first_term != "null")
                                        {
                                            bool exists = false;
                                            for (const auto &follow_B_list : followSet[B])
                                            {
                                                if (find(follow_B_list.begin(), follow_B_list.end(), first_term) != follow_B_list.end())
                                                {
                                                    exists = true;
                                                    break;
                                                }
                                            }
                                            if (!exists)
                                            {
                                                followSet[B].push_back({first_term});
                                                changed = true;
                                            }
                                        }
                                        else
                                        {
                                            // Found epsilon in FIRST(symbol), so the current symbol is nullable.
                                            beta_nullable = true;
                                        }
                                    }
                                }
                            }
                        }
                        
                        // If beta is completely nullable (or B is at the end), add FOLLOW(A) to FOLLOW(B)
                        if (i + 1 == production.size() || beta_nullable)
                        {
                            for (const auto &follow_A_list : followSet[A])
                            {
                                for (const string &follow_A : follow_A_list)
                                {
                                    bool exists = false;
                                    for (const auto &follow_B_list : followSet[B])
                                    {
                                        if (find(follow_B_list.begin(), follow_B_list.end(), follow_A) != follow_B_list.end())
                                        {
                                            exists = true;
                                            break;
                                        }
                                    }
                                    if (!exists)
                                    {
                                        followSet[B].push_back({follow_A});
                                        changed = true;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    return followSet;
}

#endif
