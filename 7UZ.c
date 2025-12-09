#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <omp.h>
#include <stdlib.h>
#include <stdbool.h>

// 7. Übungszettel - NPVP
// Hristo Popovski & Tomy Duong

// Hilfsfunktion: entferne \r und \n am Ende einer Zeile
void chomp(char *s) {
    if (!s) return;
    size_t l = strlen(s);
    while (l > 0 && (s[l-1] == '\n' || s[l-1] == '\r')) {
        s[--l] = '\0';
    }
}

// 1.1.:
typedef struct {
        int jahr;
        int monat; 
        int tag;
        int stunde;
        int minute;
        float temperatur;
        float tempDiff;
}WetterStation;

float tempDiff(WetterStation *current, WetterStation *pred);

void tempDiffStation(const char *fileName)
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
    else if (strstr(fileName, "Messturm-2")) //NO TEMP AVAILABLE
    {
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

    // Ausgabedatei-Namen erzeugen
    char tempDiffFileName[300];
    strcpy(tempDiffFileName, fileName);
    char *ext = strstr(tempDiffFileName, ".csv");
    if (ext) *ext = '\0';
    snprintf(tempDiffFileName, sizeof(tempDiffFileName), "%s_TEMP_DIFF.csv", tempDiffFileName);

    // Eingabedatei öffnen
    FILE *inputFile;
    inputFile = fopen(fileName, "r");

    if (!inputFile) {
        perror(fileName);
        printf("Error: Datei kann nicht geoeffnet werden!\n");
        return;
    }

    // Ausgabedatei erzeugen
    FILE *tempDiffFile = fopen(tempDiffFileName, "w");

    if (!tempDiffFile) {
        perror(tempDiffFileName);
        printf("Error: Datei kann nicht erstellt werden!\n");
        fclose(inputFile);
        return;
    }

    fprintf(tempDiffFile, "Date        Time     Temperature (°C)      Temp Difference\n");

    char line[500];

    // Header überspringen, aber falls die erste nicht-#-Zeile eine Datenzeile ist, wiederverwenden
    char firstDataLine[500];
    int hasFirstDataLine = 0;
    while (fgets(line, sizeof(line), inputFile)) {
        if (line[0] == '#') continue;
        // line enthält jetzt die erste Datenzeile oder es gibt keine Daten
        strncpy(firstDataLine, line, sizeof(firstDataLine));
        hasFirstDataLine = 1;
        break;
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
    pred.temperatur = 0.0f;
    pred.tempDiff = 0.0f;

    // Falls erste Datenzeile vorhanden, verarbeite sie zuerst
    if (hasFirstDataLine) {
        chomp(firstDataLine);
        int n = sscanf(firstDataLine,
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
                // printf("Printed %d",count); (für Debug)
                // schließe Dateien und beenden
                fclose(inputFile);
                fclose(tempDiffFile);
                // TEMP_DIFF Datei umbenennen (wie zuvor)
                char shortName[200];
                strcpy(shortName, fileName);
                char *firstUnderscore = strchr(shortName, '_');
                if (firstUnderscore) memmove(shortName, firstUnderscore + 1, strlen(firstUnderscore));
                char *suffix = strstr(shortName, "_2022");
                if (suffix) *suffix = '\0';

                char renamedFile[300];
                snprintf(renamedFile, sizeof(renamedFile), "%s_TEMP_DIFF.csv", shortName);
                rename(tempDiffFileName, renamedFile);

                printf("Temp-Diff gespeichert in %s\n", renamedFile);
                return;
            }
        }
    }

    // Jetzt die restlichen Zeilen normal lesen
    while (fgets(line, sizeof(line), inputFile)) {

        if (strstr(line, "end of data") != NULL) break;

        chomp(line);

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
                // printf("Printed %d",count); (für Debug)
                break;
            }
        }
    }


    fclose(inputFile);
    fclose(tempDiffFile);


    // TEMP_DIFF Datei unbennenen (wie zuvor)
    char shortName[200];
    strcpy(shortName, fileName);
    char *firstUnderscore = strchr(shortName, '_');
    if (firstUnderscore) memmove(shortName, firstUnderscore + 1, strlen(firstUnderscore));
    char *suffix = strstr(shortName, "_2022");
    if (suffix) *suffix = '\0';

    char renamedFile[300];
    snprintf(renamedFile, sizeof(renamedFile), "%s_TEMP_DIFF.csv", shortName);
    rename(tempDiffFileName, renamedFile);

    printf("Temp-Diff gespeichert in %s\n", renamedFile);
}

float tempDiff(WetterStation *current, WetterStation *pred)
{
    current->tempDiff = current->temperatur - pred->temperatur;
    *pred = *current;
    return current->tempDiff; 
}

// 1.2.:

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
        
    }
    else if(strstr(station, "Messturm-2")) //no temp
    {
        
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
        perror(baseFile);
        printf("Kann %s nicht öffnen\n", baseFile);
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
            if (files[i]) {
                char headerbuf[300];
                if (!fgets(headerbuf, sizeof(headerbuf), files[i])) {
                    // falls leer / fehlerhaft
                    fclose(files[i]);
                    files[i] = NULL;
                }
            }
        }
    }

    char outFile[200];
    snprintf(outFile, sizeof(outFile), "%s_NEIGHBOUR_MEAN.csv", station);
    FILE *out = fopen(outFile, "w");

    if (!out) {
        perror(outFile);
        // schließe bereits geöffnete Dateien
        fclose(base);
        for (int i = 0; i < 4; i++) if (files[i]) fclose(files[i]);
        return;
    }

    fprintf(out, "Date	      Time       MeanTemperature\n");

    char line[300];
    // Header der Basisdatei überspringen
    if (!fgets(line, sizeof(line), base)) {
        // leere Basisdatei
        fclose(base);
        for (int i = 0; i < 4; i++) if (files[i]) fclose(files[i]);
        fclose(out);
        return;
    }

    while (fgets(line, sizeof(line), base)) {
        chomp(line);
        char date[20], time[20];
        float tempBase, diff;
        if (sscanf(line, "%19s %19s %f %f", date, time, &tempBase, &diff) != 4)
            continue;

        // Differenz der eigenen Station mit einbeziehen
        float sum = diff;
        int count = 1;

        // Lese eine Zeile aus jeder Nachbardatei.
        for (int i = 0; i < 4; i++) {
            if (files[i]) {
                char nline[300], d[20], ti[20];
                float t, neighborDiff;
                if (fgets(nline, sizeof(nline), files[i]) &&
                    (sscanf(nline, "%19s %19s %f %f", d, ti, &t, &neighborDiff) == 4)) {
                    sum += neighborDiff;
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
    if(!fTDiff) {
        perror(tempDiffFile);
        printf("%s fehlt, merge unmoeglich \n", station);
        return;
    }
    FILE *fMean = fopen(meanFile, "r");
    if(!fMean) {
        perror(meanFile);
        printf("%s fehlt, merge unmoeglich \n", meanFile);
        fclose(fTDiff);
        return;
    }

    FILE *fFinal = fopen(finalFile, "w");
    if (!fFinal)
    {
        perror(finalFile);
        printf("%s konnte nicht erstellt werden\n", finalFile);

        fclose(fTDiff);
        fclose(fMean);

        return;
    }

    fprintf(fFinal, "Date	    Time     Temp (°C) 	   Temp Difference (°C)     Neighbour Temp Difference Mean\n");

    char line1[300];
    char line2[300];

    // Headerzeilen überspringen (falls vorhanden)
    if (!fgets(line1, sizeof(line1), fTDiff)) {
        // nichts zu mergen
        fclose(fTDiff); fclose(fMean); fclose(fFinal);
        return;
    }
    if (!fgets(line2, sizeof(line2), fMean)) {
        fclose(fTDiff); fclose(fMean); fclose(fFinal);
        return;
    }

    // Zeile für Zeile zusammenführen
    while (fgets(line1, sizeof(line1), fTDiff) && fgets(line2, sizeof(line2), fMean))
    {
        chomp(line1);
        chomp(line2);

        char date1[20], time1[20];
        float tempBase, diff, mean;

        if (sscanf(line1, "%19s %19s %f %f", date1, time1, &tempBase, &diff) != 4)
        {
            continue;
        }

        char date2[20], time2[20];
        if (sscanf(line2, "%19s %19s %f", date2, time2, &mean) != 3)
        {
            continue;
        }

        // Optional: prüfe, ob date1,time1 == date2,time2 (robustheit)
        fprintf(fFinal, "%s 	%s    %6.1f     		%6.1f      		    %6.1f\n", date1, time1, tempBase, diff, mean);
        
        
    }

    fclose(fTDiff);
    fclose(fMean);
    fclose(fFinal);

    // Temporäre Dateien löschen
    remove(tempDiffFile);
    remove(meanFile);

    printf("Merged gespeichert in %s\n", finalFile);

}


/* 2. Aufgabe mit OpenMP

Grund: Wir haben OpenMP verwendet, weil die Berechnungen der einzelnen Wetterstationen weitgehend unabhängig sind und sich daher sehr einfach und effizient 
im Shared-Memory-Modell parallelisieren lassen. Außerdem ermöglicht OpenMP eine schnelle Implementierung mit nur wenigen Codeanpassungen, 
was es für diese Aufgabe besonders geeignet macht.


Idee: 1.: Wir können alle _TEMP_DIFF.csv für jede Station gleichzeitig parallel erstellen, da wir keine Abhängigkeiten haben
Danach 2.: Wir können nun alle _NEIGHBOUR_MEAN.csv für jede Station gleichzeitig parallel erstellen, da für jede Station nun die Nachbarwerte vorlegen
Und am Ende 3.: Wir können nun die jewilige _TEMP_DIFF.csv & _NEIGHBOUR_MEAN.csv der Stationen mergen und erstellen Parallel für jede Station _FINAL.csv */

// Main
int main(void)
{
#if Aufgabe ==  1

    // tracke böntigte Zeit für Aufgabe 3
    double start = omp_get_wtime();

    DIR *d;
    struct dirent *dir;

    d = opendir(".");
    if (!d) {
        printf("Verzeichnis kann nicht geoeffnet werden!\n");
        return 1;
    }

    // Durchlaufe alle Dateien im aktuellen Ordner
    while ((dir = readdir(d)) != NULL) {
        if (strstr(dir->d_name, ".csv") != NULL &&
            strstr(dir->d_name, "_TEMP_DIFF.csv") == NULL &&
            strstr(dir->d_name, "_NEIGHBOUR_MEAN.csv") == NULL &&
            strstr(dir->d_name, "_FINAL.csv") == NULL)
        {
            printf("\n>>> Verarbeite Datei: %s\n", dir->d_name);
            tempDiffStation(dir->d_name);

            // Stationsnamen bereinigen
            char station[200];
            strcpy(station, dir->d_name);
            char *firstUnderscore = strchr(station, '_');
            if (firstUnderscore) memmove(station, firstUnderscore + 1, strlen(firstUnderscore));
            char *suffix = strstr(station, "_2022");
            if (suffix) *suffix = '\0';

            printf(">>> Berechne Mittelwert: %s\n", station);
            rechneMittelwert(station);

            mergeCSV(station);
        }
    }

    closedir(d);

    double end = omp_get_wtime();
    printf("Zeit: %f\n", end - start);
    
    printf("Final CSV's fuer alle Stationen erstellt, ausser fuer Messturm-2, denn dafuer gibt keine Temperaturwerte.\n");
    return 0;

#elif Aufgabe ==  2

    // tracke böntigte Zeit für Aufgabe 3
    double start = omp_get_wtime();

    DIR *d;
    struct dirent *dir;

    d = opendir(".");
    if (!d) {
        printf("Verzeichnis kann nicht geoeffnet werden!\n");
        return 1;
    }

    const int MAXFILES = 128;
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
            // Anstatt jede Datei direkt zu verarbeiten speichern wir diese 
            files[fileCount] = strdup(dir->d_name);
            
            // Stationsnamen bereinigen
            char station[200];
            strcpy(station, dir->d_name);
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

    #pragma omp parallel for schedule(dynamic)
    for (int i = 0; i < fileCount; i++) 
    {   
        printf(">>> Thread %d Verarbeitet die Datei: %s \n", omp_get_thread_num(), files[i]);
        tempDiffStation(files[i]);
    }

    // Stelle sicher, dass Phase 1 abgeschlossen ist (implizite Barrier am Ende von parallel for)
    printf("=== Phase 1 (temp diffs) abgeschlossen ===\n");

    // 2. Erzeuge nun alle _NEIGHBOUR_MEAN.csv für jede Station gleichzeitig Parallel 

    #pragma omp parallel for schedule(dynamic)
    for (int i = 0; i < fileCount; i++)
    {
        printf(">>> Thread %d berechnet Mittelwert: %s \n", omp_get_thread_num(), stations[i]);
        rechneMittelwert(stations[i]);
        
    }

    printf("=== Phase 2 (neighbour means) abgeschlossen ===\n");

    // 3. Merge nun die _TEMP_DIFF.csv & _NEIGHBOUR_MEAN.csv jeder Stationen zu _FINAL.csv für jede Station gleichzeitig Parallel

    #pragma omp parallel for schedule(dynamic)
    for (int i = 0; i < fileCount; i++)
    {
        printf(">>> Thread %d Merges: %s \n", omp_get_thread_num(), stations[i]);
        mergeCSV(stations[i]);
        
    }

    double end = omp_get_wtime();
    printf("Zeit: %f\n", end - start);

    printf("Final CSV's fuer alle Stationen erstellt, ausser fuer Messturm-2, denn dafuer gibt keine Temperaturwerte.\n");

    // Speicher aufräumen
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
