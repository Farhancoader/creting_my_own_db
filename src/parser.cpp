#include "parser.h"
#include <iostream>
#include <stdexcept>

using namespace std;

Parser::Parser(const vector<Token>& tokens) 
    : tokens(tokens), current(0) {}

Token Parser::peek() {
    if (current < tokens.size()) {
        return tokens[current];
    }
    return {TokenType::END_OF_INPUT, ""};
}

Token Parser::advance() {
    if (current < tokens.size()) {
        return tokens[current++];
    }
    throw runtime_error("Unexpected end of input");
}

bool Parser::match(TokenType type) {
    if (peek().type == type) {
        advance();
        return true;
    }
    return false;
}

bool Parser::check(TokenType type, const string& value) {
    Token t = peek();
    return t.type == type && t.value == value;
}

void Parser::expect(TokenType type, const string& value) {
    Token t = peek();
    if (t.type != type || (!value.empty() && t.value != value)) {
        throw runtime_error("Unexpected token: " + t.value);
    }
    advance();
}

Query* Parser::parse() {
    if (check(TokenType::KEYWORD, "SELECT")) {
        return parse_select();
    } else if (check(TokenType::KEYWORD, "INSERT")) {
        return parse_insert();
    } else if (check(TokenType::KEYWORD, "DELETE")) {
        return parse_delete();
    } else if (check(TokenType::KEYWORD, "CREATE")) {
        return parse_create();
    }
    throw runtime_error("Unknown query type");
}

Query* Parser::parse_select() {
    SelectQuery* query = new SelectQuery();
    
    expect(TokenType::KEYWORD, "SELECT");
    
    // Parse columns
    query->columns = parse_column_list();
    
    expect(TokenType::KEYWORD, "FROM");
    
    // Parse table name
    Token table_token = advance();
    if (table_token.type != TokenType::IDENTIFIER) {
        throw runtime_error("Expected table name");
    }
    query->table = table_token.value;
    
    // Parse WHERE clause (optional)
    if (check(TokenType::KEYWORD, "WHERE")) {
        advance();
        query->where = parse_where_clause();
    }
    
    return query;
}

Query* Parser::parse_insert() {
    InsertQuery* query = new InsertQuery();
    
    expect(TokenType::KEYWORD, "INSERT");
    expect(TokenType::KEYWORD, "INTO");
    
    // Parse table name
    Token table_token = advance();
    if (table_token.type != TokenType::IDENTIFIER) {
        throw runtime_error("Expected table name");
    }
    query->table = table_token.value;
    
    // Parse column list
    expect(TokenType::SYMBOL, "(");
    query->columns = parse_column_list();
    expect(TokenType::SYMBOL, ")");
    
    expect(TokenType::KEYWORD, "VALUES");
    
    // Parse value list
    expect(TokenType::SYMBOL, "(");
    query->values = parse_value_list();
    expect(TokenType::SYMBOL, ")");
    
    return query;
}

Query* Parser::parse_delete() {
    DeleteQuery* query = new DeleteQuery();
    
    expect(TokenType::KEYWORD, "DELETE");
    expect(TokenType::KEYWORD, "FROM");
    
    // Parse table name
    Token table_token = advance();
    if (table_token.type != TokenType::IDENTIFIER) {
        throw runtime_error("Expected table name");
    }
    query->table = table_token.value;
    
    // Parse WHERE clause (required for safety)
    if (check(TokenType::KEYWORD, "WHERE")) {
        advance();
        query->where = parse_where_clause();
    } else {
        throw runtime_error("DELETE requires WHERE clause");
    }
    
    return query;
}

Query* Parser::parse_create() {
    CreateQuery* query = new CreateQuery();
    
    expect(TokenType::KEYWORD, "CREATE");
    expect(TokenType::KEYWORD, "TABLE");
    
    // Parse table name
    Token table_token = advance();
    if (table_token.type != TokenType::IDENTIFIER) {
        throw runtime_error("Expected table name");
    }
    query->table = table_token.value;
    
    expect(TokenType::SYMBOL, "(");
    
    // Parse column definitions
    while (!check(TokenType::SYMBOL, ")")) {
        Token col_name = advance();
        if (col_name.type != TokenType::IDENTIFIER) {
            throw runtime_error("Expected column name");
        }
        
        Token col_type = advance();
        if (col_type.type != TokenType::IDENTIFIER && col_type.type != TokenType::KEYWORD) {
            throw runtime_error("Expected column type");
        }
        
        query->columns.push_back({col_name.value, col_type.value});
        
        if (!check(TokenType::SYMBOL, ")")) {
            expect(TokenType::SYMBOL, ",");
        }
    }
    
    expect(TokenType::SYMBOL, ")");
    
    return query;
}

Condition* Parser::parse_where_clause() {
    Condition* cond = new Condition();
    
    // Parse left side (column name)
    Token col = advance();
    if (col.type != TokenType::IDENTIFIER) {
        throw runtime_error("Expected column name in WHERE");
    }
    cond->column = col.value;
    
    // Parse operator
    Token op = advance();
    if (op.type != TokenType::OPERATOR) {
        throw runtime_error("Expected operator in WHERE");
    }
    cond->op = op.value;
    
    // Parse right side (value)
    Token val = advance();
    if (val.type == TokenType::NUMBER) {
        cond->value = val.value;
    } else if (val.type == TokenType::STRING) {
        cond->value = val.value;
    } else if (val.type == TokenType::IDENTIFIER) {
        cond->value = val.value;
    } else {
        throw runtime_error("Expected value in WHERE");
    }
    
    return cond;
}

vector<string> Parser::parse_column_list() {
    vector<string> columns;
    
    // Handle SELECT *
    if (check(TokenType::SYMBOL, "*")) {
        advance();
        columns.push_back("*");
        return columns;
    }
    
    // Parse column names
    while (true) {
        Token col = advance();
        if (col.type != TokenType::IDENTIFIER) {
            throw runtime_error("Expected column name");
        }
        columns.push_back(col.value);
        
        if (!check(TokenType::SYMBOL, ",")) {
            break;
        }
        advance();  // consume comma
    }
    
    return columns;
}

vector<string> Parser::parse_value_list() {
    vector<string> values;
    
    while (true) {
        Token val = advance();
        if (val.type == TokenType::NUMBER) {
            values.push_back(val.value);
        } else if (val.type == TokenType::STRING) {
            values.push_back(val.value);
        } else {
            throw runtime_error("Expected value");
        }
        
        if (!check(TokenType::SYMBOL, ",")) {
            break;
        }
        advance();  // consume comma
    }
    
    return values;
}
