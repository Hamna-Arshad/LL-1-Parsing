#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <sstream>
#include <stack>
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
void parseInputStringUsingStack(const string &input, const unordered_map<string, unordered_map<string, vector<string>>> &parsingTable, const string &startSymbol);
string readInputString(string filename)
{
    ifstream file(filename);
    if (!file.is_open())
    {
        return ""; // Return empty string if file can't be opened
    }

    stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}
void parseInputStringUsingStack2(const string &input,
                                const unordered_map<string, unordered_map<string, vector<string>>> &parsingTable,
                                const string &startSymbol)
{
    cout << "\n======== PARSING INPUT ========\n";
    cout << "Input: " << input << endl;

    // Split input string into tokens (space-separated)
    vector<string> tokens;
    stringstream ss(input);
    string token;
    while (ss >> token)
    {
        tokens.push_back(token);
    }
    tokens.push_back("$"); // Add end marker

    // Initialize parsing stack
    stack<string> parseStack;
    parseStack.push("$");
    parseStack.push(startSymbol);

    // Track errors and recovery
    int errorCount = 0;
    vector<string> errorMessages;
    bool recoveredFromError = false;

    // Current token position
    size_t currentIndex = 0;

    cout << "\n--- Parsing Steps ---\n";
    cout << left << setw(30) << "Stack" << setw(30) << "Remaining Input" << setw(30) << "Action" << endl;
    cout << string(90, '-') << endl;

    while (!parseStack.empty() && currentIndex <= tokens.size())
    {
        string currentToken = (currentIndex < tokens.size()) ? tokens[currentIndex] : "$";
        string topOfStack = parseStack.top();

        // Display current state
        string stackStr = "";
        stack<string> tempStack = parseStack;
        vector<string> stackContents;
        while (!tempStack.empty())
        {
            stackContents.push_back(tempStack.top());
            tempStack.pop();
        }
        for (auto it = stackContents.rbegin(); it != stackContents.rend(); ++it)
        {
            stackStr += *it + " ";
        }

        string remainingInput = "";
        for (size_t i = currentIndex; i < tokens.size(); i++)
        {
            remainingInput += tokens[i] + " ";
        }

        cout << left << setw(30) << stackStr << setw(30) << remainingInput;

        // Case 1: Successful parse completion
        if (topOfStack == "$" && currentToken == "$")
        {
            cout << "Accept - Parsing Complete" << endl;
            break;
        }

        // Case 2: Terminal at stack top (not in parsing table)
        if (parsingTable.find(topOfStack) == parsingTable.end())
        {
            if (topOfStack == currentToken)
            {
                cout << "Match: '" << topOfStack << "'" << endl;
                parseStack.pop();
                currentIndex++;
                recoveredFromError = false;
            }
            else
            {
                // Terminal mismatch - syntax error
                string errorMsg = "Error: Expected '" + topOfStack + "', found '" + currentToken + "'";
                cout << errorMsg << endl;
                errorMessages.push_back(errorMsg);
                errorCount++;

                // Error recovery strategy: Skip the current token
                cout << left << setw(30) << "" << setw(30) << "" << "Recovery: Skip token '" << currentToken << "'" << endl;
                currentIndex++;
                recoveredFromError = true;
            }
            continue;
        }

        // Case 3: Non-terminal at stack top (in parsing table)
        auto nonTerminalEntry = parsingTable.find(topOfStack);
        auto productionEntry = nonTerminalEntry != parsingTable.end() ? nonTerminalEntry->second.find(currentToken) : nonTerminalEntry->second.end();

        if (nonTerminalEntry != parsingTable.end() && productionEntry != nonTerminalEntry->second.end())
        {
            // Production found in the parsing table
            vector<string> production = productionEntry->second;
            parseStack.pop();

            string productionStr = "";
            for (const auto &sym : production)
            {
                productionStr += sym + " ";
            }

            cout << "Expand: " << topOfStack << " -> " << productionStr << endl;

            // Push production in reverse order (except epsilon)
            if (!(production.size() == 1 && (production[0] == "epsilon" || production[0] == "ε")))
            {
                for (auto it = production.rbegin(); it != production.rend(); ++it)
                {
                    parseStack.push(*it);
                }
            }
            recoveredFromError = false;
        }
        else
        {
            // No production found - syntax error
            string errorMsg = "Error: No production for " + topOfStack + " on input '" + currentToken + "'";
            cout << errorMsg << endl;
            errorMessages.push_back(errorMsg);
            errorCount++;

            // Error recovery strategy: Pop the non-terminal and try to continue
            cout << left << setw(30) << "" << setw(30) << "" << "Recovery: Skip non-terminal '" << topOfStack << "'" << endl;
            parseStack.pop();

            // Simple error recovery strategy: skip the current token if it doesn't match any expected production
            cout << left << setw(30) << "" << setw(30) << "" << "Synchronizing: Skipping token '" << currentToken << "'" << endl;
            currentIndex++;

            recoveredFromError = true;
        }
    }

    // Print final results
    cout << "\n======== PARSING RESULT ========\n";
    if (errorCount == 0)
    {
        cout << "Parsing completed successfully with no errors!" << endl;
    }
    else
    {
        cout << "Parsing completed with " << errorCount << " error(s):" << endl;
        for (size_t i = 0; i < errorMessages.size(); ++i)
        {
            cout << (i + 1) << ". " << errorMessages[i] << endl;
        }
        cout << "\nParsing status: " << (recoveredFromError ? "Failed" : "Partially successful with recovery") << endl;
    }

    cout << "==============================\n";
}
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
    cout << "Left Recursion Removed ";
    PrintCFG(noRecursion_cfg);

    unordered_map<string, vector<vector<string>>> first_list = first(noRecursion_cfg);
    cout << "----------------------------------------" << endl;
    cout << "First List \n";
    PrintFirstFollow(first_list);

    unordered_map<string, vector<vector<string>>> follow_sets = follow(noRecursion_cfg, first_list);
    cout << "----------------------------------------" << endl;
    cout << "Follow List \n";
    PrintFirstFollow(follow_sets);

    cout << "------------------------------------------------------" << endl;
    cout << "LL(1) Parsing Table \n";
    unordered_map<string, unordered_map<string, vector<string>>> parsingTable = constructLL1Table(cfg, first_list, follow_sets);

    cout << "\n";

    cout << "------------------------------------------------------" << endl;
    cout << "Parsing Table \n";
    string inputFile = "input.txt";
    string inputStr = readInputString(inputFile);
    parseInputStringUsingStack2(inputStr, parsingTable, "S");
    cout << "------------------------------------------------------" << endl;
    return 0;
}

void parseInputStringUsingStack(const string &input,
                                const unordered_map<string, unordered_map<string, vector<string>>> &parsingTable,
                                const string &startSymbol)
{
    cout << "\n======== PARSING INPUT ========\n";
    cout << "Input: " << input << endl;

    // Split input string into tokens (space-separated)
    vector<string> tokens;
    stringstream ss(input);
    string token;
    while (ss >> token)
    {
        tokens.push_back(token);
    }
    tokens.push_back("$"); // Add end marker

    // Initialize parsing stack
    stack<string> parseStack;
    parseStack.push("$");
    parseStack.push(startSymbol);

    // Track errors and recovery
    int errorCount = 0;
    vector<string> errorMessages;
    bool recoveredFromError = false;

    // Current token position
    size_t currentIndex = 0;

    cout << "\n--- Parsing Steps ---\n";
    cout << left << setw(40) << "Stack" << setw(20) << "Input" << "Action" << endl;
    cout << string(80, '-') << endl;

    while (!parseStack.empty() && currentIndex < tokens.size())
    {
        string currentToken = tokens[currentIndex];
        string topOfStack = parseStack.top();

        // Display current state
        string stackStr;
        stack<string> temp = parseStack;
        while (!temp.empty())
        {
            stackStr = temp.top() + " " + stackStr;
            temp.pop();
        }

        string remainingInput;
        for (size_t i = currentIndex; i < tokens.size(); i++)
        {
            remainingInput += tokens[i] + " ";
        }

        cout << left << setw(40) << stackStr << setw(20) << remainingInput;

        // Case 1: Successful parse completion
        if (topOfStack == "$" && currentToken == "$")
        {
            cout << "Accept" << endl;
            break;
        }

        // Case 2: Terminal at stack top
        if (parsingTable.find(topOfStack) == parsingTable.end())
        {
            if (topOfStack == currentToken)
            {
                cout << "Match: " << topOfStack << endl;
                parseStack.pop();
                currentIndex++;
                recoveredFromError = false;
            }
            else
            {
                errorMessages.push_back("Syntax Error: Expected '" + topOfStack +
                                        "' but found '" + currentToken + "'");
                errorCount++;
                cout << errorMessages.back() << endl;

                // Error recovery: Skip current token
                currentIndex++;
                recoveredFromError = true;
            }
            continue;
        }

        // Case 3: Non-terminal at stack top
        if (parsingTable.at(topOfStack).count(currentToken))
        {
            vector<string> production = parsingTable.at(topOfStack).at(currentToken);
            parseStack.pop();

            cout << "Expand: " << topOfStack << " -> ";
            for (const auto &s : production)
                cout << s << " ";
            cout << endl;

            if (!(production.size() == 1 && production[0] == "epsilon"))
            {
                for (auto it = production.rbegin(); it != production.rend(); ++it)
                {
                    parseStack.push(*it);
                }
            }
            recoveredFromError = false;
        }
        else
        {
            errorMessages.push_back("Syntax Error: Unexpected token '" + currentToken +
                                    "' after '" + topOfStack + "'");
            errorCount++;
            cout << errorMessages.back() << endl;

            // Error recovery: Pop non-terminal
            parseStack.pop();
            recoveredFromError = true;
        }
    }

    // Print results
    if (errorCount == 0)
    {
        cout << "\nParsing completed successfully!" << endl;
    }
    else
    {
        cout << "\nParsing completed with " << errorCount << " errors:" << endl;
        for (const auto &msg : errorMessages)
        {
            cout << msg << endl;
        }
        if (recoveredFromError)
        {
            cout << "Parsing continued after error recovery." << endl;
        }
    }
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
