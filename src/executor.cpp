#include "executor.h"
#include <iostream>
#include <sstream>

using namespace std;

Executor::Executor() {}

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
    if (tables.find(query->table) != tables.end()) {
        return error("Table already exists");
    }
    
    tables[query->table] = vector<Row>();
    schemas[query->table] = query->columns;
    
    return success();
}

QueryResult* Executor::execute_insert(InsertQuery* query) {
    if (tables.find(query->table) == tables.end()) {
        return error("Table does not exist");
    }
    
    if (query->columns.size() != query->values.size()) {
        return error("Column count mismatch");
    }
    
    Row row;
    for (size_t i = 0; i < query->columns.size(); i++) {
        row.data[query->columns[i]] = query->values[i];
    }
    
    tables[query->table].push_back(row);
    
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
    
    if (tables.find(query->table) == tables.end()) {
        result->success = false;
        result->error_message = "Table does not exist";
        return result;
    }
    
    // Set columns
    if (query->columns[0] == "*") {
        if (schemas.find(query->table) != schemas.end()) {
            for (const auto& col : schemas[query->table]) {
                result->columns.push_back(col.first);
            }
        }
    } else {
        result->columns = query->columns;
    }
    
    // Filter and collect rows
    for (const auto& row : tables[query->table]) {
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
    if (tables.find(query->table) == tables.end()) {
        return error("Table does not exist");
    }
    
    if (!query->where) {
        return error("DELETE requires WHERE clause");
    }
    
    auto& rows = tables[query->table];
    auto it = rows.begin();
    while (it != rows.end()) {
        if (evaluate_condition(*it, query->where)) {
            it = rows.erase(it);
        } else {
            ++it;
        }
    }
    
    return success();
}
