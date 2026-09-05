#include "tokenizer.h"
#include <cctype>
#include <unordered_set>
#include <algorithm>

using namespace std;

const unordered_set<string> keywords = {
    "SELECT", "FROM", "WHERE", "INSERT", "INTO", "VALUES",
    "CREATE", "TABLE", "DELETE", "UPDATE", "AND", "OR"
};

string to_upper(const string& str) {
    string result = str;
    transform(result.begin(), result.end(), result.begin(), ::toupper);
    return result;
}

vector<Token> Tokenizer::tokenize(const string &input) {
    vector<Token> tokens;
    size_t i = 0;
    
    while (i < input.size()) {
        while (i < input.size() && isspace(input[i])) {
            i++;
        }
        
        if (i >= input.size()) break;
        
        // Identifiers and Keywords
        if (isalpha(input[i]) || input[i] == '_') {
            string word;
            while (i < input.size() && (isalnum(input[i]) || input[i] == '_')) {
                word += input[i];
                i++;
            }
            
            string upper_word = to_upper(word);
            if (keywords.find(upper_word) != keywords.end()) {
                tokens.push_back({TokenType::KEYWORD, upper_word});
            } else {
                tokens.push_back({TokenType::IDENTIFIER, word});
            }
            continue;
        }
        
        // Numbers
        if (isdigit(input[i])) {
            string number;
            while (i < input.size() && isdigit(input[i])) {
                number += input[i];
                i++;
            }
            tokens.push_back({TokenType::NUMBER, number});
            continue;
        }
        
        // String literals
        if (input[i] == '\'') {
            i++;
            string value;
            while (i < input.size() && input[i] != '\'') {
                value += input[i];
                i++;
            }
            if (i < input.size()) i++;
            tokens.push_back({TokenType::STRING, value});
            continue;
        }
        
        // Operators: =, <, >, !, <=, >=, !=
        if (input[i] == '=' || input[i] == '<' || 
            input[i] == '>' || input[i] == '!') {
            string op;
            op += input[i];
            i++;
            
            if (i < input.size() && input[i] == '=') {
                op += input[i];
                i++;
            }
            tokens.push_back({TokenType::OPERATOR, op});
            continue;
        }
        
        // Symbols: (, ), ,, ;, *
        if (input[i] == ',' || input[i] == '(' || 
            input[i] == ')' || input[i] == ';' || input[i] == '*') {
            string symbol;
            symbol += input[i];
            tokens.push_back({TokenType::SYMBOL, symbol});
            i++;
            continue;
        }
        
        // Unknown character - skip it
        i++;
    }
    
    tokens.push_back({TokenType::END_OF_INPUT, ""});
    return tokens;
}
