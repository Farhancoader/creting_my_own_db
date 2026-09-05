#include "tokenizer.h"
#include "parser.h"
#include "executor.h"
#include <iostream>

using namespace std;

int main() {
    cout << "=== Mini SQL Database ===\n\n";
    
    Tokenizer tokenizer;
    Executor executor;
    
    // Test 1: CREATE TABLE
    cout << "Test 1: CREATE TABLE users\n";
    string sql1 = "CREATE TABLE users (id INT, name TEXT, age INT)";
    auto tokens1 = tokenizer.tokenize(sql1);
    Parser parser1(tokens1);
    auto q1 = parser1.parse();
    auto result1 = executor.execute(q1);
    cout << (result1->success ? "✓ Success" : "✗ Failed: " + result1->error_message) << "\n\n";
    delete q1;
    delete result1;
    
    // Test 2: INSERT
    cout << "Test 2: INSERT INTO users\n";
    string sql2 = "INSERT INTO users (id, name, age) VALUES (1, 'Alice', 30)";
    auto tokens2 = tokenizer.tokenize(sql2);
    Parser parser2(tokens2);
    auto q2 = parser2.parse();
    auto result2 = executor.execute(q2);
    cout << (result2->success ? "✓ Success" : "✗ Failed: " + result2->error_message) << "\n";
    delete q2;
    delete result2;
    
    cout << "Test 2b: INSERT another row\n";
    string sql2b = "INSERT INTO users (id, name, age) VALUES (2, 'Bob', 25)";
    auto tokens2b = tokenizer.tokenize(sql2b);
    Parser parser2b(tokens2b);
    auto q2b = parser2b.parse();
    auto result2b = executor.execute(q2b);
    cout << (result2b->success ? "✓ Success" : "✗ Failed: " + result2b->error_message) << "\n";
    delete q2b;
    delete result2b;
    
    cout << "Test 2c: INSERT third row\n";
    string sql2c = "INSERT INTO users (id, name, age) VALUES (3, 'Charlie', 35)";
    auto tokens2c = tokenizer.tokenize(sql2c);
    Parser parser2c(tokens2c);
    auto q2c = parser2c.parse();
    auto result2c = executor.execute(q2c);
    cout << (result2c->success ? "✓ Success" : "✗ Failed: " + result2c->error_message) << "\n\n";
    delete q2c;
    delete result2c;
    
    // Test 3: SELECT all
    cout << "Test 3: SELECT id, name, age FROM users\n";
    string sql3 = "SELECT id, name, age FROM users";
    auto tokens3 = tokenizer.tokenize(sql3);
    Parser parser3(tokens3);
    auto q3 = parser3.parse();
    auto result3 = executor.execute(q3);
    if (result3->success) {
        cout << "Columns: ";
        for (const auto& col : result3->columns) cout << col << " ";
        cout << "\n";
        cout << "Rows (" << result3->rows.size() << "):\n";
        for (const auto& row : result3->rows) {
            for (const auto& col : result3->columns) {
                auto it = row.data.find(col);
                if (it != row.data.end()) {
                    cout << "  " << col << "=" << it->second;
                }
            }
            cout << "\n";
        }
    } else {
        cout << "✗ Failed: " << result3->error_message << "\n";
    }
    delete q3;
    delete result3;
    cout << "\n";
    
    // Test 4: SELECT with WHERE
    cout << "Test 4: SELECT name FROM users WHERE age > 26\n";
    string sql4 = "SELECT name FROM users WHERE age > 26";
    auto tokens4 = tokenizer.tokenize(sql4);
    Parser parser4(tokens4);
    auto q4 = parser4.parse();
    auto result4 = executor.execute(q4);
    if (result4->success) {
        cout << "Results:\n";
        for (const auto& row : result4->rows) {
            auto it = row.data.find("name");
            if (it != row.data.end()) {
                cout << "  " << it->second << "\n";
            }
        }
    } else {
        cout << "✗ Failed: " << result4->error_message << "\n";
    }
    delete q4;
    delete result4;
    cout << "\n";
    
    // Test 5: DELETE
    cout << "Test 5: DELETE FROM users WHERE id = 2\n";
    string sql5 = "DELETE FROM users WHERE id = 2";
    auto tokens5 = tokenizer.tokenize(sql5);
    Parser parser5(tokens5);
    auto q5 = parser5.parse();
    auto result5 = executor.execute(q5);
    cout << (result5->success ? "✓ Success" : "✗ Failed: " + result5->error_message) << "\n";
    delete q5;
    delete result5;
    cout << "\n";
    
    // Test 6: SELECT after DELETE
    cout << "Test 6: SELECT id, name, age FROM users (after delete)\n";
    string sql6 = "SELECT id, name, age FROM users";
    auto tokens6 = tokenizer.tokenize(sql6);
    Parser parser6(tokens6);
    auto q6 = parser6.parse();
    auto result6 = executor.execute(q6);
    if (result6->success) {
        cout << "Rows (" << result6->rows.size() << "):\n";
        for (const auto& row : result6->rows) {
            for (const auto& col : result6->columns) {
                auto it = row.data.find(col);
                if (it != row.data.end()) {
                    cout << "  " << col << "=" << it->second;
                }
            }
            cout << "\n";
        }
    } else {
        cout << "✗ Failed: " << result6->error_message << "\n";
    }
    delete q6;
    delete result6;
    
    cout << "\n=== All tests completed ===\n";
    return 0;
}
