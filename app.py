"""
MiniDB - Streamlit Web Interface
Communicates with the compiled minidb C++ backend via subprocess.
"""

import streamlit as st
import subprocess
import pandas as pd
import re
import os
import time

# ──────────────────────────────────────────────────────────────
#  Config
# ──────────────────────────────────────────────────────────────
BASE_DIR = os.path.dirname(os.path.abspath(__file__))
BUILD_DIR = os.path.join(BASE_DIR, "build")
# Cross-platform: minidb on Linux/macOS, minidb.exe on Windows
MINIDB_EXE = os.path.join(BUILD_DIR, "minidb.exe" if os.name == "nt" else "minidb")

# ──────────────────────────────────────────────────────────────
#  Auto-build on startup (for Streamlit Cloud)
# ──────────────────────────────────────────────────────────────
def ensure_minidb_built():
    """Build minidb if executable doesn't exist."""
    if os.path.exists(MINIDB_EXE):
        return True
    
    if os.name == "nt":
        # On Windows, we can't easily build from Python
        return False
    
    # Linux/macOS: try to build with cmake/make
    try:
        os.makedirs(BUILD_DIR, exist_ok=True)
        result = subprocess.run(
            ["cmake", ".."],
            cwd=BUILD_DIR,
            capture_output=True,
            text=True,
            timeout=60,
        )
        if result.returncode != 0:
            return False
        
        result = subprocess.run(
            ["make", "-j", str(os.cpu_count() or 4)],
            cwd=BUILD_DIR,
            capture_output=True,
            text=True,
            timeout=120,
        )
        if result.returncode != 0:
            return False
        
        # Make executable
        os.chmod(MINIDB_EXE, 0o755)
        return os.path.exists(MINIDB_EXE)
    except Exception:
        return False

# Build on import (runs once per session)
ensure_minidb_built()

st.set_page_config(
    page_title="MiniDB — SQL Interface",
    page_icon="🗄️",
    layout="wide",
    initial_sidebar_state="expanded",
)

# ──────────────────────────────────────────────────────────────
#  Custom CSS
# ──────────────────────────────────────────────────────────────
st.markdown("""
<style>
@import url('https://fonts.googleapis.com/css2?family=Inter:wght@300;400;500;600;700&family=JetBrains+Mono:wght@400;500&display=swap');

html, body, [class*="css"] {
    font-family: 'Inter', sans-serif;
}

/* ── Page background ── */
.stApp {
    background: linear-gradient(135deg, #0f0f1a 0%, #12121f 50%, #0a0a14 100%);
    color: #e2e8f0;
}

/* ── Sidebar ── */
[data-testid="stSidebar"] {
    background: linear-gradient(180deg, #13131f 0%, #0e0e1c 100%);
    border-right: 1px solid rgba(139,92,246,0.2);
}

/* ── Header ── */
.minidb-header {
    background: linear-gradient(135deg, rgba(139,92,246,0.15) 0%, rgba(59,130,246,0.15) 100%);
    border: 1px solid rgba(139,92,246,0.3);
    border-radius: 16px;
    padding: 24px 32px;
    margin-bottom: 24px;
    backdrop-filter: blur(10px);
}
.minidb-header h1 {
    font-size: 2rem;
    font-weight: 700;
    background: linear-gradient(135deg, #a78bfa, #60a5fa);
    -webkit-background-clip: text;
    -webkit-text-fill-color: transparent;
    margin: 0 0 4px 0;
}
.minidb-header p {
    color: #94a3b8;
    margin: 0;
    font-size: 0.9rem;
}

/* ── Status badges ── */
.badge-success {
    display: inline-flex;
    align-items: center;
    gap: 6px;
    background: rgba(16,185,129,0.15);
    border: 1px solid rgba(16,185,129,0.4);
    color: #34d399;
    padding: 4px 12px;
    border-radius: 20px;
    font-size: 0.8rem;
    font-weight: 600;
}
.badge-error {
    display: inline-flex;
    align-items: center;
    gap: 6px;
    background: rgba(239,68,68,0.15);
    border: 1px solid rgba(239,68,68,0.4);
    color: #f87171;
    padding: 4px 12px;
    border-radius: 20px;
    font-size: 0.8rem;
    font-weight: 600;
}

/* ── Schema pill ── */
.schema-pill {
    background: rgba(59,130,246,0.12);
    border: 1px solid rgba(59,130,246,0.25);
    border-radius: 8px;
    padding: 8px 12px;
    margin-bottom: 6px;
    font-size: 0.8rem;
}
.schema-table-name {
    font-weight: 600;
    color: #60a5fa;
    font-family: 'JetBrains Mono', monospace;
}
.schema-cols {
    color: #94a3b8;
    font-family: 'JetBrains Mono', monospace;
    font-size: 0.73rem;
    margin-top: 2px;
}

/* ── Textarea styling ── */
.stTextArea textarea {
    font-family: 'JetBrains Mono', monospace !important;
    font-size: 0.9rem !important;
    background: rgba(0,0,0,0.3) !important;
    border: 1px solid rgba(139,92,246,0.3) !important;
    border-radius: 8px !important;
    color: #e2e8f0 !important;
}
.stTextArea textarea:focus {
    border-color: rgba(139,92,246,0.7) !important;
    box-shadow: 0 0 0 2px rgba(139,92,246,0.15) !important;
}

/* ── Buttons ── */
.stButton > button {
    background: linear-gradient(135deg, #7c3aed, #4f46e5) !important;
    color: white !important;
    border: none !important;
    border-radius: 8px !important;
    font-weight: 600 !important;
    transition: all 0.2s !important;
}
.stButton > button:hover {
    background: linear-gradient(135deg, #6d28d9, #4338ca) !important;
    box-shadow: 0 4px 15px rgba(139,92,246,0.4) !important;
}

/* ── Metrics ── */
[data-testid="stMetric"] {
    background: rgba(255,255,255,0.04);
    border: 1px solid rgba(255,255,255,0.08);
    border-radius: 10px;
    padding: 12px 16px;
}

/* ── Divider ── */
hr { border-color: rgba(255,255,255,0.08) !important; }
</style>
""", unsafe_allow_html=True)


# ──────────────────────────────────────────────────────────────
#  Session State
# ──────────────────────────────────────────────────────────────
if "history" not in st.session_state:
    st.session_state.history = []
if "editor_sql" not in st.session_state:
    st.session_state.editor_sql = "CREATE TABLE users (id INT, name TEXT, age INT)"
if "schema" not in st.session_state:
    st.session_state.schema = {}
if "total_queries" not in st.session_state:
    st.session_state.total_queries = 0
if "total_rows" not in st.session_state:
    st.session_state.total_rows = 0


# ──────────────────────────────────────────────────────────────
#  Backend Communication
# ──────────────────────────────────────────────────────────────
def run_query(sql: str) -> dict:
    """Send sql to minidb via stdin and parse stdout."""
    exe_name = "minidb.exe" if os.name == "nt" else "minidb"
    if not os.path.exists(MINIDB_EXE):
        return {
            "success": False,
            "error": f"{exe_name} not found at:\n{MINIDB_EXE}\n\nPlease build the project first (cmake + make).",
            "columns": [], "rows": [], "raw": "", "elapsed_ms": 0,
        }

    t0 = time.perf_counter()
    try:
        proc = subprocess.run(
            [MINIDB_EXE],
            input=sql.strip() + "\nEXIT\n",
            capture_output=True,
            text=True,
            encoding="utf-8",
            errors="replace",
            timeout=10,
        )
        elapsed_ms = int((time.perf_counter() - t0) * 1000)
        raw = proc.stdout or ""
        stderr = proc.stderr or ""

        result = parse_output(raw, sql)
        result["elapsed_ms"] = elapsed_ms
        result["raw"] = raw.strip()
        if not result["success"] and stderr:
            result["error"] = stderr.strip()
        return result

    except subprocess.TimeoutExpired:
        return {"success": False, "error": "Query timed out (>10 s)", "columns": [], "rows": [], "raw": "", "elapsed_ms": 0}
    except Exception as e:
        return {"success": False, "error": str(e), "columns": [], "rows": [], "raw": "", "elapsed_ms": 0}


def _clean_output_lines(raw: str) -> list[str]:
    """Normalize minidb.exe stdout into content lines."""
    lines = [l.strip() for l in raw.splitlines() if l.strip()]
    cleaned = []
    for line in lines:
        if line.startswith("minidb>"):
            # DML/DDL results are often printed on the same line as the prompt.
            rest = line[len("minidb>"):].strip()
            if rest:
                cleaned.append(rest)
            continue
        if line.startswith(("╔", "║", "╚")) or line == "Goodbye!":
            continue
        cleaned.append(line)
    return cleaned


def parse_output(raw: str, sql: str) -> dict:
    """Parse text output from minidb.exe into structured result."""
    lines = _clean_output_lines(raw)

    # Error detection
    for line in lines:
        if line.startswith("❌") or line.lower().startswith("error:"):
            return {"success": False, "error": line.lstrip("❌ ").strip(), "columns": [], "rows": []}

    sql_upper = sql.strip().upper()

    # ── SELECT: look for "Columns:" header then tabular data ──
    if sql_upper.startswith("SELECT"):
        columns, rows = _parse_select_lines(lines)
        if columns is not None:
            return {"success": True, "error": "", "columns": columns, "rows": rows}
        return {"success": True, "error": "", "columns": [], "rows": []}

    # ── DML / DDL success ──
    success_keywords = [
        "created", "inserted", "deleted", "row inserted",
        "rows deleted", "✓", "ok", "query executed"
    ]
    joined = " ".join(lines).lower()
    if any(kw in joined for kw in success_keywords):
        return {"success": True, "error": "", "columns": [], "rows": []}

    return {"success": bool(lines), "error": "", "columns": [], "rows": []}


def _parse_select_lines(lines: list) -> tuple:
    """
    Parse lines like:
      Columns: id | name | age |
      --------------------------------------------------
      1 | Alice | 30 |
    """
    col_idx = None
    for i, line in enumerate(lines):
        if line.lower().startswith("columns:"):
            col_idx = i
            break

    if col_idx is None:
        return None, None

    col_raw = lines[col_idx][len("columns:"):].strip()
    columns = [c.strip() for c in col_raw.split("|") if c.strip()]
    if not columns:
        return None, None

    data_start = col_idx + 1
    if data_start < len(lines) and lines[data_start].startswith("-"):
        data_start += 1

    rows = []
    for line in lines[data_start:]:
        if re.match(r"^\d+ rows? returned", line.lower()):
            break
        if line.startswith("-") or line.startswith("("):
            continue
        parts = [p.strip() for p in line.split("|") if p.strip() != ""]
        if len(parts) == len(columns):
            rows.append(dict(zip(columns, parts)))
        elif parts:
            padded = (parts + [""] * len(columns))[:len(columns)]
            rows.append(dict(zip(columns, padded)))

    return columns, rows


def infer_schema_from_sql(sql: str):
    """Track schema from CREATE TABLE statements."""
    m = re.match(
        r"CREATE\s+TABLE\s+(\w+)\s*\((.+)\)",
        sql.strip(), re.IGNORECASE | re.DOTALL
    )
    if m:
        table = m.group(1)
        cols_raw = m.group(2)
        cols = []
        for col_def in cols_raw.split(","):
            parts = col_def.strip().split()
            if len(parts) >= 2:
                cols.append({"name": parts[0], "type": parts[1]})
            elif parts:
                cols.append({"name": parts[0], "type": "?"})
        st.session_state.schema[table] = cols


# ──────────────────────────────────────────────────────────────
#  Sidebar
# ──────────────────────────────────────────────────────────────
with st.sidebar:
    st.markdown("""
    <div style="padding: 4px 0 16px 0;">
        <span style="font-size:1.5rem;">🗄️</span>
        <span style="font-size:1.1rem; font-weight:700; color:#a78bfa; margin-left:8px;">MiniDB</span>
        <span style="font-size:0.7rem; color:#64748b; margin-left:6px;">v1.0</span>
    </div>
    """, unsafe_allow_html=True)

    # Stats
    col1, col2 = st.columns(2)
    col1.metric("Queries", st.session_state.total_queries)
    col2.metric("Rows fetched", st.session_state.total_rows)

    st.markdown("---")

    # ── Schema Explorer ──
    st.markdown("**📐 Schema Explorer**")
    if st.session_state.schema:
        for tbl, cols in st.session_state.schema.items():
            col_str = ", ".join(
                f"{c['name']} <span style='color:#6366f1'>{c['type']}</span>"
                for c in cols
            )
            st.markdown(f"""
            <div class="schema-pill">
                <div class="schema-table-name">📋 {tbl}</div>
                <div class="schema-cols">{col_str}</div>
            </div>
            """, unsafe_allow_html=True)
    else:
        st.caption("No tables yet")

    st.markdown("---")

    # ── Quick Templates ──
    st.markdown("**⚡ Quick Templates**")
    templates = {
        "Create Table": "CREATE TABLE users (id INT, name TEXT, age INT)",
        "Insert Row":   "INSERT INTO users (id, name, age) VALUES (1, 'Alice', 30)",
        "Select All":   "SELECT * FROM users",
        "Select WHERE": "SELECT name, age FROM users WHERE age > 25",
        "Delete Row":   "DELETE FROM users WHERE id = 1",
    }
    for label, tmpl in templates.items():
        if st.button(label, key=f"tmpl_{label}", use_container_width=True):
            st.session_state.editor_sql = tmpl
            st.rerun()

    st.markdown("---")

    # ── Query History ──
    st.markdown("**🕒 Query History**")
    if st.session_state.history:
        for i, (sql, _) in enumerate(reversed(st.session_state.history[-10:])):
            truncated = sql[:55] + ("…" if len(sql) > 55 else "")
            if st.button(truncated, key=f"hist_{i}", use_container_width=True, help=sql):
                st.session_state.editor_sql = sql
                st.rerun()
        if st.button("🗑️ Clear History", use_container_width=True):
            st.session_state.history = []
            st.rerun()
    else:
        st.caption("No queries yet")

    st.markdown("---")
    binary_ok = os.path.exists(MINIDB_EXE)
    exe_name = "minidb.exe" if os.name == "nt" else "minidb"
    st.markdown(f"""
    <div style="font-size:0.72rem; color:#334155; text-align:center;">
        Backend: <code style="color:#7c3aed">{exe_name}</code><br>
        {"✅ Binary found" if binary_ok else "❌ Binary not found — run cmake build"}
    </div>
    """, unsafe_allow_html=True)


# ──────────────────────────────────────────────────────────────
#  Main Area
# ──────────────────────────────────────────────────────────────
st.markdown("""
<div class="minidb-header">
    <h1>🗄️ MiniDB SQL Interface</h1>
    <p>Write and execute SQL against your custom C++ database engine</p>
</div>
""", unsafe_allow_html=True)

# ── SQL Editor ──
st.markdown("**📝 SQL Editor**")

sql_input = st.text_area(
    label="SQL Editor",
    height=130,
    placeholder="Enter SQL here…  e.g.  SELECT * FROM users WHERE age > 25",
    label_visibility="collapsed",
    key="editor_sql",
)

run_col, clear_col, _ = st.columns([1, 1, 6])
with run_col:
    run_clicked = st.button("▶  Run Query", type="primary", use_container_width=True)
with clear_col:
    if st.button("✕  Clear", use_container_width=True):
        st.session_state.editor_sql = ""
        st.rerun()

st.markdown("---")

# ── Execute ──
if run_clicked and sql_input.strip():
    with st.spinner("Executing…"):
        result = run_query(sql_input.strip())

    # Update schema tracker
    infer_schema_from_sql(sql_input.strip())

    # Track stats
    st.session_state.total_queries += 1
    st.session_state.history.append((sql_input.strip(), result))

    # ── Status badge ──
    if result["success"]:
        st.markdown(
            f'<span class="badge-success">✓ Success &nbsp;·&nbsp; {result["elapsed_ms"]} ms</span>',
            unsafe_allow_html=True,
        )
    else:
        st.markdown('<span class="badge-error">✗ Error</span>', unsafe_allow_html=True)
        st.error(result.get("error", "Unknown error"))

    # ── SELECT result table ──
    if result["success"] and result["columns"]:
        df = pd.DataFrame(result["rows"], columns=result["columns"])
        st.session_state.total_rows += len(df)

        st.markdown(f"**Results** — `{len(df)}` row(s) · `{len(result['columns'])}` column(s)")
        st.dataframe(df, use_container_width=True, hide_index=True)

        csv = df.to_csv(index=False)
        st.download_button(
            "⬇ Download CSV",
            data=csv,
            file_name="minidb_result.csv",
            mime="text/csv",
        )
    elif result["success"] and not result["columns"]:
        st.success("Query executed successfully (no rows returned)")

    # ── Raw output ──
    if result.get("raw"):
        with st.expander("📄 Raw output from minidb"):
            st.code(result["raw"], language=None)

elif run_clicked and not sql_input.strip():
    st.warning("Please enter a SQL query first.")

# ── Show last result when nothing just ran ──
elif st.session_state.history and not run_clicked:
    last_sql, last_result = st.session_state.history[-1]
    st.markdown(f"**Last query:** `{last_sql[:80]}{'…' if len(last_sql) > 80 else ''}`")
    if last_result["success"] and last_result["columns"]:
        df = pd.DataFrame(last_result["rows"], columns=last_result["columns"])
        st.dataframe(df, use_container_width=True, hide_index=True)
    elif last_result["success"]:
        st.success("Last query executed successfully")
    else:
        st.error(last_result.get("error", ""))

else:
    # Welcome state
    st.markdown("""
    <div style="
        text-align: center;
        padding: 48px 24px;
        border: 1px dashed rgba(139,92,246,0.2);
        border-radius: 12px;
        color: #475569;
    ">
        <div style="font-size:3rem; margin-bottom:12px;">⌨️</div>
        <div style="font-size:1rem; color:#64748b;">
            Write a SQL query above and click <strong style="color:#a78bfa">Run Query</strong>
        </div>
        <div style="font-size:0.8rem; margin-top:8px; color:#334155;">
            Or pick a template from the sidebar →
        </div>
    </div>
    """, unsafe_allow_html=True)
