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


def run_client3():
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
        s.connect((HOST, PORT3))

        print(f"Connected as Client {client_id}")

        Thread(target=receive_message, args=(s,), daemon=True).start()

        while True:
            text = input()
            global lamport
            lamport += 1

            msg = {"lamport" : lamport, "client_id" : client_id, "local_time" : time_calc(), "message" : text}
        
            s.sendall(json.dumps(msg).encode()) # von Dictionary zum String konvertieren 

            if text.lower() == "exit":
                break

run_server3()


    