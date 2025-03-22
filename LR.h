#ifndef LR_h
#define LR_h
#include <unordered_map>
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <algorithm>


using namespace std;
#define VERBOSE1 0
void myPrintCFG(unordered_map<string, vector<vector<string>>> cfg);
void PrintAlpha(vector<vector<string>> alpha);
void PrintBeta(vector<vector<string>> beta);
string GetValidNewRule(string lhsRule, unordered_map<string, vector<vector<string>>> cfg);
bool HasNull(vector<vector<string>> rule);
void RemoveNull(vector<vector<string>> *rule);
unordered_map<string, vector<vector<string>>> Remove_Direct_LR(unordered_map<string, vector<vector<string>>> cfg, string lhs);
unordered_map<string, vector<vector<string>>> Remove_LR(unordered_map<string, vector<vector<string>>> cfg);


/*
2. Left Recursion Removal: Eliminate left recursion from the grammar to avoid infi-
nite recursion during top-down parsing. For a production of the form:

A → Aα | β,

it should be rewritten as:

A → βA′
A′ → αA′ | ε
*/

unordered_map<string, vector<vector<string>>> LR(unordered_map<string, vector<vector<string>>> cfg)
{
    bool flag = false;
    vector<vector<string>> alpha;
    vector<vector<string>> beta;
    vector<string> temp;
    // create new rule
    string newRule;

    for (auto &lhsRule : cfg)
    {
        for (auto &production : lhsRule.second)
        {
            newRule = GetValidNewRule(lhsRule.first, cfg);

            // cout<<production[0]<<endl;
            if (production[0] == lhsRule.first)
            {
                flag = true;
                if (VERBOSE1)
                    cout << "\n"
                         << lhsRule.first << " has LR " << endl;
                // find α and make a copy of it
                for (int i = 1; i < production.size(); i++)
                {
                    temp.push_back(production[i]);
                }
                // add A' to α
                temp.push_back(newRule);
                // add to alpha
                alpha.push_back(temp);
                temp.clear();
            }
        }
        // find β
        if (flag)
        {

            for (auto &production : lhsRule.second)
            {
                if (production[0] != lhsRule.first)
                {
                    // cout<<production[0]<<endl;
                    for (int i = 0; i < production.size(); i++)
                    {
                        temp.push_back(production[i]);
                    }
                    // add A' to β
                    temp.push_back(newRule);
                    beta.push_back(temp);
                    temp.clear();
                }
            }
            // add ε to alpha
            // alpha.push_back({"ε"});
            if (!HasNull(alpha))
                alpha.push_back({"null"});

            if (HasNull(beta))
                RemoveNull(&beta);

            flag = false;
            // print alpha and beta
            if (VERBOSE1)
            {
                PrintAlpha(alpha);
                PrintBeta(beta);
                cout << "New rule: " << newRule << endl;
            }
            // add new rule to cfg
            cfg[newRule] = alpha;
            cfg[lhsRule.first] = beta;
            alpha.clear();
            beta.clear();
        }
    }
    if (VERBOSE1)
        // print the new cfg
        myPrintCFG(cfg);
    return cfg;
}

unordered_map<string, vector<vector<string>>> Remove_All_LR(unordered_map<string, vector<vector<string>>> cfg)
{
    // Get all non-terminals in a fixed order
    vector<string> nonTerminals;
    for (auto &rule : cfg) {
        nonTerminals.push_back(rule.first);
    }
    
    // Sort the non-terminals to ensure a consistent ordering
    sort(nonTerminals.begin(), nonTerminals.end());
    
    // Step 1: Eliminate indirect left recursion
    for (int i = 0; i < nonTerminals.size(); i++) {
        string Ai = nonTerminals[i];
        
        // For each previous non-terminal Aj (j < i)
        for (int j = 0; j < i; j++) {
            string Aj = nonTerminals[j];
            
            // Replace each production Ai -> Aj γ with Ai -> δ1 γ | δ2 γ | ... | δn γ
            // where Aj -> δ1 | δ2 | ... | δn are all productions for Aj
            vector<vector<string>> newProductions;
            
            for (auto &production : cfg[Ai]) {
                if (production.size() > 0 && production[0] == Aj) {
                    // This production starts with Aj, replace it
                    vector<string> gamma(production.begin() + 1, production.end());
                    
                    // For each production of Aj
                    for (auto &ajProduction : cfg[Aj]) {
                        vector<string> newProduction = ajProduction;
                        // Append gamma
                        newProduction.insert(newProduction.end(), gamma.begin(), gamma.end());
                        newProductions.push_back(newProduction);
                    }
                } else {
                    // Keep the production as is
                    newProductions.push_back(production);
                }
            }
            
            // Replace the productions for Ai
            cfg[Ai] = newProductions;
        }
        
        // Step 2: Eliminate direct left recursion for Ai
        cfg = Remove_Direct_LR(cfg, Ai);
    }
    
    return cfg;
}

// This function handles direct left recursion for a specific non-terminal
unordered_map<string, vector<vector<string>>> Remove_Direct_LR(unordered_map<string, vector<vector<string>>> cfg, string lhs)
{
    bool flag = false;
    vector<vector<string>> alpha;
    vector<vector<string>> beta;
    vector<string> temp;
    // create new rule
    string newRule = GetValidNewRule(lhs, cfg);

    for (auto &production : cfg[lhs])
    {
        // cout<<production[0]<<endl;
        if (production.size() > 0 && production[0] == lhs)
        {
            flag = true;
            if (VERBOSE1)
                cout << "\n" << lhs << " has direct LR " << endl;
            // find α and make a copy of it
            for (int i = 1; i < production.size(); i++)
            {
                temp.push_back(production[i]);
            }
            // add A' to α
            temp.push_back(newRule);
            // add to alpha
            alpha.push_back(temp);
            temp.clear();
        }
    }
    
    // find β
    if (flag)
    {
        for (auto &production : cfg[lhs])
        {
            if (production.size() == 0 || production[0] != lhs)
            {
                // cout<<production[0]<<endl;
                for (int i = 0; i < production.size(); i++)
                {
                    temp.push_back(production[i]);
                }
                // add A' to β
                temp.push_back(newRule);
                beta.push_back(temp);
                temp.clear();
            }
        }
        // add ε to alpha
        if (!HasNull(alpha))
            alpha.push_back({"null"});

        if (HasNull(beta))
            RemoveNull(&beta);

        // print alpha and beta
        if (VERBOSE1)
        {
            PrintAlpha(alpha);
            PrintBeta(beta);
            cout << "New rule: " << newRule << endl;
        }
        // add new rule to cfg
        cfg[newRule] = alpha;
        cfg[lhs] = beta;
    }
    
    return cfg;
}

unordered_map<string, vector<vector<string>>> Remove_LR(unordered_map<string, vector<vector<string>>> cfg)
{
    return Remove_All_LR(cfg);
}

void myPrintCFG(unordered_map<string, vector<vector<string>>> cfg)
{
    cout << "cfg: " << endl;
    for (auto it = cfg.begin(); it != cfg.end(); ++it)
    {
        cout << it->first << " ->";

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
}
void PrintAlpha(vector<vector<string>> alpha)
{
    cout << "alpha: ";
    for (auto &lhsRule : alpha)
    {
        for (int i = 0; i < lhsRule.size(); i++)
        {
            cout << lhsRule[i] << " ";
        }
        cout << " , ";
    }
}
void PrintBeta(vector<vector<string>> beta)
{
    cout << "beta: ";
    for (auto &lhsRule : beta)
    {
        for (int i = 0; i < lhsRule.size(); i++)
        {
            cout << lhsRule[i] << " ";
        }
        cout << " , ";
    }
}

string GetValidNewRule(string lhsRule, unordered_map<string, vector<vector<string>>> cfg)
{
    string newRule = lhsRule + "'";
    // check if new rule already exists
    while (cfg.find(newRule) != cfg.end())
    {
        newRule = lhsRule + "'" + "'";
    }
    return newRule;
}
bool HasNull(vector<vector<string>> rule)
{
    for (auto &prod : rule)
    {
        if (prod[0] == "null")
        {
            return true;
        }
    }

    return false;
}
void RemoveNull(vector<vector<string>> *rule)
{
    for (auto &prod : *rule)
    {
        if (prod[0] == "null")
        {
            prod.erase(prod.begin());
        }
    }
}
#endif