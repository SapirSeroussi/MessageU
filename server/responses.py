from constants import *
import struct
import uuid

def create_header(code, payload_size):
    return struct.pack("<B H I", VERSION, code, payload_size)

def successful_register_response():
    uuid_bytes = uuid.uuid4().bytes
    header = create_header(SUCCESSFUL_REGISTER_RESPONSE, len(uuid_bytes))
    return header, uuid_bytes

def clients_list_response(current_client_id, clients_manager):
    map_without_curr = {client.get_client_id(): client.get_name() for client in clients_manager.get_clients_list() if client.get_client_id() != current_client_id}
    
    serialized_data = b""
    for client_id, name in map_without_curr.items():
        serialized_data += client_id + name.encode().ljust(NAME_SIZE, b'\x00')

    size = len(serialized_data)
    header = create_header(CLIENTS_LIST_RESPONSE, size)
    return header + serialized_data, size

def public_key_response(requested_client_id, requested_public_key):
    payload = requested_client_id + requested_public_key
    header = create_header(PUBLIC_KEY_RESPONSE, len(payload))
    return header + payload


def messages_sent_response(recipient_id, message_id):
    message_id = str(message_id).encode('utf-8').ljust(MESSAGE_ID_SIZE, b'\x00')
    payload = recipient_id + message_id
    header = create_header(MESSAGE_SENT_RESPONSE, len(payload))
    return header + payload

def pending_message_response(clients_manager, client_id):
    messages = clients_manager.get_bytes_messages_list(client_id)
    
    serialized_data = b"".join(messages) 
    header = create_header(PENDING_MESSAGES_RESPONSE, len(serialized_data))
    return header + serialized_data
