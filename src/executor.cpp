#include "executor.h"
#include <iostream>

using namespace std;

Executor::Executor() {
    table_manager = new TableManager("minidb_data");
}

Executor::~Executor() {
    if (table_manager) {
        table_manager->save_all();
        delete table_manager;
    }
}

QueryResult* Executor::error(const string& msg) {
    QueryResult* result = new QueryResult();
    result->success = false;
    result->error_message = msg;
    return result;
}

QueryResult* Executor::success() {
    QueryResult* result = new QueryResult();
    result->success = true;
    result->error_message = "";
    return result;
}

QueryResult* Executor::execute(Query* query) {
    if (query->type == QueryType::SELECT) {
        return execute_select((SelectQuery*)query);
    } else if (query->type == QueryType::INSERT) {
        return execute_insert((InsertQuery*)query);
    } else if (query->type == QueryType::DELETE) {
        return execute_delete((DeleteQuery*)query);
    } else if (query->type == QueryType::CREATE) {
        return execute_create((CreateQuery*)query);
    }
    return error("Unknown query type");
}

QueryResult* Executor::execute_create(CreateQuery* query) {
    if (!table_manager->create_table(query->table, query->columns)) {
        return error("Table already exists");
    }
    return success();
}

QueryResult* Executor::execute_insert(InsertQuery* query) {
    if (!table_manager->table_exists(query->table)) {
        return error("Table does not exist");
    }
    
    if (query->columns.size() != query->values.size()) {
        return error("Column count mismatch");
    }
    
    map<string, string> row;
    for (size_t i = 0; i < query->columns.size(); i++) {
        row[query->columns[i]] = query->values[i];
    }
    
    if (!table_manager->insert_row(query->table, row)) {
        return error("Failed to insert row");
    }
    
    return success();
}

bool Executor::evaluate_condition(const Row& row, Condition* cond) {
    auto it = row.data.find(cond->column);
    if (it == row.data.end()) {
        return false;
    }
    
    string row_value = it->second;
    string cond_value = cond->value;
    
    if (cond->op == "=") {
        return row_value == cond_value;
    } else if (cond->op == ">") {
        try {
            int rv = stoi(row_value);
            int cv = stoi(cond_value);
            return rv > cv;
        } catch (...) {
            return row_value > cond_value;
        }
    } else if (cond->op == "<") {
        try {
            int rv = stoi(row_value);
            int cv = stoi(cond_value);
            return rv < cv;
        } catch (...) {
            return row_value < cond_value;
        }
    } else if (cond->op == ">=") {
        try {
            int rv = stoi(row_value);
            int cv = stoi(cond_value);
            return rv >= cv;
        } catch (...) {
            return row_value >= cond_value;
        }
    } else if (cond->op == "<=") {
        try {
            int rv = stoi(row_value);
            int cv = stoi(cond_value);
            return rv <= cv;
        } catch (...) {
            return row_value <= cond_value;
        }
    } else if (cond->op == "!=") {
        return row_value != cond_value;
    }
    
    return false;
}

QueryResult* Executor::execute_select(SelectQuery* query) {
    QueryResult* result = new QueryResult();
    result->success = true;
    result->error_message = "";
    
    if (!table_manager->table_exists(query->table)) {
        result->success = false;
        result->error_message = "Table does not exist";
        return result;
    }
    
    auto schema = table_manager->get_schema(query->table);
    
    // Set columns
    if (query->columns[0] == "*") {
        for (const auto& col : schema.columns) {
            result->columns.push_back(col.first);
        }
    } else {
        result->columns = query->columns;
    }
    
    // Get all rows
    auto all_rows = table_manager->get_all_rows(query->table);
    
    // Filter and collect rows
    for (const auto& row_data : all_rows) {
        Row row;
        row.data = row_data;
        
        if (query->where) {
            if (!evaluate_condition(row, query->where)) {
                continue;
            }
        }
        result->rows.push_back(row);
    }
    
    return result;
}

QueryResult* Executor::execute_delete(DeleteQuery* query) {
    if (!table_manager->table_exists(query->table)) {
        return error("Table does not exist");
    }
    
    if (!query->where) {
        return error("DELETE requires WHERE clause");
    }
    
    auto all_rows = table_manager->get_all_rows(query->table);
    vector<int> to_delete;
    
    for (size_t i = 0; i < all_rows.size(); i++) {
        Row row;
        row.data = all_rows[i];
        if (evaluate_condition(row, query->where)) {
            to_delete.push_back(i);
        }
    }
    
    if (!table_manager->delete_rows(query->table, to_delete)) {
        return error("Failed to delete rows");
    }
    
    return success();
}
