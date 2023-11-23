
import os
import shutil
import config  # as config


def delete_database_file_name():
    with config.global_lock:
        try:
            if os.path.exists(config.database_file_name):
                os.remove(config.database_file_name)
        except Exception as e:
            print(f'cant delete the db file to reset it {e}')
        try:
            # delete this files that based on the database
            private_key = os.path.join(
                '..', 'cpp-client', 'cpp-client', 'priv.key')
            os.remove(private_key)
        except Exception as e:
            print(f'cant delete priv.key, {e}')

        try:
            identifier = os.path.join(
                '..', 'cpp-client', 'cpp-client', 'me.info')
            os.remove(identifier)
        except Exception as e:
            print(f'cant delete me.info, {e}')

        config.green_print(
            f'database is reset with private keys and identifier file')


def reset_stored_files_folder():
    with config.global_lock:
        if os.path.exists(config.received_files_folder_name):
            shutil.rmtree(config.received_files_folder_name)
        os.mkdir(config.received_files_folder_name)


def write_to_file(directory, file_name, input):
    with config.global_lock:
        try:
            # Create the directory path if it doesn't exist
            if not os.path.exists(directory):
                os.makedirs(directory)

            file_path = os.path.join(directory, file_name)
            with open(file_path, "wb") as file:
                file.write(input)
        except Exception as e:
            config.red_print(f'An error occurred: {str(e)}')
