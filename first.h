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

#endif
