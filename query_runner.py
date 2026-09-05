import sys
import os
import ctypes

# Add include path for our C++ library
sys.path.insert(0, os.path.dirname(__file__))

# Mock C++ interface for Python
class TokenType:
    KEYWORD = 0
    IDENTIFIER = 1
    NUMBER = 2
    STRING = 3
    OPERATOR = 4
    SYMBOL = 5
    END_OF_INPUT = 6

class QueryExecutor:
    def __init__(self):
        self.tables = {}
        self.load_from_disk()
    
    def execute(self, query_str):
        query_str = query_str.strip().upper()
        
        if query_str.startswith("CREATE TABLE"):
            return self.handle_create(query_str)
        elif query_str.startswith("INSERT"):
            return self.handle_insert(query_str)
        elif query_str.startswith("SELECT"):
            return self.handle_select(query_str)
        elif query_str.startswith("DELETE"):
            return self.handle_delete(query_str)
        elif query_str == "SHOW TABLES":
            return self.show_tables()
        else:
            return "Unknown command"
    
    def show_tables(self):
        if not self.tables:
            return "No tables"
        return "Tables:\n" + "\n".join(self.tables.keys())
    
    def handle_create(self, query):
        # Simple CREATE TABLE parser
        try:
            parts = query.split("(")
            table_name = parts[0].replace("CREATE TABLE", "").strip()
            cols_def = parts[1].replace(")", "").strip()
            
            self.tables[table_name] = {
                "columns": [c.strip().split() for c in cols_def.split(",")],
                "rows": []
            }
            
            self.save_to_disk()
            return f"Table '{table_name}' created"
        except Exception as e:
            return f"Error: {str(e)}"
    
    def handle_insert(self, query):
        try:
            # Simple INSERT parser
            parts = query.split("VALUES")
            table_part = parts[0].replace("INSERT INTO", "").strip()
            table_name = table_part.split("(")[0].strip()
            
            # Extract values
            values_str = parts[1].strip().replace("(", "").replace(")", "").strip()
            values = [v.strip().strip("'\"") for v in values_str.split(",")]
            
            if table_name in self.tables:
                row = dict(zip(
                    [c[0] for c in self.tables[table_name]["columns"]],
                    values
                ))
                self.tables[table_name]["rows"].append(row)
                self.save_to_disk()
                return "1 row inserted"
            else:
                return "Table not found"
        except Exception as e:
            return f"Error: {str(e)}"
    
    def handle_select(self, query):
        try:
            parts = query.split("FROM")
            table_name = parts[1].split("WHERE")[0].strip()
            
            if table_name not in self.tables:
                return "Table not found"
            
            rows = self.tables[table_name]["rows"]
            cols = [c[0] for c in self.tables[table_name]["columns"]]
            
            # Format output
            result = " | ".join(cols) + "\n"
            result += "-" * 50 + "\n"
            for row in rows:
                result += " | ".join(str(row.get(c, "")) for c in cols) + "\n"
            
            return result
        except Exception as e:
            return f"Error: {str(e)}"
    
    def handle_delete(self, query):
        try:
            parts = query.split("WHERE")
            table_name = parts[0].replace("DELETE FROM", "").strip()
            
            if table_name in self.tables:
                # Simple delete (remove all for demo)
                self.tables[table_name]["rows"] = []
                self.save_to_disk()
                return "Rows deleted"
            return "Table not found"
        except Exception as e:
            return f"Error: {str(e)}"
    
    def save_to_disk(self):
        import json
        with open("minidb_data.json", "w") as f:
            json.dump(self.tables, f)
    
    def load_from_disk(self):
        import json
        try:
            with open("minidb_data.json", "r") as f:
                self.tables = json.load(f)
        except:
            self.tables = {}

if __name__ == "__main__":
    if len(sys.argv) > 1:
        query = " ".join(sys.argv[1:])
        executor = QueryExecutor()
        result = executor.execute(query)
        print(result)
    else:
        print("No query provided")
