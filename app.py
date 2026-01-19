# 9. Übungszettel - NPVP
# Hristo Popovski & Tomy Duong 

import streamlit as st
import requests
import pandas as pd

# -----------------------------------------------------
# Konfiguration
# -----------------------------------------------------
API_BASE = "http://127.0.0.1:5000"  # Muss mit Flask-Port übereinstimmen

st.set_page_config(page_title="Weather API Dashboard", layout="wide")
st.title("Weather Data Dashboard")

# -----------------------------------------------------
# Hilfsfunktion für API-Abfragen
# -----------------------------------------------------
def request_data(endpoint):
    """GET-Request an die Flask-API senden"""
    try:
        r = requests.get(f"{API_BASE}{endpoint}")
        if r.status_code == 200:
            return r.json()
        else:
            st.error(f"Fehler {r.status_code}: {r.json().get('error', 'Unbekannter Fehler')}")
            return []
    except requests.exceptions.ConnectionError:
        st.error("Verbindung zur Flask-API nicht möglich. Läuft der API-Server?")
        return []

# -----------------------------------------------------
# Seitenmenü / Navigation
# -----------------------------------------------------
option = st.sidebar.selectbox(
    "Aktion auswählen:",
    (
        "Alle Wetterdaten anzeigen",
        "Nach Station filtern",
        "Nach Channel filtern",
        "Station löschen"
    )
)

# -----------------------------------------------------
# 1. Alle Wetterdaten anzeigen
# -----------------------------------------------------
if option == "Alle Wetterdaten anzeigen":
    st.subheader("Gesamtübersicht")
    data = request_data("/weather")
    if data:
        st.dataframe(pd.DataFrame(data), use_container_width=True)

# -----------------------------------------------------
# 2. Station filtern
# -----------------------------------------------------
elif option == "Nach Station filtern":
    station = st.text_input("Station-ID eingeben (z. B. Botanischer-Garten-1)")
    if st.button("Abrufen"):
        if station:
            data = request_data(f"/weather/station/{station}")
            if data:
                st.dataframe(pd.DataFrame(data))
            else:
                st.warning(f"Keine Daten für Station '{station}' gefunden.")
        else:
            st.warning("Bitte geben Sie eine Station-ID ein.")

# -----------------------------------------------------
# 3. Channel filtern
# -----------------------------------------------------
elif option == "Nach Channel filtern":
    channel = st.text_input("Channel-Nummer eingeben (z. B. 042)")
    if st.button("Abrufen"):
        if channel:
            data = request_data(f"/weather/channel/{channel}")
            if data:
                st.dataframe(pd.DataFrame(data))
            else:
                st.warning(f"Keine Daten für Channel '{channel}' gefunden.")
        else:
            st.warning("Bitte Channel angeben.")

# -----------------------------------------------------
# 4. Station löschen
# -----------------------------------------------------
elif option == "Station löschen":
    station_del = st.text_input("Station-ID zum Löschen eingeben")
    if st.button("Löschen"):
        if station_del:
            try:
                r = requests.delete(f"{API_BASE}/weather/{station_del}")
                if r.status_code == 200:
                    st.success(r.json()["message"])
                else:
                    st.error(r.json().get("error", "Fehler beim Löschen"))
            except requests.exceptions.ConnectionError:
                st.error("Verbindung zur Flask-API nicht möglich.")
        else:
            st.warning("Bitte eine Station-ID eingeben.")

# -----------------------------------------------------
# Footer
# -----------------------------------------------------
st.markdown("---")
st.caption("Weather Dashboard - Streamlit frontend for Flask REST-API")


''' 
5 Reflexion und Erweiterung 


1. Architekturprinzip: Wodurch unterscheidet sich eine REST-API grundsätzlich von einem RPC-basierten Ansatz?:

REST: Ressourcenorientiert, jede URI repräsentiert eine Ressource. Operationen erfolgen über 
standardisierte HTTP-Methoden (GET, POST, PUT, DELETE). Client und Server sind stateless, 
Kommunikation über einfache HTTP-Nachrichten.

RPC: Funktions- bzw. prozedurorientiert, der Client ruft explizit Server-Funktionen auf. 
Tight Coupling, oft abhängig von Plattform und Datenformat. 
REST ist leichter skalierbar und plattformunabhängig.

2. Vorteile von REST: Nennen Sie zwei Vorteile einer REST-API für Web-Anwendungen gegenüber RPC.:

    1. Lose Kopplung / Interoperabilität: Clients müssen Server-Implementierung nicht kennen, einfache Integration über HTTP.

    2. Skalierbarkeit und Performance: Stateless-Kommunikation, Caching von GET-Requests und standardisierte HTTP-Methoden vereinfachen Lastverteilung.

3. Persistenz: Warum ist eine Datenbank (z.B. SQLite) langfristig besser geeignet als eine JSON-Datei zur Speicherung der Wetterdaten?

Datenbanken bieten Transaktionen, Indexierung und parallelen Zugriff, wodurch Daten konsistent bleiben.
JSON-Dateien eignen sich nur für kleine und temporäre Datenmengen. Sie sind fehleranfällig bei parallelem Zugriff und großen Datenbeständen.

4. Sicherheit: Wie könnte man den Zugriff auf die REST-API absichern? Diskutieren Sie mögliche Verfahren (z.B. HTTPS).

HTTPS: Verschlüsselt die Datenübertragung und schützt vor Man-in-the-Middle-Angriffen.
Authentifizierung / Autorisierung: z.B. API-Keys, OAuth2 oder JWT.
Zusätzliche Maßnahmen: Rate-Limiting, Firewalls, Token-Validierung, um Missbrauch und DoS-Angriffe zu verhindern.

'''


