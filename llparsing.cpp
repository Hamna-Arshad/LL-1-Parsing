#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <sstream>
#include "LF.h"
using namespace std;

int main() {
    fstream file("cfg.txt");
    string line;
    string nonterminal, arrow, token;
    unordered_map<string, vector<vector<string>>> cfg;
    
    while (getline(file, line)) {
        stringstream ss(line);
        ss >> nonterminal >> arrow >> ws; // reads the arrow and whitespaces afterward to remove them from buffer        
        vector<string> rule;  
        
        while (ss >> token) {
            cout << "Token: " << token << endl;
            if (token == "|") {
                cfg[nonterminal].push_back(rule);                
                rule.clear();
            } else {
                rule.push_back(token);
            }
        }
        if (!rule.empty()) {
            cfg[nonterminal].push_back(rule);
        }
    }
    unordered_map<string, vector<vector<string>>> left_factored_cfg = left_factor(cfg);
    
    cout<<endl<<"Left Factored CFG: "<<endl<<endl;
    for (auto it = left_factored_cfg.begin(); it != left_factored_cfg.end(); ++it) {
        cout << "Nonterminal: " << it->first << " ->";
        
        for (int i = 0; i < it->second.size(); i++) {
            cout << " ";
            
            for (int j = 0; j < it->second[i].size(); j++) {
                cout << it->second[i][j] << " ";
            }
            cout << "|";
        }
        cout << endl;
    }
    
    return 0;
}