#ifndef PARSER_H
#define PARSER_H

#include "tokenizer.h"
#include <vector>
#include <string>
#include <memory>

using namespace std;

// AST Node Types
enum class QueryType {
    SELECT,
    INSERT,
    DELETE,
    CREATE
};

// Represents a condition in WHERE clause (e.g., age > 30)
struct Condition {
    string column;
    string op;           
    string value;
};

// Base Query AST
struct Query {
    QueryType type;
    virtual ~Query() = default;
};

// SELECT query AST
struct SelectQuery : Query {
    vector<string> columns; 
    string table;              
    Condition* where;          
    
    SelectQuery() : where(nullptr) { type = QueryType::SELECT; }
    ~SelectQuery() { delete where; }
};

// INSERT query AST
struct InsertQuery : Query {
    string table;              // "users"
    vector<string> columns;    // ["id", "name", "age"]
    vector<string> values;     // ["1", "Alice", "30"]
    
    InsertQuery() { type = QueryType::INSERT; }
};

// DELETE query AST
struct DeleteQuery : Query {
    string table;              // "users"
    Condition* where;          // WHERE clause
    
    DeleteQuery() : where(nullptr) { type = QueryType::DELETE; }
    ~DeleteQuery() { delete where; }
};

// CREATE TABLE query AST
struct CreateQuery : Query {
    string table;              // "users"
    vector<pair<string, string>> columns;  // [("id", "INT"), ("name", "TEXT")]
    
    CreateQuery() { type = QueryType::CREATE; }
};

// Parser class
class Parser {
private:
    vector<Token> tokens;
    size_t current;
    
    // Helper methods
    Token peek();
    Token advance();
    bool match(TokenType type);
    bool check(TokenType type, const string& value);
    void expect(TokenType type, const string& value = "");
    
    // Parsing methods
    Query* parse_select();
    Query* parse_insert();
    Query* parse_delete();
    Query* parse_create();
    
    Condition* parse_where_clause();
    vector<string> parse_column_list();
    vector<string> parse_value_list();

public:
    Parser(const vector<Token>& tokens);
    Query* parse();
};

#endif