# 8. Übungszettel - NPVP
# Hristo Popovski & Tomy Duong

# 4.1. TCP-Server:

import socket
from threading import Thread, Lock
import json

HOST = "127.0.0.1"
PORT3 = 65434

clients = []
messages = []
lock = Lock()

def broadcast():
    if not messages:
        return  # keine Nachrichten zum Senden

    # gesamte Nachrichtenliste in JSON umwandeln
    data = json.dumps(messages).encode()

    # an alle verbundenen Clients senden
    for c in clients[:]:  # über Kopie iterieren
        try:
            c.sendall(data)  # TCP: komplette Daten senden
        except:
            clients.remove(c)  


def handle_client(conn, addr):
        
    print(f"Connected by {addr}")
    clients.append(conn)

    while True:
                text = conn.recv(4096)
                if not text:
                    break 

                msg = json.loads(text.decode()) # Vom String wieder zum Dictionary konvertieren 


                if msg["message"].lower() == "exit":
                    print(f"Disconncted from {addr}")
                    break

                with lock:  # kritischer Abschnitt
                    messages.append(msg)
                    messages.sort(key=lambda m: (m["lamport"], m["client_id"])) # Sortierung nach lamport & client_id
                    broadcast()


def run_server3():
    
    global lamport

    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
        s.bind((HOST, PORT3))
        s.listen()
        
        print(f"Server listening on {HOST}:{PORT3}")
        

        while True:
            conn, addr = s.accept()
            # erstelle Thread für jeden Client
            Thread(
                target=handle_client,
                args=(conn, addr),
                daemon=True
            ).start()





run_server3()


    