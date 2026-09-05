#ifndef EXECUTOR_H
#define EXECUTOR_H

#include "parser.h"
#include "table_manager.h"
#include <vector>
#include <map>
#include <string>
#include <memory>

using namespace std;

struct Row {
    map<string, string> data;
};

struct QueryResult {
    vector<string> columns;
    vector<Row> rows;
    bool success;
    string error_message;
};

class Executor {
private:
    TableManager* table_manager;
    
public:
    Executor();
    ~Executor();
    
    QueryResult* execute(Query* query);
    
private:
    QueryResult* execute_select(SelectQuery* query);
    QueryResult* execute_insert(InsertQuery* query);
    QueryResult* execute_delete(DeleteQuery* query);
    QueryResult* execute_create(CreateQuery* query);
    
    bool evaluate_condition(const Row& row, Condition* cond);
    QueryResult* error(const string& msg);
    QueryResult* success();
};

#endif
