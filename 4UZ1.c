// 4. Übungszettel - NPVP
// Hristo Popovski & Tomy Duong 

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>

// 1. Aufgabe:  Speisende Philosophen in C 

#define NUM_THREADS 5

sem_t essstaebchen[NUM_THREADS];

// mutex für Aufgabe 3
// pthread_mutex_t lockP;

void essen(int ph);
void denken(int ph);
void essstaebchen_verwenden(int i);
void *speisende_philosophen(void *philosoph);


void main(void)
{ 
    pthread_t philosoph[NUM_THREADS];

    // pthread_mutex_init(&lockP, NULL); // für Aufgabe 3

    // initialisere Semaphoren 
    for (int i = 0; i < NUM_THREADS; i ++)
    {
        sem_init(&essstaebchen[i], 0, 1);
    }

    // erstellt die Threads
    for (int t = 0; t < NUM_THREADS; t++)
    {   
        pthread_create(&philosoph[t], NULL, speisende_philosophen, (void*)t);    
    } 

    for (int t = 0; t < NUM_THREADS; t++)
    {
        pthread_join(philosoph[t], NULL);
    } 

    for (int i = 0; i < NUM_THREADS; i ++)
    {
        sem_destroy(&essstaebchen[i]);
    }

    // pthread_mutex_destroy(&lockP);
    
    return 0;

}

void essen(int ph)
{
    printf("Philosoph %d beginnt zu essen. \n", ph + 1);
    // sleep(1); // nicht mehr nötig nach Aufgabe 3
    printf("Philosoph %d ist fertig mit essen. \n", ph + 1);
}


void denken(int ph)
{
    printf("Philosoph %d denkt nach. \n", ph + 1);
    // sleep(1); // nicht mehr nötig nach Aufgabe 3
}


void essstaebchen_verwenden(int i)   
{ 

    int left = i;
    int right = (i + 1) % NUM_THREADS;
    
    // lock für Aufgabe 3
    // pthread_mutex_lock(&lockP);
    
    sem_wait(&essstaebchen[left]);
    sem_wait(&essstaebchen[right]);

    // unlock für Aufgabe 3
    // pthread_mutex_unlock(&lockP);

    essen(i);

    sem_post(&essstaebchen[left]);
    sem_post(&essstaebchen[right]);

}


void *speisende_philosophen(void *philosoph)
{
    long tid;
    tid = (long) philosoph;
    
    while (1)
    {
        denken(tid);

        sleep(1); 

        printf("Philosoph %d hat hunger...\n", tid + 1);

        essstaebchen_verwenden(tid);
    }

}

/* 2. Aufgabe: Verklemmungen

Ja, theoretisch kann es zu einem Deadlock kommen, wenn zum Beispiel jeder Philosoph mit "sem_wait(&essstaebchen[left])" 
zuerst den linken Essstaebchen nimmt und dann mit "sem_wait(&essstaebchen[right])" das rechte Essstaebchen nehmen will.
Nun wartet aber jeder Philosoph auf das rechte Essstaebchen, da sein rechter Nachbar diesen bereits genommen hat.
Somit kommt es zu keinen Fortschritt und wir haben einen Deadlock. 

Abfolge der Operationen die zum Deadlock führen:

Philosoph 1: sem_wait(&essstaebchen[left])
Philosoph 2: sem_wait(&essstaebchen[left])
Philosoph 3: sem_wait(&essstaebchen[left])
Philosoph 4: sem_wait(&essstaebchen[left])
Philosoph 5: sem_wait(&essstaebchen[left])

Philosoph 1: sem_wait(&essstaebchen[right])
Philosoph 2: sem_wait(&essstaebchen[right])
Philosoph 3: sem_wait(&essstaebchen[right])
Philosoph 4: sem_wait(&essstaebchen[right])
Philosoph 5: sem_wait(&essstaebchen[right])

Da wir sleep eingebaut haben kam es bei uns zu keinen Deadlock, aber da es in der Theorie trotzdem vorkommen kann, 
haben wir in der 3. Aufgabe Mutex Locking benutzt. Wir haben die Funktion ohne sleep probiert, und dann kam es zu einem Deadlock.

3. Aufgabe: Verklemmungsvermeidung

Um den Deadlock zu verhindern kann man Mutex Locking verwenden, wir stellen sicher das der Philosoph linken und rechten Essstaebchen erhält, 
bevor der nächste ein Essstaebchen nehmen kann. Da wir sleep nicht mehr benötigen ist die Funktion auch schneller. 

*/