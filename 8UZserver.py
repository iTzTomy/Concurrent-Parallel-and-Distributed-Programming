# 8. Übungszettel - NPVP
# Hristo Popovski & Tomy Duong

import socket

# 2.1. TCP-Server:

HOST = "127.0.0.1"
PORT = 65432

def run_server():
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
        s.bind((HOST, PORT))
        s.listen()
        print(f"Server listening on {HOST}:{PORT}")
        conn, addr = s.accept()
        with conn:
                print(f"Connected by {addr}")
                # 2.1. Hello World
                conn.sendall("Hello World.".encode())
                print(f"Sent \"Hello World.\" to {addr}")

run_server()
