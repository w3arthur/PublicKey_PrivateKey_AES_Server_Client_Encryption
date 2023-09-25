import sqlite3

# Connect to the SQLite database
conn = sqlite3.connect('defensive.db')
cursor = conn.cursor()

# Execute an SQL query to retrieve data from the "client" table
cursor.execute('SELECT * FROM client')

# Fetch and print all rows
rows = cursor.fetchall()
for row in rows:
    print(row)

# Close the database connection
conn.close()
