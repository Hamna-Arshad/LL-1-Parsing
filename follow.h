#ifndef FOLLOW_H
#define FOLLOW_H

#include <iostream>
#include <unordered_map>
#include <vector>
#include <string>
#include <algorithm> 
using namespace std;

unordered_map<string, vector<vector<string>>> follow(unordered_map<string, vector<vector<string>>> cfg,unordered_map<string, vector<vector<string>>> first_sets)
{
    unordered_map<string, vector<vector<string>>> followSet;
    
    // Initialize empty follow
    for (auto &lhsRule : cfg)
    {
        followSet[lhsRule.first] = {};
    }

    // $ to the start
    string start_symbol = cfg.begin()->first;
    followSet[start_symbol].push_back({"$"});

    bool changed = true;
    while (changed)
    {
        changed = false;
        
        for (auto &lhsRule : cfg)
        {
            string A = lhsRule.first; 
            
            for (auto &production : lhsRule.second)
            {
                for (size_t i = 0; i < production.size(); i++)
                {
                    string B = production[i];
                    //cout<<"Processing " << B << endl;

                    if (cfg.find(B) != cfg.end()) // B is NT
                    {
                        // Case 1: A → α B β  (Add First(β) - {ε} to Follow(B))
                        if (i + 1 < production.size())
                        {
                            string next_symbol = production[i + 1];

                            if (cfg.find(next_symbol) != cfg.end()) // next_symbol is a non-terminal
                            {
                                for (const auto &first_term_list : first_sets[next_symbol])
                                {
                                    for (const string &first_term : first_term_list)
                                    {
                                        if (first_term != "null")
                                        {
                                            bool exists = false;
                                            for (const auto &follow_term_list : followSet[B])
                                            {
                                                if (find(follow_term_list.begin(), follow_term_list.end(), first_term) != follow_term_list.end())
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
                                    }
                                }
                            }
                            else // next_symbol is a terminal
                            {
                                bool exists = false;
                                for (const auto &follow_term_list : followSet[B])
                                {
                                    if (find(follow_term_list.begin(), follow_term_list.end(), next_symbol) != follow_term_list.end())
                                    {
                                        exists = true;
                                        break;
                                    }
                                }
                                if (!exists)
                                {
                                    followSet[B].push_back({next_symbol});
                                    changed = true;
                                }
                            }
                        }

                        // Case 2: A → α B (or if β contains ε, add Follow(A) to Follow(B))
                        if (i + 1 == production.size() || 
                            any_of(first_sets[production[i + 1]].begin(), first_sets[production[i + 1]].end(),
                                   [](const vector<string> &v) { return find(v.begin(), v.end(), "null") != v.end(); }))
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
