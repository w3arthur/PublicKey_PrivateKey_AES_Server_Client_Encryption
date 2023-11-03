if __name__ != "__main__":
    import os
    import sqlite3
    import config  # as config

    def delete_database_file_name():
        if os.path.exists(config.database_file_name):
            os.remove(config.database_file_name)

    def create_clients_table():
        conn = sqlite3.connect(config.database_file_name)
        cursor = conn.cursor()
        query = f'''CREATE TABLE IF NOT EXISTS {config.clients_table_name} (
                ID VARCHAR(160) PRIMARY KEY,
                Name VARCHAR(255),
                PublicKey VARCHAR(160),
                LastSeen DATETIME DEFAULT CURRENT_TIMESTAMP,
                AESKey VARCHAR(128)
            )'''

        cursor.execute(query)

        conn.commit()
        conn.close()

    def create_files_table():
        conn = sqlite3.connect(config.database_file_name)
        cursor = conn.cursor()
        query = f'''CREATE TABLE IF NOT EXISTS {config.files_table_name} (
                ID VARCHAR(160) PRIMARY KEY,
                FileName VARCHAR(255),
                PathName VARCHAR(255),
                Verified BOOL
            )'''

        cursor.execute(query)

        conn.commit()
        conn.close()
