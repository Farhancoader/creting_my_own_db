#include "tokenizer.h"
#include "parser.h"
#include <iostream>

using namespace std;

void print_query(Query* query) {
    if (query->type == QueryType::SELECT) {
        SelectQuery* q = (SelectQuery*)query;
        cout << "SELECT Query:\n";
        cout << "  Columns: ";
        for (const auto& col : q->columns) cout << col << " ";
        cout << "\n  Table: " << q->table << "\n";
        if (q->where) {
            cout << "  WHERE: " << q->where->column << " " 
                 << q->where->op << " " << q->where->value << "\n";
        }
    } else if (query->type == QueryType::INSERT) {
        InsertQuery* q = (InsertQuery*)query;
        cout << "INSERT Query:\n";
        cout << "  Table: " << q->table << "\n";
        cout << "  Columns: ";
        for (const auto& col : q->columns) cout << col << " ";
        cout << "\n  Values: ";
        for (const auto& val : q->values) cout << val << " ";
        cout << "\n";
    } else if (query->type == QueryType::DELETE) {
        DeleteQuery* q = (DeleteQuery*)query;
        cout << "DELETE Query:\n";
        cout << "  Table: " << q->table << "\n";
        if (q->where) {
            cout << "  WHERE: " << q->where->column << " " 
                 << q->where->op << " " << q->where->value << "\n";
        }
    } else if (query->type == QueryType::CREATE) {
        CreateQuery* q = (CreateQuery*)query;
        cout << "CREATE Query:\n";
        cout << "  Table: " << q->table << "\n";
        cout << "  Columns: ";
        for (const auto& col : q->columns) {
            cout << col.first << " " << col.second << ", ";
        }
        cout << "\n";
    }
}

int main() {
    cout << "=== Parser Test ===\n\n";
    
    Tokenizer tokenizer;
    
    // Test 1: SELECT
    cout << "Test 1: SELECT statement\n";
    string sql1 = "SELECT name, age FROM users WHERE age > 30";
    auto tokens1 = tokenizer.tokenize(sql1);
    Parser parser1(tokens1);
    auto q1 = parser1.parse();
    print_query(q1);
    delete q1;
    cout << "\n";
    
    // Test 2: INSERT
    cout << "Test 2: INSERT statement\n";
    string sql2 = "INSERT INTO users (id, name, age) VALUES (1, 'Alice', 30)";
    auto tokens2 = tokenizer.tokenize(sql2);
    Parser parser2(tokens2);
    auto q2 = parser2.parse();
    print_query(q2);
    delete q2;
    cout << "\n";
    
    // Test 3: DELETE
    cout << "Test 3: DELETE statement\n";
    string sql3 = "DELETE FROM users WHERE id = 5";
    auto tokens3 = tokenizer.tokenize(sql3);
    Parser parser3(tokens3);
    auto q3 = parser3.parse();
    print_query(q3);
    delete q3;
    cout << "\n";
    
    // Test 4: CREATE TABLE
    cout << "Test 4: CREATE TABLE statement\n";
    string sql4 = "CREATE TABLE users (id INT, name TEXT, age INT)";
    auto tokens4 = tokenizer.tokenize(sql4);
    Parser parser4(tokens4);
    auto q4 = parser4.parse();
    print_query(q4);
    delete q4;
    
    cout << "\n=== All tests completed ===\n";
    return 0;
}
