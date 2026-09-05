#include "table_manager.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <algorithm>
#include <string>

namespace fs = std::filesystem;
using namespace std;

TableManager::TableManager(const string& directory) 
    : db_dir(directory) {
    ensure_directory_exists();
    load_all();
}

TableManager::~TableManager() {
    save_all();
    for (auto& [name, engine] : storage_engines) {
        delete engine;
    }
}

void TableManager::ensure_directory_exists() {
    try {
        fs::create_directories(db_dir);
    } catch (...) {
        // Directory already exists
    }
}

bool TableManager::create_table(const string& table_name,
                               const vector<pair<string, string>>& columns) {
    if (table_exists(table_name)) {
        return false;
    }
    
    TableSchema schema;
    schema.table_name = table_name;
    schema.columns = columns;
    
    schemas[table_name] = schema;
    tables[table_name] = vector<map<string, string>>();
    
    save_table(table_name);
    return true;
}

bool TableManager::table_exists(const string& table_name) {
    return schemas.find(table_name) != schemas.end();
}

vector<string> TableManager::get_table_names() {
    vector<string> names;
    for (const auto& [name, schema] : schemas) {
        names.push_back(name);
    }
    return names;
}

TableSchema TableManager::get_schema(const string& table_name) {
    if (table_exists(table_name)) {
        return schemas[table_name];
    }
    return TableSchema();
}

bool TableManager::insert_row(const string& table_name,
                             const map<string, string>& row) {
    if (!table_exists(table_name)) {
        return false;
    }
    
    tables[table_name].push_back(row);
    save_table(table_name);
    return true;
}

vector<map<string, string>> TableManager::get_all_rows(const string& table_name) {
    if (!table_exists(table_name)) {
        return vector<map<string, string>>();
    }
    return tables[table_name];
}

bool TableManager::delete_rows(const string& table_name,
                              const vector<int>& row_indices) {
    if (!table_exists(table_name)) {
        return false;
    }
    
    // Sort indices in reverse to delete from end first
    vector<int> sorted_indices = row_indices;
    sort(sorted_indices.rbegin(), sorted_indices.rend());
    
    for (int idx : sorted_indices) {
        if (idx >= 0 && idx < (int)tables[table_name].size()) {
            tables[table_name].erase(tables[table_name].begin() + idx);
        }
    }
    
    save_table(table_name);
    return true;
}

string TableManager::get_table_file(const string& table_name) {
    return db_dir + "/" + table_name + ".tbl";
}

void TableManager::save_table(const string& table_name) {
    if (!table_exists(table_name)) {
        return;
    }
    
    string filename = get_table_file(table_name);
    ofstream file(filename, ios::binary);
    
    if (!file.is_open()) {
        return;
    }
    
    // Write schema
    const TableSchema& schema = schemas[table_name];
    uint32_t col_count = schema.columns.size();
    file.write((const char*)&col_count, sizeof(col_count));
    
    for (const auto& [col_name, col_type] : schema.columns) {
        uint32_t name_len = col_name.length();
        file.write((const char*)&name_len, sizeof(name_len));
        file.write(col_name.c_str(), name_len);
        
        uint32_t type_len = col_type.length();
        file.write((const char*)&type_len, sizeof(type_len));
        file.write(col_type.c_str(), type_len);
    }
    
    // Write rows
    const auto& rows = tables[table_name];
    uint32_t row_count = rows.size();
    file.write((const char*)&row_count, sizeof(row_count));
    
    for (const auto& row : rows) {
        uint32_t field_count = row.size();
        file.write((const char*)&field_count, sizeof(field_count));
        
        for (const auto& [col_name, col_value] : row) {
            uint32_t name_len = col_name.length();
            file.write((const char*)&name_len, sizeof(name_len));
            file.write(col_name.c_str(), name_len);
            
            uint32_t value_len = col_value.length();
            file.write((const char*)&value_len, sizeof(value_len));
            file.write(col_value.c_str(), value_len);
        }
    }
    
    file.close();
}

void TableManager::load_table(const string& table_name) {
    string filename = get_table_file(table_name);
    ifstream file(filename, ios::binary);
    
    if (!file.is_open()) {
        return;
    }
    
    // Read schema
    uint32_t col_count;
    file.read((char*)&col_count, sizeof(col_count));
    
    TableSchema schema;
    schema.table_name = table_name;
    
    for (uint32_t i = 0; i < col_count; i++) {
        uint32_t name_len;
        file.read((char*)&name_len, sizeof(name_len));
        string col_name(name_len, '\0');
        file.read(&col_name[0], name_len);
        
        uint32_t type_len;
        file.read((char*)&type_len, sizeof(type_len));
        string col_type(type_len, '\0');
        file.read(&col_type[0], type_len);
        
        schema.columns.push_back({col_name, col_type});
    }
    
    schemas[table_name] = schema;
    
    // Read rows
    uint32_t row_count;
    file.read((char*)&row_count, sizeof(row_count));
    
    vector<map<string, string>> rows;
    for (uint32_t i = 0; i < row_count; i++) {
        uint32_t field_count;
        file.read((char*)&field_count, sizeof(field_count));
        
        map<string, string> row;
        for (uint32_t j = 0; j < field_count; j++) {
            uint32_t name_len;
            file.read((char*)&name_len, sizeof(name_len));
            string col_name(name_len, '\0');
            file.read(&col_name[0], name_len);
            
            uint32_t value_len;
            file.read((char*)&value_len, sizeof(value_len));
            string col_value(value_len, '\0');
            file.read(&col_value[0], value_len);
            
            row[col_name] = col_value;
        }
        rows.push_back(row);
    }
    
    tables[table_name] = rows;
    file.close();
}

void TableManager::save_all() {
    for (const auto& [name, schema] : schemas) {
        save_table(name);
    }
}

void TableManager::load_all() {
    try {
        for (const auto& entry : fs::directory_iterator(db_dir)) {
            string filename = entry.path().filename().string();
            // Check if file ends with .tbl
            if (filename.length() >= 4 && 
                filename.substr(filename.length() - 4) == ".tbl") {
                string table_name = filename.substr(0, filename.length() - 4);
                load_table(table_name);
            }
        }
    } catch (...) {
        // Directory doesn't exist yet
    }
}
