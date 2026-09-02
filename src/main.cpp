#include "../include/storage_engine.h"
#include <iostream>

int main() {
    // Create a database file "mydb.db"
    StorageEngine db("mydb.db");
    
    // Add some data
    db.put("alice", "alice@example.com");
    db.put("bob", "bob@example.com");
    db.put("count", "42");
    
    // Retrieve data
    auto alice_email = db.get("alice");
    if (alice_email) {
        std::cout << "Alice's email: " << *alice_email << std::endl;
    }
    
    // Program exits here, destructor called, data saved to disk
    return 0;
}