#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <omp.h>

// 7. Übungszettel - NPVP
// Hristo Popovski & Tomy Duong

// 1.1.:
typedef struct {
        int jahr;
        int monat;
        int tag;
        int stunde;
        int minute;
        float temperatur;
        float tempDiff;
} WetterStation;

float tempDiff(WetterStation *current, WetterStation *pred);

void tempDiffStation(const char *fileName, const char *station)
{
    char csvFormat[200];

    // CSV Format auswählen abhängig vom Dateinamen
    if (strstr(fileName, "Gatow-1") != NULL) {
        strcpy(csvFormat, "%d %d %d %d %d %*d %*d %*d %f");
    }
    else if (strstr(fileName, "Botanischer-Garten-1") != NULL) {
        strcpy(csvFormat, "%d %d %d %d %d %*d %*d %*d %f");
    }
    else if (strstr(fileName, "Marzahn-1") != NULL) {
        strcpy(csvFormat, "%d %d %d %d %d %*d %*d %*d %*f %*s %f");
    }
    else if (strstr(fileName, "Tegel-Forstamt-1") != NULL) {
        strcpy(csvFormat, "%d %d %d %d %d %*d %*d %*d %f");
    }
    else if (strstr(fileName, "Wannsee-1") != NULL) {
        strcpy(csvFormat, "%d %d %d %d %d %*d %*d %*d %f");
    }
    else if (strstr(fileName, "Planetarium-1") != NULL) {
        strcpy(csvFormat, "%d %d %d %d %d %*d %*d %*d %*f %*s %*f %*s %f");
    }
    else if (strstr(fileName, "Pichelsdorf-1") != NULL) {
        strcpy(csvFormat, "%d %d %d %d %d %*d %*d %*d %*f %*s %f");
    }
    else if (strstr(fileName, "Botanischer-Garten-2") != NULL) {
        strcpy(csvFormat, "%d %d %d %d %d %*d %*d %*d %f");
    }
    else if (strstr(fileName, "Funkturm-1") != NULL) {
        strcpy(csvFormat, "%d %d %d %d %d %*d %*d %*d %*f %*s %f");
    }
    else if (strstr(fileName, "Tegeler-See-1") != NULL) {
        strcpy(csvFormat, "%d %d %d %d %d %*d %*d %*d %*f %*s %*f %*s %*f %*s %*f %*s %*f %*s %f");
    }
    else if (strstr(fileName, "Messwiese-2") != NULL) {
        strcpy(csvFormat, "%d %d %d %d %d %*d %*d %*d %f");
    }
    else if (strstr(fileName, "Mueggelsee-1") != NULL) {
        strcpy(csvFormat, "%d %d %d %d %d %*d %*d %*d %*f %*s %*f %*s %*f %*s %f");
    }
    else if (strstr(fileName, "Messturm-2") != NULL) {
        // NO TEMP AVAILABLE
        printf("Keine Temperatur verfuegbar fuer Station %s\n", fileName);
        return;
    }
    else if (strstr(fileName, "Moabit-1") != NULL) {
        strcpy(csvFormat, "%d %d %d %d %d %*d %*d %*d %f");
    }
    else {
        printf("Keine Formatregel, Standardformat wird verwendet.\n");
        strcpy(csvFormat, "%d %d %d %d %d %*d %*d %*d %f");
    }

    // Ausgabedatei-Namen erzeugen (mit cleaned station name)
    char tempDiffFileName[300];
    snprintf(tempDiffFileName, sizeof(tempDiffFileName), "%s_TEMP_DIFF.csv", station);

    // Eingabedatei öffnen
    FILE *inputFile = fopen(fileName, "r");
    if (!inputFile) {
        printf("Error: Datei kann nicht geoeffnet werden: %s\n", fileName);
        return;
    }

    // Ausgabedatei erzeugen (critical nur für fopen)
    FILE *tempDiffFile = NULL;
#pragma omp critical
    {
        tempDiffFile = fopen(tempDiffFileName, "w");
    }

    if (!tempDiffFile) {
        printf("Error: Datei kann nicht erstellt werden: %s\n", tempDiffFileName);
        fclose(inputFile);
        return;
    }

    fprintf(tempDiffFile, "Date        Time     Temperature (°C)      Temp Difference\n");

    char line[500];

    // Header überspringen
    while (fgets(line, sizeof(line), inputFile)) {
        if (line[0] != '#') break;
    }

    WetterStation rec;
    WetterStation pred;

    int count = 0;
    int Print = 9000;       // Begrenzung der Ausgabe

    // Initialisiere Predecessor 
    pred.jahr = 2022;
    pred.monat = 1;
    pred.tag = 1;
    pred.stunde = 0;
    pred.minute = 0;
    pred.temperatur = 0.0;
    pred.tempDiff = 0.0;

    while (fgets(line, sizeof(line), inputFile)) {

        if (strstr(line, "end of data") != NULL) break;

        int n = sscanf(line,
                       csvFormat,
                       &rec.jahr, &rec.monat, &rec.tag,
                       &rec.stunde, &rec.minute,
                       &rec.temperatur);

        // Nur vollständige stündliche Messungen berücksichtigen
        if (n == 6 && rec.minute == 0)
        {
            tempDiff(&rec, &pred);

            fprintf(tempDiffFile, "%04d-%02d-%02d  %02d:%02d       %6.1f                        %6.1f\n",
                    rec.jahr, rec.monat, rec.tag,
                    rec.stunde, rec.minute,
                    rec.temperatur, rec.tempDiff);

            count++;

            if (count >= Print)
            {
                break;
            }
        }
    }

    fclose(inputFile);
    fclose(tempDiffFile);

    printf("Temp-Diff gespeichert in %s\n", tempDiffFileName);
}

float tempDiff(WetterStation *current, WetterStation *pred)
{
    current->tempDiff = current->temperatur - pred->temperatur;
    *pred = *current;
    return current->tempDiff;
}

// 1.2. Nachbarstruktur
typedef struct {
    char nord[50];
    char sued[50];
    char ost[50];
    char westen[50];
} Nachbar;

Nachbar getNachbar(const char *station)
{
    Nachbar nachbar = {"", "", "", ""};

    // Nachbarn anhand Karte ermitteln
    if(strstr(station, "Gatow-1"))
    {
        strcpy(nachbar.nord, "Pichelsdorf-1");
        strcpy(nachbar.sued, "Wannsee-1" );
        strcpy(nachbar.ost, "Messwiese-2");
    }
    else if(strstr(station, "Botanischer-Garten-1"))
    {
        strcpy(nachbar.nord, "Messwiese-2");
        strcpy(nachbar.westen, "Gatow-1");
    }
    else if(strstr(station, "Marzahn-1"))
    {
        strcpy(nachbar.westen, "Planetarium-1");
    }
    else if(strstr(station, "Tegel-Forstamt-1"))
    {
        strcpy(nachbar.sued, "Tegeler-See-1");
    }
    else if(strstr(station, "Wannsee-1"))
    {
        strcpy(nachbar.nord, "Gatow-1");
        strcpy(nachbar.ost, "Botanischer-Garten-1");
    }
    else if(strstr(station, "Planetarium-1"))
    {
        strcpy(nachbar.westen, "Moabit-1");
        strcpy(nachbar.ost, "Marzahn-1");
    }
    else if(strstr(station, "Pichelsdorf-1"))
    {
        strcpy(nachbar.sued, "Gatow-1" );
        strcpy(nachbar.ost, "Funkturm-1");
    }
    else if(strstr(station, "Botanischer-Garten-2"))
    {
        strcpy(nachbar.nord, "Messwiese-2");
        strcpy(nachbar.westen, "Gatow-1");
    }
    else if(strstr(station, "Funkturm-1"))
    {
        strcpy(nachbar.westen, "Pichelsdorf-1");
        strcpy(nachbar.ost, "Moabit-1");
    }
    else if(strstr(station, "Tegeler-See-1"))
    {
        strcpy(nachbar.nord, "Tegel-Forstamt-1");
        strcpy(nachbar.sued, "Moabit-1" );
    }
    else if(strstr(station, "Messwiese-2"))
    {
        strcpy(nachbar.sued, "Botanischer-Garten-2");
    }
    else if(strstr(station, "Mueggelsee-1"))
    {
        // keine Nachbarn definiert
    }
    else if(strstr(station, "Messturm-2")) //no temp
    {
        // keine Nachbarn
    }
    else if(strstr(station, "Moabit-1"))
    {
        strcpy(nachbar.nord, "Tegeler-See-1");
        strcpy(nachbar.westen, "Funkturm-1");
        strcpy(nachbar.ost, "Planetarium-1");
    }

    return nachbar;
}

void rechneMittelwert(const char *station)
{
    char baseFile[200];
    snprintf(baseFile, sizeof(baseFile), "%s_TEMP_DIFF.csv", station);

    FILE *base = fopen(baseFile, "r");
    if (!base) {
        printf("Kann %s nicht öffnen, wird übersprungen\n", baseFile);
        return;
    }

    Nachbar n = getNachbar(station);

    const char *neighbors[4] = { n.nord, n.sued, n.ost, n.westen };
    FILE *files[4] = { NULL, NULL, NULL, NULL };

    // Versuchen jede Nachbar-Datei zu öffnen.
    for (int i = 0; i < 4; i++)
    {
        if (strlen(neighbors[i]) > 0)
        {
            char fname[200];
            snprintf(fname, sizeof(fname), "%s_TEMP_DIFF.csv", neighbors[i]);
            files[i] = fopen(fname, "r");
            if (files[i]) fgets(fname, sizeof(fname), files[i]); // Header überspringen
        }
    }

    char outFile[200];
    snprintf(outFile, sizeof(outFile), "%s_NEIGHBOUR_MEAN.csv", station);

    FILE *out = NULL;
#pragma omp critical
    {
        out = fopen(outFile, "w");
    }
    if(!out) {
        for (int i = 0; i < 4; i++) if (files[i]) fclose(files[i]);
        fclose(base);
        printf("Kann %s nicht erstellt werden, überspringe Mittelwert-Berechnung\n", outFile);
        return;
    }

    fprintf(out, "Date	      Time       MeanTemperature\n");

    char line[300];
    fgets(line, sizeof(line), base); // Header überspringen

    while (fgets(line, sizeof(line), base)) {
        char date[20], time[20];
        float tempBase, diff;
        if (sscanf(line, "%s %s %f %f", date, time, &tempBase, &diff) != 4)
            continue;

        // Differenz der eigenen Station mit einbeziehen
        float sum = diff;
        int count = 1;

        // Lese eine Zeile aus jeder Nachbardatei.
        for (int i = 0; i < 4; i++) {
            if (files[i]) {
                char nline[300], d[20], ti[20];
                float t, diff2;
                if (fgets(nline, sizeof(nline), files[i]) &&
                    sscanf(nline, "%s %s %f %f", d, ti, &t, &diff2) == 4) {
                    sum += diff2;
                    count++;
                }
            }
        }

        fprintf(out, "%s    %s      %.3f\n", date, time, sum / count);
    }

    fclose(base);
    for (int i = 0; i < 4; i++) if (files[i]) fclose(files[i]);
    fclose(out);

    printf("Nachbar-Mittelwert gespeichert in %s\n", outFile);
}

void mergeCSV (const char *station)
{
    char tempDiffFile[200];
    char meanFile[200];
    char finalFile[200];
    snprintf(tempDiffFile, sizeof(tempDiffFile), "%s_TEMP_DIFF.csv", station);
    snprintf(meanFile, sizeof(meanFile), "%s_NEIGHBOUR_MEAN.csv", station);
    snprintf(finalFile, sizeof(finalFile), "%s_FINAL.csv", station);

    FILE *fTDiff = fopen(tempDiffFile, "r");
    FILE *fMean = fopen(meanFile, "r");

    if(!fTDiff || !fMean)
    {
        // genauer ausgeben, was fehlt
        if (!fTDiff && !fMean) {
            printf("%s fehlt: sowohl %s_TEMP_DIFF.csv als auch %s_NEIGHBOUR_MEAN.csv fehlen, merge unmoeglich\n", station, station, station);
        } else if (!fTDiff) {
            printf("%s fehlt: %s_TEMP_DIFF.csv fehlt, merge unmoeglich\n", station, station);
            if(fMean) fclose(fMean);
        } else { // !fMean
            printf("%s fehlt: %s_NEIGHBOUR_MEAN.csv fehlt, merge unmoeglich\n", station, station);
            if(fTDiff) fclose(fTDiff);
        }
        return;
    }

    FILE *fFinal = NULL;
#pragma omp critical
    {
        fFinal = fopen(finalFile, "w");
    }
    if (!fFinal)
    {
        printf("%s konnte nicht erstellt werden\n", finalFile);
        fclose(fTDiff);
        fclose(fMean);
        return;
    }

    fprintf(fFinal, "Date	    Time     Temp (°C) 	   Temp Difference (°C)     Neighbour Temp Difference Mean\n");

    char line1[300];
    char line2[300];

    fgets(line1, sizeof(line1), fTDiff);
    fgets(line2, sizeof(line2), fMean);

    // Zeile für Zeile zusammenführen
    while (fgets(line1, sizeof(line1), fTDiff) && fgets(line2, sizeof(line2), fMean))
    {
        char date[20], time[20];
        float tempBase, diff, mean;

        if (sscanf(line1, "%s %s %f %f", date, time, &tempBase, &diff) != 4)
        {
            continue;
        }

        if (sscanf(line2, "%s %s %f", date, time, &mean) != 3)
        {
            continue;
        }

        fprintf(fFinal, "%s 	%s    %6.1f     		%6.1f      		    %6.1f\n", date, time, tempBase, diff, mean);
    }

    fclose(fTDiff);
    fclose(fMean);
    fclose(fFinal);

    // Temporäre Dateien löschen
    remove(tempDiffFile);
    remove(meanFile);

    printf("Merged gespeichert in %s\n", finalFile);
}

// Main
int main(void)
{
#if Aufgabe ==  1

    // tracke benötigte Zeit für Aufgabe 3
    double start = omp_get_wtime();

    DIR *d;
    struct dirent *dir;

    d = opendir(".");
    if (!d) {
        printf("Verzeichnis kann nicht geoeffnet werden!\n");
        return 1;
    }

    const int MAXFILES = 64;
    char *files[MAXFILES];
    char *stations[MAXFILES];
    int fileCount = 0;

    // Alle CSV-Dateien sammeln
    while ((dir = readdir(d)) != NULL) {
        if (strstr(dir->d_name, ".csv") != NULL &&
            strstr(dir->d_name, "_TEMP_DIFF.csv") == NULL &&
            strstr(dir->d_name, "_NEIGHBOUR_MEAN.csv") == NULL &&
            strstr(dir->d_name, "_FINAL.csv") == NULL)
        {
            files[fileCount] = strdup(dir->d_name);

            // Stationsnamen bereinigen
            char station[200];
            strncpy(station, dir->d_name, sizeof(station));
            station[sizeof(station)-1] = '\0';
            char *firstUnderscore = strchr(station, '_');
            if (firstUnderscore) memmove(station, firstUnderscore + 1, strlen(firstUnderscore));
            char *suffix = strstr(station, "_2022");
            if (suffix) *suffix = '\0';

            stations[fileCount] = strdup(station);

            fileCount++;
            if (fileCount >= MAXFILES) break;
        }
    }
    closedir(d);

    if (fileCount == 0) {
        printf(">>> Keine Eingabe-CSV-Dateien gefunden.\n");
        return 0;
    }

    // Erzeuge alle _TEMP_DIFF.csv Dateien
    for (int i = 0; i < fileCount; i++) {
        printf(">>> Verarbeite TEMP_DIFF fuer Datei: %s\n", files[i]);
        tempDiffStation(files[i], stations[i]);
    }

    // Berechne alle _NEIGHBOUR_MEAN.csv Dateien
    for (int i = 0; i < fileCount; i++) {
        printf(">>> Berechne Mittelwert fuer Station: %s\n", stations[i]);
        rechneMittelwert(stations[i]);
    }

    // Merge alle _TEMP_DIFF.csv & _NEIGHBOUR_MEAN.csv zu _FINAL.csv
    for (int i = 0; i < fileCount; i++) {
        printf(">>> Mergen fuer Station: %s\n", stations[i]);
        mergeCSV(stations[i]);
    }

    double end = omp_get_wtime();
    printf("Zeit: %f\n", end - start);

    printf("Final CSV's fuer alle Stationen erstellt, ausser fuer Messturm-2, denn dafuer gibt keine Temperaturwerte.\n");

    // Aufräumen
    for (int i = 0; i < fileCount; i++) {
        free(files[i]);
        free(stations[i]);
    }

    return 0;

#elif Aufgabe ==  2

    // tracke benötigte Zeit für Aufgabe 3
    double start = omp_get_wtime();

    DIR *d;
    struct dirent *dir;

    d = opendir(".");
    if (!d) {
        printf("Verzeichnis kann nicht geoeffnet werden!\n");
        return 1;
    }

    const int MAXFILES = 64;
    char *files[MAXFILES];
    char *stations[MAXFILES];
    int fileCount = 0;

    // Durchlaufe alle Dateien im aktuellen Ordner
    while ((dir = readdir(d)) != NULL) {
        if (strstr(dir->d_name, ".csv") != NULL &&
            strstr(dir->d_name, "_TEMP_DIFF.csv") == NULL &&
            strstr(dir->d_name, "_NEIGHBOUR_MEAN.csv") == NULL &&
            strstr(dir->d_name, "_FINAL.csv") == NULL)
        {
            files[fileCount] = strdup(dir->d_name);

            // Stationsnamen bereinigen
            char station[200];
            strncpy(station, dir->d_name, sizeof(station));
            station[sizeof(station)-1] = '\0';
            char *firstUnderscore = strchr(station, '_');
            if (firstUnderscore) memmove(station, firstUnderscore + 1, strlen(firstUnderscore));
            char *suffix = strstr(station, "_2022");
            if (suffix) *suffix = '\0';

            stations[fileCount] = strdup(station);

            fileCount++;
            if (fileCount >= MAXFILES) break;
        }
    }
    closedir(d);

    if (fileCount == 0) {
        printf(">>> Keine Eingabe-CSV-Dateien gefunden.\n");
        return 0;
    }

    // 1. Erzeuge alle _TEMP_DIFF.csv für jede Station gleichzeitig Parallel
    #pragma omp parallel for 
    for (int i = 0; i < fileCount; i++)
    {
        printf(">>> Thread %d Verarbeitet die Datei: %s \n", omp_get_thread_num(), files[i]);
        tempDiffStation(files[i], stations[i]);
    }

    // 2. Erzeuge nun alle _NEIGHBOUR_MEAN.csv für jede Station gleichzeitig Parallel
    #pragma omp parallel for 
    for (int i = 0; i < fileCount; i++)
    {
        printf(">>> Thread %d berechnet Mittelwert: %s \n", omp_get_thread_num(), stations[i]);
        rechneMittelwert(stations[i]);
    }

    // 3. Merge nun die _TEMP_DIFF.csv & _NEIGHBOUR_MEAN.csv jeder Stationen zu _FINAL.csv für jede Station gleichzeitig Parallel
    #pragma omp parallel for 
    for (int i = 0; i < fileCount; i++)
    {
        printf(">>> Thread %d Merges: %s \n", omp_get_thread_num(), stations[i]);
        mergeCSV(stations[i]);
    }

    double end = omp_get_wtime();
    printf("Zeit: %f\n", end - start);

    printf("Final CSV's fuer alle Stationen erstellt, ausser fuer Messturm-2, denn dafuer gibt keine Temperaturwerte.\n");

    // aufräumen
    for (int i = 0; i < fileCount; i++) {
        free(files[i]);
        free(stations[i]);
    }

    return 0;

#else

    printf("Diese Aufgabe existiert nicht. Schreibe z.B. make Aufgabe1\n");
    return 1;

#endif

    return 0;
}
