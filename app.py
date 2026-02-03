import streamlit as st
import requests
import pandas as pd
from datetime import datetime

API_BASE = "http://127.0.0.1:5000"

st.set_page_config(page_title="Weather API Dashboard", layout="wide")
st.title("Weather Data Dashboard")

def request_data(endpoint, method="GET", json_data=None):
    url = f"{API_BASE}{endpoint}"
    try:
        if method == "GET":
            r = requests.get(url)
        elif method == "POST":
            r = requests.post(url, json=json_data)
        elif method == "PUT":
            r = requests.put(url, json=json_data)
        elif method == "DELETE":
            r = requests.delete(url)
        else:
            st.error("Ungültige Methode")
            return None

        if r.status_code in (200, 201):
            return r.json()
        else:
            error_msg = r.json().get('error', 'Unbekannter Fehler')
            st.error(f"Fehler {r.status_code}: {error_msg}")
            return None
    except requests.exceptions.ConnectionError:
        st.error("Verbindung zur Flask-API nicht möglich. Läuft der API-Server?")
        return None

tab_paul, tab_clara = st.tabs([
    "Paul - Expertenmodus (Daten anzeigen, filtern, löschen & bereinigen)",
    "Clara - Kindermodus (aktuelles Wetter anschauen)"
])

with tab_paul:
    st.subheader("Wetterdaten bereinigen & korrigieren")
    st.markdown("Stationen löschen, einzelne Werte korrigieren, fehlerhafte Einträge entfernen")

    option = st.sidebar.selectbox(
        "Aktion auswählen:",
        (
            "Alle Wetterdaten anzeigen",
            "Nach Station filtern",
            "Nach Channel filtern",
            "Station löschen",
            "Daten bereinigen & korrigieren"
        )
    )

    if option == "Alle Wetterdaten anzeigen":
        st.subheader("Gesamtübersicht")
        data = request_data("/weather")
        if data:
            df = pd.DataFrame(data)
            st.dataframe(df, use_container_width=True)

    elif option == "Nach Station filtern":
        station = st.text_input("Station-ID eingeben (z. B. Botanischer-Garten-1)")
        if st.button("Abrufen"):
            if station:
                data = request_data(f"/weather/station/{station}")
                if data:
                    st.dataframe(pd.DataFrame(data), use_container_width=True)
                else:
                    st.warning(f"Keine Daten für Station '{station}' gefunden.")
            else:
                st.warning("Bitte geben Sie eine Station-ID ein.")

    elif option == "Nach Channel filtern":
        channel = st.text_input("Channel-Nummer eingeben (z. B. 042)")
        if st.button("Abrufen"):
            if channel:
                data = request_data(f"/weather/channel/{channel}")
                if data:
                    st.dataframe(pd.DataFrame(data), use_container_width=True)
                else:
                    st.warning(f"Keine Daten für Channel '{channel}' gefunden.")
            else:
                st.warning("Bitte Channel angeben.")

    elif option == "Station löschen":
        station_del = st.text_input("Station-ID zum Löschen eingeben")
        if st.button("Löschen"):
            if station_del:
                result = request_data(f"/weather/{station_del}", method="DELETE")
                if result:
                    st.success(result.get("message", "Station erfolgreich gelöscht!"))
                else:
                    st.error("Fehler beim Löschen")
            else:
                st.warning("Bitte eine Station-ID eingeben.")

    elif option == "Daten bereinigen & korrigieren":
        st.subheader("Fehlerhafte Messwerte korrigieren")
        st.markdown("Bearbeiten Sie Werte direkt. Änderungen werden direkt auf bestehenden Datensätzen aktualisiert (keine Kopien).")

        if st.button("Alle Daten zum Bearbeiten laden"):
            data = request_data("/weather")
            if data:
                df = pd.DataFrame(data)
                st.session_state['edit_df'] = df
                st.success(f"{len(df)} Datensätze geladen")

        if 'edit_df' in st.session_state:
            edited_df = st.data_editor(
                st.session_state['edit_df'],
                column_config={
                    "value": st.column_config.NumberColumn("Wert", required=True),
                    "unit": st.column_config.TextColumn("Einheit", required=True),
                    "station-id": st.column_config.TextColumn("Station-ID", disabled=True),
                    "channel": st.column_config.TextColumn("Channel", disabled=True),
                    "date-start": st.column_config.TextColumn("Zeitpunkt", disabled=True),
                    "aggregation": st.column_config.TextColumn("Aggregation", required=False),
                },
                num_rows="dynamic",
                use_container_width=True,
                hide_index=False,
            )

            if st.button("Änderungen speichern"):
                changes_made = False
                original_df = st.session_state['edit_df'].copy()

                for idx, row in edited_df.iterrows():
                    payload = row.to_dict()
                    data_id = payload.get("id")

                    if idx >= len(original_df):
                        payload.pop("id", None)
                        request_data("/weather", method="POST", json_data=payload)
                        changes_made = True
                    else:
                        original = original_df.iloc[idx]
                        if not original.equals(row):
                            if data_id:
                                request_data(f"/weather/id/{data_id}", method="PUT", json_data=payload)
                                changes_made = True
                            else:
                                payload.pop("id", None)
                                request_data("/weather", method="POST", json_data=payload)
                                changes_made = True

                if changes_made:
                    st.success("Änderungen gespeichert (Updates oder neue Einträge)")
                    del st.session_state['edit_df']
                    st.rerun()
                else:
                    st.info("Keine Änderungen erkannt")

# Clara-Tab – kindgerecht, groß, einfach, emotional
with tab_clara:
    st.subheader("Hallo Clara! 🌤️ Schau mal, wie das Wetter gerade ist!")
    st.markdown("Keine komplizierten Zahlen - nur ob es warm oder kalt ist!")

    col1, col2, col3 = st.columns([1, 4, 1])
    with col2:
        if st.button("Wetter anschauen ✨", type="primary", use_container_width=True):
            data = request_data("/weather")
            if data:
                # Neueste AirTemp suchen
                temps = []
                for entry in data:
                    if entry.get("varkey") == "AirTemp" and entry.get("value"):
                        try:
                            val = float(entry["value"])
                            dt_str = entry.get("date-start", "")
                            if dt_str:
                                dt = datetime.fromisoformat(dt_str.replace("Z", "+00:00"))
                                temps.append((dt, val))
                        except:
                            pass

                if temps:
                    temps.sort(reverse=True)  # neueste zuerst
                    current_temp = temps[0][1]

                    st.metric(
                        label="Aktuelle Temperatur in Berlin",
                        value=f"{current_temp:.1f} °C",
                        delta_color="normal"
                    )

                    if current_temp >= 22:
                        st.success("🌞 Sommer-Vibes! Perfekt für Eis, Sonne und gute Laune! 🍦😎")
                    elif current_temp >= 15:
                        st.info("🌼 Angenehm warm! Ideal für einen Spaziergang oder Zeit draußen. 🚶‍♂️🌤️")
                    elif current_temp >= 10:
                        st.info("🍂 Frisch, aber okay! Mit Pulli lässt sich's draußen aushalten. 🧥🙂")
                    elif current_temp >= 5:
                        st.warning("❄️ Ziemlich kühl! Jacke nicht vergessen. 🧣☕")
                    else:
                        st.error("🥶 Eiskalt! Heute ist definitiv ein Drinnen-Tag. 🏠🔥")

st.markdown("---")
st.caption("Weather Dashboard - Streamlit frontend for Flask REST-API")