if __name__ != "__main__":
    import sqlite3
    from ..config import *

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
