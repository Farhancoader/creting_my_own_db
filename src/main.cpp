#include "tokenizer.h"
#include "parser.h"
#include "executor.h"
#include <iostream>
#include <string>

using namespace std;

void print_result(QueryResult* result) {
    if (!result->success) {
        cout << "❌ Error: " << result->error_message << "\n";
        return;
    }
    
    if (result->columns.empty() && result->rows.empty()) {
        cout << "✓ Query executed successfully\n";
        return;
    }
    
    // Print columns
    cout << "\nColumns: ";
    for (const auto& col : result->columns) {
        cout << col << " | ";
    }
    cout << "\n";
    cout << string(50, '-') << "\n";
    
    // Print rows
    if (result->rows.empty()) {
        cout << "No rows returned\n";
    } else {
        for (const auto& row : result->rows) {
            for (const auto& col : result->columns) {
                auto it = row.data.find(col);
                if (it != row.data.end()) {
                    cout << it->second << " | ";
                } else {
                    cout << "NULL | ";
                }
            }
            cout << "\n";
        }
    }
    cout << "\n" << result->rows.size() << " rows returned\n\n";
}

int main() {
    cout << "╔════════════════════════════════════════╗\n";
    cout << "║   Mini SQL Database (Interactive)     ║\n";
    cout << "║   Type 'EXIT' to quit                 ║\n";
    cout << "║   Type 'HELP' for commands            ║\n";
    cout << "╚════════════════════════════════════════╝\n\n";
    
    Tokenizer tokenizer;
    Executor executor;
    
    string input;
    while (true) {
        cout << "minidb> ";
        getline(cin, input);
        
        // Trim whitespace
        if (input.empty()) continue;
        
        // Check for exit
        if (input == "EXIT" || input == "exit") {
            cout << "Goodbye!\n";
            break;
        }
        
        // Check for help
        if (input == "HELP" || input == "help") {
            cout << "\n";
            cout << "Supported Commands:\n";
            cout << "  CREATE TABLE <name> (col1 TYPE, col2 TYPE, ...)\n";
            cout << "  INSERT INTO <table> (col1, col2, ...) VALUES (val1, val2, ...)\n";
            cout << "  SELECT col1, col2 FROM <table> [WHERE condition]\n";
            cout << "  DELETE FROM <table> WHERE condition\n";
            cout << "\n";
            cout << "Examples:\n";
            cout << "  CREATE TABLE users (id INT, name TEXT, age INT)\n";
            cout << "  INSERT INTO users (id, name, age) VALUES (1, 'Alice', 30)\n";
            cout << "  SELECT name, age FROM users WHERE age > 25\n";
            cout << "  DELETE FROM users WHERE id = 1\n";
            cout << "\n";
            continue;
        }
        
        try {
            // Tokenize
            auto tokens = tokenizer.tokenize(input);
            
            // Parse
            Parser parser(tokens);
            auto query = parser.parse();
            
            // Execute
            auto result = executor.execute(query);
            
            // Print result
            print_result(result);
            
            // Cleanup
            delete query;
            delete result;
            
        } catch (const exception& e) {
            cout << "❌ Error: " << e.what() << "\n\n";
        }
    }
    
    return 0;
}
