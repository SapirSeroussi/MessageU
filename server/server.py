from info import ClientsManager, MessageInfo, DatabaseManager
from constants import *
from responses import *
from methods import *

import threading
import socket
import struct

db_manager = DatabaseManager()
clients_manager = ClientsManager(db_manager)

def precess_request(data):
    client_id, version, code, payload_size, payload = unpack_header(data)
    
    if version != VERSION:
        return create_header(ERROR_RESPONSE, 0)

    
    if code == REGISTER_REQUEST:
        # registration request - ignore the id
        if payload_size != NAME_SIZE+PUBLIC_KEY_SIZE:
            print("ERROR: a problem occure, the client cant be registered")
            return create_header(ERROR_RESPONSE, 0)
        name = payload[:NAME_SIZE].decode('utf-8').rstrip('\x00')
        public_key = payload[NAME_SIZE : NAME_SIZE+PUBLIC_KEY_SIZE] 

        header, uuid = successful_register_response()
        
        successfuly_added = clients_manager.add_client(uuid, name, public_key)
        if successfuly_added:
            print(f"Client '{name}' registered successfully.")
            return header + uuid
        else: 
            print(f"Client '{name}' registration failed.")
            return create_header(ERROR_RESPONSE, 0)
    
    # client_id dont exist
    if not clients_manager.client_exists(client_id):
        print(f"ERROR: id = {client_id} isnt found")
        return create_header(ERROR_RESPONSE, 0)
    
    clients_manager.update_last_seen(client_id)
    print(f"Client {client_id} last seen updated.")
    
    # clients list request
    # list length must devide by (ID_SIZE + NAME_SIZE) - member size
    if code == CLIENTS_LIST_REQUEST:
        if len(payload) != 0:
            print("ERROR: a problem occure, the list cant be sent")
            return create_header(ERROR_RESPONSE, 0)
        list, length = clients_list_response(client_id, clients_manager)
        if length % (ID_SIZE + NAME_SIZE) != 0:
            print("ERROR: a problem occure, the list cant be sent")
            return create_header(ERROR_RESPONSE, 0)
        print("Clients list sent successfully.")
        return list
    
    if code == PUBLIC_KEY_REQUEST:
        if payload_size != ID_SIZE:
            print("ERROR: a problem occure, the public key cant be sent")
            return create_header(ERROR_RESPONSE, 0)
        requested_client = clients_manager.get_client_by_id(payload)

        if requested_client:
            print("Public key sent successfully.")
            return public_key_response(requested_client.get_client_id(), requested_client.get_public_key())
        print("ERROR: the requested id isnt found, the public key cant be sent")
        return create_header(ERROR_RESPONSE, 0)

    if code == SEND_MESSAGE_REQUEST:
        if len(payload) < MESSAGE_HEADER:
            print("ERROR: a problem occure, the message cant be sent")
            return create_header(ERROR_RESPONSE, 0)

        recipient_id, message_type, message_size = struct.unpack(f'<{ID_SIZE}s B I', payload[: MESSAGE_HEADER])

        if len(payload) != MESSAGE_HEADER + message_size:
            print("ERROR: a problem occure, the message cant be sent")
            return create_header(ERROR_RESPONSE, 0)
        
        content = payload[MESSAGE_HEADER : MESSAGE_HEADER + message_size]
        
        if not check_type(message_type, message_size):
            print("ERROR: a problem occure, the message cant be sent")
            return create_header(ERROR_RESPONSE, 0)
        
        if not clients_manager.client_exists(recipient_id):
            print("ERROR: the recipient id isnt found, the message cant be sent")
            return create_header(ERROR_RESPONSE, 0) 

        msg = MessageInfo(recipient_id, client_id, message_type,content)
        clients_manager.add_message(recipient_id, msg)

        print("Message sent successfully.")
        return messages_sent_response(recipient_id, msg.get_message_id())

    if code == PENDING_MESSAGES_REQUEST:
        if len(payload) != 0:
            return create_header(ERROR_RESPONSE, 0)
        print("Pending messages sent successfully")
        return pending_message_response(clients_manager, client_id)
        

def handle_client(client_socket, client_address):
    print(f"Connection from {client_address}")
    with client_socket:
        while True:
            data = client_socket.recv(BUFFER_SIZE)
            if not data:
                break  
        
            response = precess_request(data)
            client_socket.sendall(response)
            

if __name__ == "__main__":
    port = get_port()
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as server_socket:
        server_socket.bind((SERVER_IP, port))
        server_socket.listen(5)  # Allows up to 5 clients in queue
        print(f"Server listening on {SERVER_IP}:{port}")

        while True:
            client_socket, client_address = server_socket.accept()
            client_thread = threading.Thread(target=handle_client, args=(client_socket, client_address))
            client_thread.start() 


