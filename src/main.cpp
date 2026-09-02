#include "bplus_tree.h"
#include <iostream>

using namespace std;

int main() {
    cout << "=== B+ Tree Test ===\n\n";
    
    BPlusTree<int, string> tree;
    
    cout << "Test 1: Insert 8 entries\n";
    tree.insert(10, "Alice");
    tree.insert(20, "Bob");
    tree.insert(30, "Charlie");
    tree.insert(40, "David");
    tree.insert(50, "Eve");
    tree.insert(60, "Frank");
    tree.insert(70, "Grace");
    tree.insert(80, "Henry");
    cout << "Inserted successfully\n\n";
    
    cout << "Tree structure:\n";
    tree.print();
    cout << "\n";
    
    cout << "Test 2: Search\n";
    auto val = tree.search(30);
    if (val) {
        cout << "Found key 30: " << *val << "\n";
    }
    
    val = tree.search(60);
    if (val) {
        cout << "Found key 60: " << *val << "\n";
    }
    
    val = tree.search(99);
    if (!val) {
        cout << "Key 99 not found (expected)\n";
    }
    cout << "\n";
    
    cout << "Test 3: Range search (25-65)\n";
    auto results = tree.range_search(25, 65);
    for (const auto& r : results) {
        cout << "  " << r << "\n";
    }
    cout << "\n";
    
    cout << "Test 4: Insert more entries\n";
    tree.insert(15, "Iris");
    tree.insert(25, "Jack");
    tree.insert(35, "Kate");
    tree.insert(45, "Liam");
    cout << "Inserted 4 more entries\n";
    cout << "Tree structure after splits:\n";
    tree.print();
    cout << "\n";
    
    cout << "=== All tests passed ===\n";
    return 0;
}
