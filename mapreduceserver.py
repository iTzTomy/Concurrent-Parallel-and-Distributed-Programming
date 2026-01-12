# 9. Übungszettel - NPVP
# Hristo Popovski & Tomy Duong 

# 1.3. RPyC Server: Die Verarbeitungsknoten

import rpyc
from rpyc.utils.server import ThreadedServer
import argparse 

@rpyc.service
class WordCountService(rpyc.Service):

    def on_connect(self, conn):
        pass

    def on_disconnect(self, conn):
        pass
    
    @rpyc.exposed
    def count_words(self , lines):
        wordcount = {}
        
        for line in lines:
            # Alle nicht-alphanumerischen Zeichen werden entfernt
            # Sämtliche Wörter werden in Kleinbuchstaben gewandelt
            chars = [c if c.isalnum() else ' ' for c in line.lower()]
            # Leerzeichen und Tabs am Anfang oder Ende der Zeile werden entfernt
            normalizedLine = ''.join(chars).strip().split()
   
            # Aktualisiere dict
            for word in normalizedLine:
                if word in wordcount:
                    wordcount[word] += 1
                else:
                    wordcount[word] = 1

        return wordcount

if __name__ == '__main__':

    parser = argparse.ArgumentParser(prog='mapreduceserver', description='Zaehlt Anzahl von Woerter.')
    
    parser.add_argument('--port', type=int, required=True, help='Port-Nummer')
    
    args = parser.parse_args()

    protocol_config = {
        # aktiviere pickle damit wir obtain benutzen können
        "allow_pickle": True,
        "allow_public_attrs": True
    }

    # Starte Server mit gegebenen Port
    server = ThreadedServer(
        WordCountService, 
        port=args.port,
        protocol_config=protocol_config)
    print(f"Service started on port: {args.port}")

    server.start()
