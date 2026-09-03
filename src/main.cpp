#include "tokenizer.h"
#include <iostream>

using namespace std;

void print_token_type(TokenType type) {
    switch(type) {
        case TokenType::KEYWORD: cout << "KEYWORD"; break;
        case TokenType::IDENTIFIER: cout << "IDENTIFIER"; break;
        case TokenType::NUMBER: cout << "NUMBER"; break;
        case TokenType::STRING: cout << "STRING"; break;
        case TokenType::OPERATOR: cout << "OPERATOR"; break;
        case TokenType::SYMBOL: cout << "SYMBOL"; break;
        case TokenType::END_OF_INPUT: cout << "EOF"; break;
    }
}

int main() {
    cout << "=== Tokenizer Test ===\n\n";
    
    Tokenizer tokenizer;
    
    cout << "Test 1: SELECT statement\n";
    string sql1 = "SELECT name FROM users";
    auto tokens = tokenizer.tokenize(sql1);
    cout << "Input: " << sql1 << "\n";
    cout << "Tokens:\n";
    for (const auto& token : tokens) {
        if (token.type == TokenType::END_OF_INPUT) break;
        cout << "  ";
        print_token_type(token.type);
        cout << " : '" << token.value << "'\n";
    }
    cout << "\n";
    
    cout << "Test 2: WHERE clause\n";
    string sql2 = "SELECT * FROM users WHERE age > 30";
    tokens = tokenizer.tokenize(sql2);
    cout << "Input: " << sql2 << "\n";
    cout << "Tokens:\n";
    for (const auto& token : tokens) {
        if (token.type == TokenType::END_OF_INPUT) break;
        cout << "  ";
        print_token_type(token.type);
        cout << " : '" << token.value << "'\n";
    }
    cout << "\n";
    
    cout << "Test 3: INSERT statement\n";
    string sql3 = "INSERT INTO users VALUES (1, 'Alice')";
    tokens = tokenizer.tokenize(sql3);
    cout << "Input: " << sql3 << "\n";
    cout << "Tokens:\n";
    for (const auto& token : tokens) {
        if (token.type == TokenType::END_OF_INPUT) break;
        cout << "  ";
        print_token_type(token.type);
        cout << " : '" << token.value << "'\n";
    }
    cout << "\n";
    
    cout << "Test 4: Case insensitive\n";
    string sql4 = "select name from users where id = 5";
    tokens = tokenizer.tokenize(sql4);
    cout << "Input: " << sql4 << "\n";
    cout << "Tokens:\n";
    for (const auto& token : tokens) {
        if (token.type == TokenType::END_OF_INPUT) break;
        cout << "  ";
        print_token_type(token.type);
        cout << " : '" << token.value << "'\n";
    }
    
    cout << "\n=== All tests completed ===\n";
    return 0;
}
