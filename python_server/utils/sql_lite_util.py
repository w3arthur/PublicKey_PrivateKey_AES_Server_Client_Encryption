import datetime
import sqlite3
import uuid
import config  # as config


def update_last_seen_by_id(client_id):
    with config.global_lock:
        conn = sqlite3.connect(config.database_file_name)
        cursor = conn.cursor()

        # Define the new LastSeen value (e.g., using the current timestamp)
        new_last_seen = datetime.datetime.now().strftime(
            '%Y-%m-%d %H:%M:%S')  # Replace this with the desired timestamp

        # Construct the SQL query to update the LastSeen for a specific ID
        update_query = f'''
            UPDATE {config.clients_table_name}
            SET LastSeen = ?
            WHERE ID = ?
        '''

        # Execute the update query with the new LastSeen value and the specified ID
        cursor.execute(update_query, (new_last_seen, client_id))
        conn.commit()
        conn.close()


def check_client(client_name) -> bool:
    with config.global_lock:
        # Connect to the SQLite database
        conn = sqlite3.connect(config.database_file_name)
        cursor = conn.cursor()

        data = (client_name,)
        cursor.execute("SELECT Name FROM clients WHERE Name = ?", data)
        result = cursor.fetchone()

        conn.commit()
        conn.close()
        return result is not None


def register_client(unique_id, client_name) -> bytes:
    with config.global_lock:
        # Connect to the SQLite database
        conn = sqlite3.connect(config.database_file_name)
        cursor = conn.cursor()

        data_to_insert = (str(unique_id), client_name,)

        cursor.execute(
            f"INSERT INTO {config.clients_table_name} (ID, Name) VALUES (?, ?)", data_to_insert)
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


def update_keys_in_clients_table(client_id, public_key, aes_key):
    with config.global_lock:
        conn = sqlite3.connect(config.database_file_name)
        cursor = conn.cursor()

        # Update the PublicKey and AESKey for the specified ID
        update_query = f'''
            UPDATE {config.clients_table_name}
            SET PublicKey = ?,
                AESKey = ?
            WHERE ID = ?
        '''

        data_to_update = (public_key, aes_key, client_id)

        cursor.execute(update_query, data_to_update)
        conn.commit()
        conn.close()


def get_client_info_by_id(client_id):
    with config.global_lock:
        conn = sqlite3.connect(config.database_file_name)
        cursor = conn.cursor()

        # Retrieve the Name, PublicKey, and AESKey for the specified ID
        select_query = f'''
            SELECT Name, PublicKey, AESKey
            FROM {config.clients_table_name}
            WHERE ID = ?
        '''

        cursor.execute(select_query, (client_id,))
        result = cursor.fetchone()
        conn.close()

        if result:
            name, public_key_bytes, base64_aes_key = result
            return name, public_key_bytes, base64_aes_key
    return None, None, None


def insert_file_data(user_id, file_name, path_name):
    with config.global_lock:
        conn = sqlite3.connect(config.database_file_name)
        cursor = conn.cursor()

        # Insert data into the files table
        cursor.execute(f"INSERT INTO {config.files_table_name} (UserId, FileName, PathName) VALUES (?, ?, ?)",
                       (user_id, file_name, path_name))

        conn.commit()
        conn.close()


def update_verification_to_true(user_id, filename):
    with config.global_lock:
        conn = sqlite3.connect(config.database_file_name)
        cursor = conn.cursor()

        # Update the Verified column to True for the specified UserId and FileName
        cursor.execute(f"UPDATE {config.files_table_name} SET Verified = 1 WHERE UserId = ? AND FileName = ?",
                       (user_id, filename))

        conn.commit()
        conn.close()
