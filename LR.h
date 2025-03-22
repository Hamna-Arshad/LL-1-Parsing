#ifndef LR_h
#define LR_h
#include <unordered_map>
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
using namespace std;

unordered_map<string, vector<vector<string>>> cfgg{
    {"E", {{"E", "+", "T"}, {"T"}}},
    {"T", {{"T", "*", "F"}, {"F"}}},
    {"F", {{"(", "E", ")"}, {"id"}, {"F"}}}};

/*

E -> E + T | T
T -> T * F | F
F -> ( E ) | id

2. Left Recursion Removal: Eliminate left recursion from the grammar to avoid infi-
nite recursion during top-down parsing. For a production of the form:

A → Aα | β,

it should be rewritten as:

A → βA′
A′ → αA′ | ε

*/
void PrintCFG(unordered_map<string, vector<vector<string>>> cfg)
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
unordered_map<string, vector<vector<string>>> Remove_LR(unordered_map<string, vector<vector<string>>> cfg)
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
            newRule = lhsRule.first + "'";
            // cout<<production[0]<<endl;
            if (production[0] == lhsRule.first)
            {
                flag = true;

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
                    beta.push_back(temp);
                    temp.clear();
                }
            }
            // add ε to alpha
            alpha.push_back({"ε"});

            flag = false;
            // print alpha and beta
            PrintAlpha(alpha);
            PrintBeta(beta);

            cout << "New rule: " << newRule << endl;
            // add new rule to cfg
            cfg[newRule] = alpha;
            cfg[lhsRule.first] = beta;
            alpha.clear();
            beta.clear();
        }
    }
    // print the new cfg
    PrintCFG(cfg);
    return cfg;
}

// E -> E + T | T
/*
#include "LR.h"
unordered_map <string, vector<vector<string>>> cfg{
    {"E", {{"E", "+", "T"}, {"T"}}},
    {"T", {{"T", "*", "F"}, {"F"}}},
    {"else", {{"T", "*", "F"}, {"else"}}},
    {"F", {{"(", "E", ")"}, {"id"}, {"F"}}}
};
int main()
{
    Remove_LR(cfg);
}

*/
#endif
