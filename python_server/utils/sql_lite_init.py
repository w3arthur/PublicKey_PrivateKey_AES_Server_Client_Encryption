import os
import sqlite3
import config  # as config


def create_clients_table():
    with config.global_lock:
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


def create_files_table():  # run secondary
    with config.global_lock:
        conn = sqlite3.connect(config.database_file_name)
        cursor = conn.cursor()
        query = f'''CREATE TABLE IF NOT EXISTS {config.files_table_name} (
                UserId VARCHAR(160),
                FileName VARCHAR(255),
                PathName VARCHAR(255),
                Verified BOOL DEFAULT 0,
                FOREIGN KEY (UserId) REFERENCES {config.clients_table_name}(ID)
            )'''

        cursor.execute(query)

        conn.commit()
        conn.close()
