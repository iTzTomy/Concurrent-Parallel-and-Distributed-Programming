# 9. Übungszettel - NPVP
# Hristo Popovski & Tomy Duong 

## Projektstart

1. Virtuelle Umgebung aktivieren:

**Windows PowerShell**
```powershell
venv\Scripts\Activate.ps1
```

2. Streamlit-Dashboard starten:
```powershell
streamlit run app.py
```

> Die Flask-API läuft standardmäßig auf `http://127.0.0.1:5000`  
> Streamlit läuft standardmäßig auf `http://localhost:8501`

---

## REST-Endpunkte

| Methode | Endpunkt | Beschreibung |
|---------|----------|--------------|
| GET     | /weather | Alle Wetterdaten abrufen |
| GET     | /weather/station/<station_id> | Daten einer bestimmten Station abrufen |
| GET     | /weather/channel/<channel_id> | Daten eines bestimmten Channels abrufen |
| GET     | /weather/id/<id> | Einzelnen Datensatz anhand ID abrufen |
| POST    | /weather | Neuen Wetterdatensatz hinzufügen |
| PUT     | /weather/<station_id> | Vorhandene Wetterdaten einer Station aktualisieren |
| DELETE  | /weather/<station_id> | Alle Wetterdaten einer Station löschen |

---

## Erweiterungen / Änderungen

- Implementierung der **CRUD-Funktionalität** (POST, PUT, DELETE) über Flask.
- Streamlit-Dashboard **DELETE-Funktion**, um Datensätze einer Station direkt zu löschen.


---

## 5 Reflexion und Erweiterung

**1. Architekturprinzip: Wodurch unterscheidet sich eine REST-API grundsätzlich von einem RPC-basierten Ansatz?:**  
REST: Ressourcenorientiert, jede URI repräsentiert eine Ressource. Operationen erfolgen über 
standardisierte HTTP-Methoden (GET, POST, PUT, DELETE). Client und Server sind stateless, 
Kommunikation über einfache HTTP-Nachrichten.

RPC: Funktions- bzw. prozedurorientiert, der Client ruft explizit Server-Funktionen auf. 
Tight Coupling, oft abhängig von Plattform und Datenformat. 
REST ist leichter skalierbar und plattformunabhängig.

**2. Vorteile von REST: Nennen Sie zwei Vorteile einer REST-API für Web-Anwendungen gegenüber RPC.:**  
1. Lose Kopplung / Interoperabilität: Clients müssen Server-Implementierung nicht kennen, einfache Integration über HTTP.  
2. Skalierbarkeit und Performance: Stateless-Kommunikation, Caching von GET-Requests und standardisierte HTTP-Methoden vereinfachen Lastverteilung.

**3. Persistenz: Warum ist eine Datenbank (z.B. SQLite) langfristig besser geeignet als eine JSON-Datei zur Speicherung der Wetterdaten?:**

Datenbanken bieten Transaktionen, Indexierung und parallelen Zugriff, wodurch Daten konsistent bleiben.
JSON-Dateien eignen sich nur für kleine und temporäre Datenmengen. Sie sind fehleranfällig bei parallelem Zugriff und großen Datenbeständen.

**4. Sicherheit: Wie könnte man den Zugriff auf die REST-API absichern? Diskutieren Sie mögliche Verfahren (z.B. HTTPS).:**

HTTPS: Verschlüsselt die Datenübertragung und schützt vor Man-in-the-Middle-Angriffen.
Authentifizierung / Autorisierung: z.B. API-Keys, OAuth2 oder JWT.
Zusätzliche Maßnahmen: Rate-Limiting, Firewalls, Token-Validierung, um Missbrauch und DoS-Angriffe zu verhindern.

---

