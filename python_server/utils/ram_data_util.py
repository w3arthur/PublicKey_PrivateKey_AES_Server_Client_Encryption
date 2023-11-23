import datetime
import config  # as config


def update_last_seen_by_id(ram_object, client_id):
    with config.global_lock:
        try:
            ram_object[client_id]['last_seen'] = datetime.datetime.now().strftime(
                '%Y-%m-%d %H:%M:%S')
        except:
            pass  # may not work it the user not created
            # print('cant update last seen in ram')


def check_client(ram_object,  client_name) -> bool:
    with config.global_lock:
        for key, nested_dict in ram_object.items():
            if 'name' in nested_dict and nested_dict['name'] == client_name:
                return True
        return False


def register_client(ram_object, unique_id, client_name):
    with config.global_lock:
        ram_object[unique_id] = {'name': client_name, 'public_key': '', 'last_seen': datetime.datetime.now().strftime(
            '%Y-%m-%d %H:%M:%S'), 'aes_key': ''}
        return unique_id


def update_keys_in_clients_table(ram_object, client_id, public_key, aes_key):
    with config.global_lock:
        ram_object[client_id]['public_key'] = public_key
        ram_object[client_id]['aes_key'] = aes_key


def get_client_info_by_id(ram_object, client_id):
    with config.global_lock:
        try:
            return ram_object[client_id]['name'], ram_object[client_id]['public_key'], ram_object[client_id]['aes_key']
        except:
            return None, None, None


def insert_file_data(ram_array, user_id, file_name, path_name):
    with config.global_lock:
        ram_array.append(
            {'user_id': user_id, 'file_name': file_name, 'path_name': path_name, 'verified': False})


def update_verification_to_true(ram_array, user_id, filename):
    with config.global_lock:
        for file_data in ram_array:
            if file_data['user_id'] == user_id and file_data['file_name'] == filename:
                file_data['verified'] = True
