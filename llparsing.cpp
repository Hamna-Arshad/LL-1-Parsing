#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <sstream>
#include "LF.h"
#include "LR.h"
#include "first.h"
#include "follow.h"
#include "parsetable.h"
using namespace std;
#define VERBOSE 0

void PrintCFG(unordered_map<string, vector<vector<string>>> cfg);
unordered_map<string, vector<vector<string>>> ReadFile(string filename);
void PrintFirstFollow(unordered_map<string, vector<vector<string>>> first_list);
void printLL1Table(const unordered_map<string, unordered_map<string, vector<string>>> &parsingTable,
    const set<string> &terminals);
int main()
{

    string filename = "cfg.txt";
    unordered_map<string, vector<vector<string>>> cfg = ReadFile(filename);
    cout << "----------------------------------------" << endl;
    cout << "Original ";
    PrintCFG(cfg);

    unordered_map<string, vector<vector<string>>> left_factored_cfg = left_factor(cfg);
    cout << "----------------------------------------" << endl;
    cout << "Left Factored ";
    PrintCFG(left_factored_cfg);

    unordered_map<string, vector<vector<string>>> noRecursion_cfg = LR(left_factored_cfg);
    cout << "----------------------------------------" << endl;
    cout << "No Left Recursion ";
    PrintCFG(noRecursion_cfg);

    unordered_map<string, vector<vector<string>>> first_list = first(noRecursion_cfg);
    cout << "----------------------------------------" << endl;
    cout << "First List \n";
    PrintFirstFollow(first_list);

    unordered_map<string, vector<vector<string>>> follow_sets = follow(noRecursion_cfg, first_list);
    cout << "----------------------------------------" << endl;
    cout << "Follow List \n";
    PrintFirstFollow(follow_sets);

    unordered_map<string, unordered_map<string, vector<string>>> parsingTable = constructLL1Table(cfg, first_list, follow_sets);
    cout << "------------------------------------------------------" << endl;
    cout << "LL(1) Parsing Table \n";
    set<string> terminals;
    for (auto &rule : cfg)
    {
        for (auto &production : rule.second)
        {
            for (const string &symbol : production)
            {
                if (cfg.find(symbol) == cfg.end() && symbol != "ε")
                {
                    terminals.insert(symbol);
                }
            }
        }
    }
    terminals.insert("$");
    printLL1Table(parsingTable, terminals);

    return 0;
}

void PrintFirstFollow(unordered_map<string, vector<vector<string>>> first_list)
{
    for (auto it = first_list.begin(); it != first_list.end(); ++it)
    {
        cout << it->first << ":\t{ ";
        for (int i = 0; i < it->second.size(); i++)
        {
            // Print each token in the i-th production
            for (int j = 0; j < it->second[i].size(); j++)
            {
                cout << it->second[i][j];
            }
            // Print a production separator if it's not the last production
            if (i < it->second.size() - 1)
            {
                cout << " , ";
            }
        }
        cout << " }" << endl;
    }
    cout << endl;
}

void PrintCFG(unordered_map<string, vector<vector<string>>> cfg)
{
    cout << "CFG: " << endl;

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
    cout << endl;
}
unordered_map<string, vector<vector<string>>> ReadFile(string filename)
{
    fstream file(filename);
    string line;
    string nonterminal, arrow, token;
    unordered_map<string, vector<vector<string>>> cfg;

    while (getline(file, line))
    {
        stringstream ss(line);
        ss >> nonterminal >> arrow >> ws; // reads the arrow and whitespaces afterward to remove them from buffer
        vector<string> rule;

        while (ss >> token)
        {
            if (VERBOSE)
                cout << "Token: " << token << endl;
            if (token == "|")
            {
                cfg[nonterminal].push_back(rule);
                rule.clear();
            }
            else
            {
                rule.push_back(token);
            }
        }
        if (!rule.empty())
        {
            cfg[nonterminal].push_back(rule);
        }
    }
    file.close();
    return cfg;
}
void printLL1Table(const unordered_map<string, unordered_map<string, vector<string>>> &parsingTable,
                   const set<string> &terminals)
{

    // Print column headers (terminals)
    cout << setw(15) << " ";
    for (const string &terminal : terminals)
    {
        cout << setw(15) << terminal;
    }
    cout << endl;

    // Print rows (non-terminals and their productions)
    for (const auto &row : parsingTable)
    {
        string nonTerminal = row.first;
        cout << setw(15) << nonTerminal;

        for (const string &terminal : terminals)
        {
            if (row.second.find(terminal) != row.second.end())
            {
                // Print the production
                string production;
                for (const string &symbol : row.second.at(terminal))
                {
                    production += symbol + " ";
                }
                cout << setw(15) << production;
            }
            else
            {
                // Print empty cell
                cout << setw(15) << " ";
            }
        }
        cout << endl;
    }
}