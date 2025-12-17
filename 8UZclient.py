# 8. Übungszettel - NPVP
# Hristo Popovski & Tomy Duong

import socket

# 2.1. TCP-Client:

HOST = "127.0.0.1"
PORT = 65432

def run_client():
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
        s.connect((HOST, PORT))
        data = s.recv(1024)
        print(f"Received from server: {data.decode()}")

run_client()



