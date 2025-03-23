#ifndef F_H
#define F_H
#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <string>
#include <algorithm> // for std::find
using namespace std;

unordered_map<string, vector<vector<string>>> first(unordered_map<string, vector<vector<string>>> cfg)
{
    unordered_map<string, vector<vector<string>>> first_list;
    unordered_set<string> non_terminals;

    // Collect all non-terminals
    for (auto it = cfg.begin(); it != cfg.end(); ++it)
    {
        non_terminals.insert(it->first);
    }

    // Initialize first_list with the first symbol of each production
    for (auto it = cfg.begin(); it != cfg.end(); ++it)
    {
        string current_nonterminal = it->first;
        vector<vector<string>> current_rules = it->second;
        
        for (int i = 0; i < current_rules.size(); i++)
        {
            if (!current_rules[i].empty()) {
                first_list[current_nonterminal].push_back({ current_rules[i][0] });
            }
        }
    }

    bool changed = true;
    while (changed) {
        changed = false;
        
        // For each non-terminal
        for (auto it = first_list.begin(); it != first_list.end(); ++it)
        {
            string non_terminal = it->first;
            vector<vector<string>> current_productions = it->second;
            vector<vector<string>> new_productions;
            
            // For each production of the non-terminal
            for (int i = 0; i < current_productions.size(); i++)
            {
                vector<string> prod = current_productions[i];
                
                // If this production has a non-terminal as its first symbol
                if (prod.size() == 1 && non_terminals.find(prod[0]) != non_terminals.end()) {
                    bool added = false;
                    
                    // Add all productions from the non-terminal's FIRST set
                    if (first_list.find(prod[0]) != first_list.end()) {
                        for (const auto& first_prod : first_list[prod[0]]) {
                            // Check if this production is already in new_productions
                            bool already_exists = false;
                            for (const auto& existing_prod : new_productions) {
                                if (existing_prod == first_prod) {
                                    already_exists = true;
                                    break;
                                }
                            }
                            
                            if (!already_exists) {
                                new_productions.push_back(first_prod);
                                added = true;
                                changed = true;
                            }
                        }
                    }
                    
                    // If nothing was added, keep the original production
                    if (!added) {
                        new_productions.push_back(prod);
                    }
                } else {
                    // Keep terminals and other symbols as is
                    new_productions.push_back(prod);
                }
            }
            
            // Process productions with multiple symbols for FIRST propagation
            for (auto rule_it = cfg[non_terminal].begin(); rule_it != cfg[non_terminal].end(); ++rule_it) {
                const vector<string>& rule = *rule_it;
                
                if (rule.size() >= 2) {  // Rules with at least 2 symbols
                    // Process each symbol in the rule
                    for (size_t i = 0; i < rule.size() - 1; i++) {
                        string current = rule[i];
                        
                        // Only process if it's a non-terminal
                        if (non_terminals.find(current) != non_terminals.end()) {
                            // Check if this non-terminal can derive null
                            bool has_null = false;
                            for (const auto& first_prod : first_list[current]) {
                                if (first_prod.size() == 1 && first_prod[0] == "null") {
                                    has_null = true;
                                    break;
                                }
                            }
                            
                            // If this non-terminal can derive null, add FIRST(next) - {null}
                            if (has_null) {
                                string next = rule[i + 1];
                                
                                if (non_terminals.find(next) != non_terminals.end()) {
                                    // Next is a non-terminal
                                    for (const auto& next_first : first_list[next]) {
                                        if (!(next_first.size() == 1 && next_first[0] == "null")) {
                                            // Add to new_productions if not already there
                                            bool already_exists = false;
                                            for (const auto& existing : new_productions) {
                                                if (existing == next_first) {
                                                    already_exists = true;
                                                    break;
                                                }
                                            }
                                            
                                            if (!already_exists) {
                                                new_productions.push_back(next_first);
                                                changed = true;
                                            }
                                        }
                                    }
                                } else if (next != "null") {
                                    // Next is a terminal
                                    vector<string> term_prod = {next};
                                    
                                    // Check if already exists
                                    bool already_exists = false;
                                    for (const auto& existing : new_productions) {
                                        if (existing == term_prod) {
                                            already_exists = true;
                                            break;
                                        }
                                    }
                                    
                                    if (!already_exists) {
                                        new_productions.push_back(term_prod);
                                        changed = true;
                                    }
                                }
                            } else {
                                // If this symbol cannot derive null, don't process further
                                break;
                            }
                        } else if (current != "null") {
                            // Terminal that's not null, don't process further
                            break;
                        }
                    }
                    
                    // Check if ALL symbols in the rule can derive null
                    bool all_derive_null = true;
                    for (const string& sym : rule) {
                        if (non_terminals.find(sym) != non_terminals.end()) {
                            // Check if this non-terminal has null in its FIRST set
                            bool has_null = false;
                            for (const auto& first_prod : first_list[sym]) {
                                if (first_prod.size() == 1 && first_prod[0] == "null") {
                                    has_null = true;
                                    break;
                                }
                            }
                            
                            if (!has_null) {
                                all_derive_null = false;
                                break;
                            }
                        } else if (sym != "null") {
                            // Terminal that's not null
                            all_derive_null = false;
                            break;
                        }
                    }
                    
                    // CRITICAL FIX: Only add null if ALL symbols can derive null
                    if (all_derive_null) {
                        vector<string> null_prod = {"null"};
                        
                        // Check if null is already in new_productions
                        bool null_exists = false;
                        for (const auto& existing : new_productions) {
                            if (existing.size() == 1 && existing[0] == "null") {
                                null_exists = true;
                                break;
                            }
                        }
                        
                        if (!null_exists) {
                            new_productions.push_back(null_prod);
                            changed = true;
                        }
                    } else {
                        // IMPORTANT: Remove null from new_productions if it exists
                        // and not all symbols can derive null
                        for (auto it = new_productions.begin(); it != new_productions.end();) {
                            if (it->size() == 1 && (*it)[0] == "null") {
                                it = new_productions.erase(it);
                                changed = true;
                            } else {
                                ++it;
                            }
                        }
                    }
                }
            }
            
            // Update the productions for this non-terminal
            it->second = new_productions;
        }
    }

    return first_list;
}
#endif

/*#ifndef F_H
#define F_H
#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <string>
#include <algorithm> // for std::find
using namespace std;

unordered_map<string, vector<vector<string>>> first(unordered_map<string, vector<vector<string>>> cfg)
{
    unordered_map<string, vector<vector<string>>> first_list;
    unordered_set<string> non_terminals;

    for (auto it = cfg.begin(); it != cfg.end(); ++it)
    {
        non_terminals.insert(it->first); // store all the nonterminals
    }

    // Stores the prefix of all the production rules for each nonterminal
    for (auto it = cfg.begin(); it != cfg.end(); ++it)
    {
        string current_nonterminal = it->first;
        vector<vector<string>> current_rules = it->second;
        cout << "Prefix: \n";
        for (int i = 0; i < current_rules.size(); i++)
        {
            cout << current_rules[i][0] << ", ";
            first_list[current_nonterminal].push_back({ current_rules[i][0] });
        }
        cout << endl;
    }

    // if a token is a nonterminal,replace that production with the productions from that nonterminal.
    for (auto it = first_list.begin(); it != first_list.end(); ++it)
    {
        string non_terminal = it->first;
        vector<vector<string>> new_productions; // will store the updated productions
        for (int i = 0; i < it->second.size(); i++)
        {
            vector<string> prod = it->second[i];
            bool replaced = false;

            for (int j = 0; j < prod.size(); j++)
            {
                if (first_list.find(prod[j]) != first_list.end())
                {
                    // Replace this production with all productions from first_list[token]
                    vector<vector<string>> replacement = first_list[prod[j]];
                    for (int k = 0; k < replacement.size(); k++)
                    {
                        new_productions.push_back(replacement[k]);
                    }
                    replaced = true;
                    break; 
                }
            }
            
            if (!replaced)
            {
                new_productions.push_back(prod);
            }
        }
        // Update the productions for this nonterminal
        it->second = new_productions;
    }

    return first_list;
}

#endif*/
