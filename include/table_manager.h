#ifndef TABLE_MANAGER_H
#define TABLE_MANAGER_H

#include "storage_engine.h"
#include <vector>
#include <map>
#include <string>
#include <memory>

using namespace std;

struct TableSchema {
    string table_name;
    vector<pair<string, string>> columns;  // (column_name, type)
};

struct TableData {
    string table_name;
    vector<map<string, string>> rows;
};

class TableManager {
private:
    // In-memory tables
    map<string, vector<map<string, string>>> tables;
    map<string, TableSchema> schemas;
    
    // Storage engines for persistence
    map<string, StorageEngine*> storage_engines;
    
    string db_dir;
    
public:
    TableManager(const string& directory = "minidb_data");
    ~TableManager();
    
    // Table operations
    bool create_table(const string& table_name, 
                     const vector<pair<string, string>>& columns);
    bool table_exists(const string& table_name);
    vector<string> get_table_names();
    TableSchema get_schema(const string& table_name);
    
    // Row operations
    bool insert_row(const string& table_name, 
                   const map<string, string>& row);
    vector<map<string, string>> get_all_rows(const string& table_name);
    bool delete_rows(const string& table_name, 
                    const vector<int>& row_indices);
    
    // Persistence
    void save_all();
    void load_all();
    
private:
    void save_table(const string& table_name);
    void load_table(const string& table_name);
    string get_table_file(const string& table_name);
    void ensure_directory_exists();
};

#endif