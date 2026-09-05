import streamlit as st
import subprocess
import os
import sys

st.set_page_config(page_title="Mini SQL Database", layout="wide")

st.title("🗄️ Mini SQL Database")
st.markdown("Interactive SQL Database - Create, Insert, Select, Delete")

# Sidebar info
with st.sidebar:
    st.markdown("""
    ### 📖 Quick Guide
    
    **CREATE TABLE**
```sql
    CREATE TABLE users (id INT, name TEXT, age INT)
```
    
    **INSERT**
```sql
    INSERT INTO users (id, name, age) VALUES (1, 'Alice', 30)
```
    
    **SELECT**
```sql
    SELECT * FROM users WHERE age > 25
```
    
    **DELETE**
```sql
    DELETE FROM users WHERE id = 1
```
    """)

# Text input for SQL query
sql_query = st.text_area(
    "Enter SQL Query:",
    placeholder="SELECT * FROM users",
    height=100
)

# Execute button
if st.button("Execute Query", type="primary"):
    if sql_query.strip():
        st.info(f"Executing: `{sql_query}`")
        
        # Run the query through minidb
        try:
            # Write query to temp file
            with open("temp_query.sql", "w") as f:
                f.write(sql_query)
            
            # Execute minidb with query
            result = subprocess.run(
                [sys.executable, "query_runner.py", sql_query],
                capture_output=True,
                text=True,
                timeout=5
            )
            
            if result.returncode == 0:
                output = result.stdout
                if output.strip():
                    st.success("✅ Query executed successfully!")
                    st.text(output)
                else:
                    st.success("✅ Query executed successfully!")
            else:
                st.error(f"❌ Error: {result.stderr}")
                
        except subprocess.TimeoutExpired:
            st.error("❌ Query timeout")
        except Exception as e:
            st.error(f"❌ Error: {str(e)}")
    else:
        st.warning("Please enter a SQL query")

# Display available tables
st.markdown("---")
st.subheader("📊 Database Info")

try:
    result = subprocess.run(
        [sys.executable, "query_runner.py", "SHOW TABLES"],
        capture_output=True,
        text=True,
        timeout=5
    )
    if result.returncode == 0 and result.stdout.strip():
        st.text("Available tables:\n" + result.stdout)
except:
    st.info("No tables yet. Create one with CREATE TABLE command.")
