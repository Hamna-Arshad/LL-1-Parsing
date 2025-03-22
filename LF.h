#ifndef LF_H
#define LF_H
#include <unordered_map>
#include <unordered_set>
#include <vector>

using namespace std;

unordered_map<string, vector<vector<string>>> left_factor(unordered_map<string, vector<vector<string>>> cfg)
{
    bool needs_lf = false, completely_removed = true;
    unordered_set<string> duplicates, seen;
    vector<string> prefix_list;
    vector<vector<string>> rules_require_left_factoring, rules_not_requiring_left_factoring;
    unordered_map<string, vector<vector<string>>> left_factored_cfg;
    string new_nonterminal;

    do
   {
    completely_removed = true;
    left_factored_cfg.clear();        // stores the prefix of all the production rules for each nontermimal
        for (auto it = cfg.begin(); it != cfg.end(); ++it)
        {
            string current_nonterminal = it->first;
            vector<vector<string>> current_rules = it->second;

            for (int i = 0; i < it->second.size(); i++)
            {
                if (it->second[i][0] != "null")
                    prefix_list.push_back(it->second[i][0]); // store all the prefixes for each nonterminal
            }
            for (const string &s : prefix_list) // iterate through the prefixes and check for duplicates
            {
                if (seen.find(s) != seen.end())
                {
                    cout << "Duplicate found: " << s << endl;
                    duplicates.insert(s);
                    needs_lf = true;
                    completely_removed = false;
                }
                else 
                {
                    seen.insert(s);
                }
            }
            // preserve the rules that do not require left factoring
            if (!needs_lf)
            {
                left_factored_cfg[current_nonterminal] = current_rules;
            }
            else
            {
                // store the rules that require left factoring
                for (int i = 0; i < it->second.size(); i++)
                {
                    if (!it->second[i].empty() && duplicates.find(it->second[i][0]) != duplicates.end())
                    {
                        rules_require_left_factoring.push_back(it->second[i]);
                    }
                    else
                    {
                        rules_not_requiring_left_factoring.push_back(it->second[i]);
                    }
                }
                // perform lf
                if (!rules_require_left_factoring.empty())
                {
                    vector<string> new_rule;
                    // creates a new rule and adds to the cfg
                    new_nonterminal = current_nonterminal + "'";
                    vector<vector<string>> new_rules;
                    for (int i = 0; i < rules_require_left_factoring.size(); i++)
                    {
                        new_rule = rules_require_left_factoring[i];
                        if (new_rule.size() > 1)
                        {
                            new_rule.erase(new_rule.begin());
                        }
                        else
                        {
                            new_rule[0] = "null";
                        }
                        left_factored_cfg[new_nonterminal].push_back(new_rule);
                    }

                    // updates the previous rule with the new nonterminal
                    for (int i = 0; i < rules_not_requiring_left_factoring.size(); i++)
                    {
                        left_factored_cfg[current_nonterminal].push_back(rules_not_requiring_left_factoring[i]);
                    }
                    // update the new modified ' wala rule
                    left_factored_cfg[current_nonterminal].push_back({rules_require_left_factoring[0][0], new_nonterminal});
                }
            }
            // clean all the data structures
            rules_require_left_factoring.clear();
            rules_not_requiring_left_factoring.clear();
            duplicates.clear();
            prefix_list.clear();
            seen.clear();
            needs_lf = false;
        }
        cfg = left_factored_cfg;
        cout << endl
        << "Current CFG: " << endl
        << endl;
   for (auto it = left_factored_cfg.begin(); it != left_factored_cfg.end(); ++it)
   {
       cout << "Nonterminal: " << it->first << " ->";

       for (int i = 0; i < it->second.size(); i++)
       {
           cout << " ";

           for (int j = 0; j < it->second[i].size(); j++)
           {
               cout << it->second[i][j] << " ";
           }
           cout << "|";
       }
       cout << endl;
   }

    }while (completely_removed == false);
    //if (completely_removed)
        return left_factored_cfg;
}

#endif
