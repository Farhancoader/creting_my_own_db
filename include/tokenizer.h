#ifndef TOKENIZER_H
#define TOKENIZER_H

#include <string>
#include <vector>

using namespace std;

enum class TokenType {
    KEYWORD,
    IDENTIFIER,
    NUMBER,
    STRING,
    OPERATOR,
    SYMBOL,
    END_OF_INPUT
};

struct Token {
    TokenType type;
    string value;
};

class Tokenizer {
public:
    vector<Token> tokenize(const string &input);
};

#endif 