if __name__ != "__main__":
    import sqlite3
    import uuid
    from config import database_name, clients_table_name, files_table_name

    def check_client(client_name) -> bool:
        # Connect to the SQLite database
        conn = sqlite3.connect(database_name)
        cursor = conn.cursor()

        data = (client_name,)
        cursor.execute("SELECT Name FROM clients WHERE Name = ?", data)
        result = cursor.fetchone()

        conn.commit()
        conn.close()
        return result is not None

    def register_client(client_name) -> bytes:
        # Connect to the SQLite database
        conn = sqlite3.connect(database_name)
        cursor = conn.cursor()

        # Insert data into the "client" table
        unique_id = uuid.uuid4()

        data_to_insert = (str(unique_id), client_name,)

        cursor.execute(
            f"INSERT INTO {clients_table_name} (ID, Name) VALUES (?, ?)", data_to_insert)
        last_row_id = cursor.lastrowid

        cursor.execute(
            "SELECT ID, Name FROM clients WHERE ROWID = ?", (last_row_id,))
        result = cursor.fetchone()
        if result is not None:
            last_inserted_id = result[0]
            last_inserted_name = result[1]

        conn.commit()
        conn.close()
        return uuid.UUID(last_inserted_id).bytes

    def insert_client(client_name, public_key, last_seen, aes_key):
        # Connect to the SQLite database
        conn = sqlite3.connect(database_name)
        cursor = conn.cursor()

        # Insert data into the "client" table
        data_to_insert = (client_name, public_key, last_seen, aes_key)
        cursor.execute('''
            INSERT INTO client (Name, PublicKey, LastSeen, AESKey)
            VALUES (?, ?, ?, ?)
        ''', data_to_insert)

        # Commit the changes and close the database connection
        conn.commit()
        conn.close()
