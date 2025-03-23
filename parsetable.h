#ifndef PARSETABLE_H
#define PARSETABLE_H

#include <iostream>
#include <unordered_map>
#include <vector>
#include <set>
#include <iomanip>
#include <algorithm>
using namespace std;

// Forward declaration
void printParsingTable(const unordered_map<string, unordered_map<string, vector<string>>> &parsingTable, const set<string> &terminals, const set<string> &nonTerminals);

// Function to compute FIRST set of a production (sequence of symbols)
set<string> computeFirstOfProduction(const vector<string> &production,
                                     const unordered_map<string, vector<vector<string>>> &cfg,
                                     const unordered_map<string, vector<vector<string>>> &first_sets)
{
    set<string> result;
    bool allCanBeEmpty = true;

    for (const string &symbol : production)
    {
        if (symbol == "null")
        {
            result.insert("null");
            break;
        }

        if (cfg.find(symbol) == cfg.end())
        { // Terminal
            result.insert(symbol);
            allCanBeEmpty = false;
            break;
        }
        else
        { // Non-terminal
            bool containsEpsilon = false;
            for (const auto &firstSet : first_sets.at(symbol))
            {
                if (!firstSet.empty())
                {
                    if (firstSet[0] == "null")
                    {
                        containsEpsilon = true;
                    }
                    else
                    {
                        result.insert(firstSet[0]);
                    }
                }
            }

            if (!containsEpsilon)
            {
                allCanBeEmpty = false;
                break;
            }
        }
    }

    if (allCanBeEmpty)
    {
        result.insert("null");
    }

    return result;
}

// Function to construct the LL(1) parsing table
unordered_map<string, unordered_map<string, vector<string>>> constructLL1Table(
    unordered_map<string, vector<vector<string>>> &cfg,
    unordered_map<string, vector<vector<string>>> &first_sets,
    unordered_map<string, vector<vector<string>>> &follow_sets)
{
    unordered_map<string, unordered_map<string, vector<string>>> parsingTable;

    // Collect all terminals and non-terminals
    set<string> terminals;
    set<string> nonTerminals;

    // Add all non-terminals from the grammar
    for (const auto &rule : cfg)
    {
        nonTerminals.insert(rule.first);
    }

    // Find all terminals from the grammar
    for (const auto &rule : cfg)
    {
        for (const auto &production : rule.second)
        {
            for (const string &symbol : production)
            {
                if (nonTerminals.find(symbol) == nonTerminals.end() && symbol != "null")
                {
                    terminals.insert(symbol);
                }
            }
        }
    }
    terminals.insert("$"); // End of input marker

    cout << "Terminals: ";
    for (const auto &terminal : terminals)
    {
        cout << terminal << " ";
    }
    cout << endl;

    cout << "Non-terminals: ";
    for (const auto &nonTerminal : nonTerminals)
    {
        cout << nonTerminal << " ";
    }
    cout << endl
         << endl;

    // Initialize the parsing table with empty entries
    for (const string &nonTerminal : nonTerminals)
    {
        for (const string &terminal : terminals)
        {
            parsingTable[nonTerminal][terminal] = vector<string>();
        }
    }

    // Construct the parsing table
    for (const auto &rule : cfg)
    {
        string nonTerminal = rule.first;

        for (size_t prodIndex = 0; prodIndex < rule.second.size(); prodIndex++)
        {
            const vector<string> &production = rule.second[prodIndex];

            // Calculate FIRST set of the production
            set<string> firstOfProduction = computeFirstOfProduction(production, cfg, first_sets);

            // Case 1: For each terminal in FIRST(production), add the production to the table
            for (const string &terminal : firstOfProduction)
            {
                if (terminal != "null")
                {
                    // Check for conflict
                    if (!parsingTable[nonTerminal][terminal].empty())
                    {
                        //cout << "WARNING: Conflict detected in parsing table at [" << nonTerminal<< ", " << terminal << "]!" << endl;
                    }
                    parsingTable[nonTerminal][terminal] = production;
                }
            }

            // Case 2: If FIRST(production) contains null, add the production for each symbol in FOLLOW(nonTerminal)
            if (firstOfProduction.find("null") != firstOfProduction.end())
            {
                for (const auto &followSet : follow_sets[nonTerminal])
                {
                    if (!followSet.empty())
                    {
                        string followSymbol = followSet[0];
                        // Check for conflict
                        if (!parsingTable[nonTerminal][followSymbol].empty())
                        {
                            /*cout << "WARNING: Conflict detected in parsing table at [" << nonTerminal
                                 << ", " << followSymbol << "]!" << endl;*/
                        }
                        parsingTable[nonTerminal][followSymbol] = production;
                    }
                }
            }
        }
    }

    // Print the parsing table in a nicely formatted way
    printParsingTable(parsingTable, terminals, nonTerminals);

    return parsingTable;
}

// Function to print a nicely formatted parsing table
void printParsingTable(const unordered_map<string, unordered_map<string, vector<string>>> &parsingTable,
                       const set<string> &terminals, const set<string> &nonTerminals)
{
    cout << "\nParsing Table Entries:" << endl;
    cout <<"[Non-terminal, Terminal] -> Production Rule" << endl;
    for (const auto &nonTerminalEntry : parsingTable)
    {
        const string &nonTerminal = nonTerminalEntry.first;
        for (const auto &terminalEntry : nonTerminalEntry.second)
        {
            const string &terminal = terminalEntry.first;
            const vector<string> &production = terminalEntry.second;

            if (!production.empty())
            {
                cout << "[" << nonTerminal << ", " << terminal << "] -> " << nonTerminal << " -> ";
                for (const string &symbol : production)
                {
                    cout << symbol << " ";
                }
                cout << endl;
            }
        }
    }
    cout << endl;

    cout << "\nLL(1) Parsing Table:" << endl;
    
    vector<string> terminalVec(terminals.begin(), terminals.end());
    vector<string> nonTerminalVec(nonTerminals.begin(), nonTerminals.end());
    
    // Sort terminals and non-terminals for consistent ordering
    sort(terminalVec.begin(), terminalVec.end());
    sort(nonTerminalVec.begin(), nonTerminalVec.end());
    
    // Calculate column width based on content
    const int colWidth = 15;
    const int leftColWidth = 12;
    
    // Print header row with terminals
    cout << left << setw(leftColWidth-4) << "";
    for (const auto &terminal : terminalVec) {
        cout << "|\t\t" << setw(colWidth - 10) << terminal << " ";
    }
    cout << "|" << endl;
    
    // Print separator line
    cout << string(leftColWidth-4, '-');
    for (size_t i = 0; i < terminalVec.size(); i++) {
        cout << string(colWidth-2, '-');
    }
    cout << endl;
    
    // Print table content
    for (const auto &nonTerminal : nonTerminalVec) {
        cout << left << setw(leftColWidth-4) << nonTerminal;
        
        for (const auto &terminal : terminalVec) {
            cout << "| ";
            
            // Check if there's an entry in the parsing table
            auto it1 = parsingTable.find(nonTerminal);
            if (it1 != parsingTable.end()) {
                auto it2 = it1->second.find(terminal);
                if (it2 != it1->second.end() && !it2->second.empty()) {
                    // Format the production
                    string prod = nonTerminal + "->";
                    for (const auto &symbol : it2->second) {
                        prod += symbol ;
                    }
                    
                    // Trim if too long for the column
                    if (prod.length() > colWidth - 3) {
                        prod = prod.substr(0, colWidth - 6) + "...";
                    }
                    
                    cout << setw(colWidth - 4) << left << prod;
                } else {
                    cout << setw(colWidth - 4) << "";
                }
            } else {
                cout << setw(colWidth - 4) << "";
            }
        }
        cout << "|" << endl;
    }
    
    // Print bottom separator line
    cout << string(leftColWidth, '-');
    for (size_t i = 0; i < terminalVec.size(); i++) {
        cout << string(colWidth-2, '-');
    }
}

#endif