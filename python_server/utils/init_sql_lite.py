if __name__ != "__main__":
    import sqlite3
    from config import database_name, clients_table_name, files_table_name

    def create_clients_table():
        conn = sqlite3.connect(database_name)
        cursor = conn.cursor()
        query = f'''CREATE TABLE IF NOT EXISTS {clients_table_name} (
                ID INTEGER PRIMARY KEY,
                Name TEXT,
                PublicKey BLOB,
                LastSeen DATETIME,
                AESKey BLOB
            )'''

        cursor.execute(query)

        conn.commit()
        conn.close()

    def create_files_table():
        conn = sqlite3.connect(database_name)
        cursor = conn.cursor()
        query = f'''CREATE TABLE IF NOT EXISTS {files_table_name} (
                ID INTEGER PRIMARY KEY,
                FileName TEXT,
                PathName TEXT,
                Verified BOOL
            )'''

        cursor.execute(query)

        conn.commit()
        conn.close()
