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
/*
void parseInput(const string &input_string,
                const unordered_map<string, unordered_map<string, vector<string>>> &parsingTable,
                const string &startSymbol)
{
    cout << "\n======== PARSING INPUT ========\n";
    cout << "Input: " << input_string << endl;

    // Tokenize the input string
    vector<string> tokens;
    stringstream ss(input_string);
    string token;

    while (getline(ss, token, ' '))
    {
        // Skip empty tokens
        if (!token.empty())
        {
            tokens.push_back(token);
        }
    }
    tokens.push_back("$"); // Add end marker

    // Initialize parsing stack
    stack<string> parseStack;
    parseStack.push("$");         // End marker at the bottom
    parseStack.push(startSymbol); // Start symbol

    // Initialize current token index
    size_t currentIndex = 0;

    // Variables for error tracking
    int errorCount = 0;
    vector<string> errorMessages;

    cout << "\n--- Parsing Steps ---\n";
    cout << left << setw(30) << "Stack" << setw(15) << "Input" << "Action" << endl;
    cout << string(70, '-') << endl;

    // Continue parsing until stack is empty or all tokens are consumed
    while (!parseStack.empty() && currentIndex <= tokens.size())
    {
        // Print current stack contents
        string stackContents = "";
        stack<string> tempStack = parseStack;
        vector<string> stackItems;

        while (!tempStack.empty())
        {
            stackItems.push_back(tempStack.top());
            tempStack.pop();
        }

        reverse(stackItems.begin(), stackItems.end());
        for (const auto &item : stackItems)
        {
            stackContents += item + " ";
        }

        // Get current input token
        string currentToken = (currentIndex < tokens.size()) ? tokens[currentIndex] : "$";

        cout << left << setw(30) << stackContents << setw(15) << currentToken;

        // Get top of stack
        string topOfStack = parseStack.top();

        // Case 1: Top of stack is $ and current token is $ -> Parsing completed successfully
        if (topOfStack == "$" && currentToken == "$")
        {
            cout << "Accept" << endl;
            break;
        }

        // Case 2: Top of stack is terminal
        bool isTerminal = true;
        for (const auto &pair : parsingTable)
        {
            if (pair.first == topOfStack)
            {
                isTerminal = false;
                break;
            }
        }

        if (isTerminal)
        {
            // If terminal matches current token
            if (topOfStack == currentToken)
            {
                cout << "Match: " << topOfStack << endl;
                parseStack.pop();
                currentIndex++;
            }
            else
            {
                // Error: Terminal mismatch
                string errorMsg = "Syntax Error: Expected '" + topOfStack + "' but found '" + currentToken + "'";
                cout << errorMsg << endl;
                errorMessages.push_back(errorMsg);
                errorCount++;

                // Error recovery: Skip current token
                currentIndex++;
            }
            continue;
        }

        // Case 3: Top of stack is non-terminal
        // Check if there's an entry in parsing table
        if (parsingTable.find(topOfStack) != parsingTable.end() &&
            parsingTable.at(topOfStack).find(currentToken) != parsingTable.at(topOfStack).end())
        {

            // Get production rule
            vector<string> production = parsingTable.at(topOfStack).at(currentToken);

            // Remove the non-terminal from stack
            parseStack.pop();

            // Push production in reverse order onto the stack
            string actionStr = "Expand: " + topOfStack + " -> ";
            for (auto it = production.rbegin(); it != production.rend(); ++it)
            {
                if (*it != "epsilon")
                {
                    parseStack.push(*it);
                    actionStr += *it + " ";
                }
                else
                {
                    actionStr += "ε ";
                }
            }
            cout << actionStr << endl;
        }
        else
        {
            // Error: No production rule found
            string errorMsg = "Syntax Error: No production rule for " + topOfStack + " with token " + currentToken;
            cout << errorMsg << endl;
            errorMessages.push_back(errorMsg);
            errorCount++;

            // Error recovery: Pop the non-terminal and continue
            parseStack.pop();
        }
    }

    cout << "\n--- Parsing Summary ---\n";
    if (errorCount == 0)
    {
        cout << "Parsing completed successfully!" << endl;
    }
    else
    {
        cout << "Parsing completed with " << errorCount << " errors." << endl;
        cout << "\nError Details:\n";
        for (size_t i = 0; i < errorMessages.size(); i++)
        {
            cout << i + 1 << ". " << errorMessages[i] << endl;
        }
    }
}

// Function to parse a file containing input strings
void parseInputFile(const string &filename,
                    const unordered_map<string, unordered_map<string, vector<string>>> &parsingTable,
                    const string &startSymbol)
{
    ifstream file(filename);
    if (!file.is_open())
    {
        cout << "Error opening file: " << filename << endl;
        return;
    }

    string line;
    int lineNumber = 1;

    cout << "\n====== PARSING FILE: " << filename << " ======\n";

    while (getline(file, line))
    {
        cout << "\n\n===== LINE " << lineNumber << " =====\n";
        parseInput(line, parsingTable, startSymbol);
        lineNumber++;
    }

    file.close();
}

// Function to parse input from the example you provided
void parseExampleInput(const unordered_map<string, unordered_map<string, vector<string>>> &parsingTable)
{
    string input = "int x;\nx = 5 + ;\nif (x > 0{\nx = x - 1;\n}";

    // Split into lines to process
    stringstream ss(input);
    string line;
    int lineNumber = 1;
    vector<string> errors;

    cout << "\n====== PARSING EXAMPLE INPUT ======\n";

    while (getline(ss, line))
    {
        cout << "Line " << lineNumber << ": " << line << endl;

        // Process each line with the parser
        // This is a simplified version for the example - you'd need to adapt this
        // to work with your specific grammar and token handling

        // Here we're just demonstrating error detection for the specific example
        if (lineNumber == 2 && line.find("x = 5 + ;") != string::npos)
        {
            errors.push_back("Line 2: Syntax Error: Unexpected token ';' after '+'");
        }
        if (lineNumber == 3 && line.find("if (x > 0{") != string::npos)
        {
            errors.push_back("Line 3: Syntax Error: Expected ')' before '{'");
        }

        lineNumber++;
    }

    // Print errors
    for (const auto &error : errors)
    {
        cout << error << endl;
    }

    if (errors.size() > 0)
    {
        cout << "Parsing continued after error recovery." << endl;
        cout << "Parsing completed with " << errors.size() << " errors." << endl;
    }
    else
    {
        cout << "Parsing completed successfully!" << endl;
    }
}*/

void parseInputString(const string &input,
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

    string inputFile = "input.txt";
    string inputStr = readInputString(inputFile);
    // parseInputFile(inputFile, parsingTable, "S"); // Assuming "S" is your start symbol
    parseInputString(inputStr, parsingTable, "S");

    // For the specific example in your assignment
    // parseExampleInput(parsingTable);
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
