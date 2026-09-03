#include "tokenizer.h"
#include <iostream>

using namespace std;

void print_tokens(const vector<Token>& tokens) {
    for (const auto& token : tokens) {
        cout << "Type: ";
        switch(token.type) {
            case TokenType::KEYWORD: cout << "KEYWORD"; break;
            case TokenType::IDENTIFIER: cout << "IDENTIFIER"; break;
            case TokenType::NUMBER: cout << "NUMBER"; break;
            case TokenType::STRING: cout << "STRING"; break;
            case TokenType::OPERATOR: cout << "OPERATOR"; break;
            case TokenType::SYMBOL: cout << "SYMBOL"; break;
            case TokenType::END_OF_INPUT: cout << "EOF"; break;
        }
        cout << " | Value: '" << token.value << "'\n";
    }
}

int main() {
    Tokenizer tokenizer;
    
    cout << "Test 1: Simple SELECT\n";
    auto tokens = tokenizer.tokenize("SELECT name FROM users");
    print_tokens(tokens);
    cout << "\n";
    
    cout << "Test 2: WHERE clause\n";
    tokens = tokenizer.tokenize("SELECT * FROM users WHERE age > 30");
    print_tokens(tokens);
    cout << "\n";
    
    cout << "Test 3: INSERT statement\n";
    tokens = tokenizer.tokenize("INSERT INTO users VALUES (1, 'Alice')");
    print_tokens(tokens);
    cout << "\n";
    
    cout << "Test 4: Case insensitive\n";
    tokens = tokenizer.tokenize("select name from users");
    print_tokens(tokens);
    
    return 0;
}