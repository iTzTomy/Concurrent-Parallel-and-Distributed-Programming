# 9. Übungszettel - NPVP
# Hristo Popovski & Tomy Duong 

# 1.4 RPyC Client: Map und Reduce

import rpyc
from rpyc.utils.helpers import async_
from rpyc.utils.classic import obtain
import argparse
import math

# lese den ganzen Text als Liste ein 

with open('100.txt', encoding="utf-8") as file:
    lines = file.readlines()

# Finde Start und Ende des eigentlichen Inhalts (Project Gutenberg Header/Footer entfernen)
start_idx = 0
end_idx = len(lines)

for i, line in enumerate(lines):
    if "*** START OF" in line or "***START OF" in line:
        start_idx = i + 1
        break

for i in range(len(lines) - 1, -1, -1):
    if "*** END OF" in lines[i] or "***END OF" in lines[i]:
        end_idx = i
        break

# Nimm nur den eigentlichen Inhalt
lines = lines[start_idx:end_idx]

# ignoriere Leerzeichen
lines = [line.strip() for line in lines]

parser = argparse.ArgumentParser(prog='mapreduceclient', description='Liest Textdatei zeilenweise ein und teilt anhand der Ports auf.')

parser = argparse.ArgumentParser()
parser.add_argument("--file", required=True)
parser.add_argument("--ports", required=True)

args = parser.parse_args()

# Anzahl an Ports erhalten

ports = [int(p.strip()) for p in args.ports.split(",")]
num_ports = len(ports)

# Splitte lines in subListen für jeden Port

num_lines = len(lines)
lines_per_port = math.ceil(num_lines / num_ports)

subLists = [lines[i : i + lines_per_port] for i in range(0, num_lines, lines_per_port)]

final_dict = {}

for port, subListe in zip(ports, subLists):

    # Map Step
    conn = rpyc.connect("localhost", port, config={
        # aktiviere pickle damit wir obtain benutzen können
        "allow_pickle": True,
        "allow_public_attrs": True
    })
    async_count = async_(conn.root.count_words)
    # Nutze obtain, damit result von NetRef zu einem Dictionary umgewandelt wird
    result = obtain(async_count(subListe).value)

    # Reduce Step
    for word, count in result.items():
        if word in final_dict:
            final_dict[word] += count
        else:
            final_dict[word] = count

    conn.close()

with open("reduced.txt", "w", encoding="utf-8") as f:
    # Sortiere Wörter nach Count absteigend und schreibe als String in reduced.txt
    f.write(str(dict(sorted(final_dict.items(), key=lambda x: x[1], reverse=True))))

print("MapReduce abgeschlossen. Ergebnisse stehen sortiert in 'reduced.txt'.")


'''

1.6 Ausführung

'the': 30396
Etwas weniger, wahrscheinlich durch ungenaue Normalisierung

1.7 Fragestellungen

Wie oft kommen die als typisches Shakespeare-Englisch empfundenen Worte „thou“ und „thee“ in Shakespeares Werken vor?

'thou': 5937

'thee': 3423 

Kommt das Wort „queen“ oder „king“ häufiger vor, und in welchem Verhältnis zueinander?

'king': 3210

'queen': 1092

Das Wort "king" kommt häufiger vor. 
Verhältnis king:queen = 3210:1092 ≈ 2,94:1 (etwa dreimal so oft)

'''