// 5. Übungszettel - NPVP
// Hristo Popovski & Tomy Duong 

// 1. Aufgabe: Produktion in C
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define NUM_THREADS 5 
#define NUM_PLACES 3
#define TOTAL_ITEMS 1000

#define NUM_PRODUCER 10
#define NUM_CONSUMER 5

sem_t empty;  
sem_t full;
sem_t mutex;

// Zähler
int produced_count = 0; 
int consumed_count = 0; 
int all_produced = 0;

int last;
int buffer[NUM_PLACES];



void* Producer(void *threadid);
void* Consumer(void *threadid);

// für Aufgabe 2
void* ProducerNoWait(void *threadid);
void* Consumer2(void *threadid);
int *bufferDynamic; 
int capacity = NUM_PLACES;

int main(void)
{
    pthread_t ProducerT[NUM_PRODUCER];
    pthread_t ConsumerT[NUM_CONSUMER];

#if Aufgabe ==  1
    // initialisere Semaphoren 
    
    sem_init(&full, 0, NUM_PLACES);
    sem_init(&empty, 0, 0);
    sem_init(&mutex, 0, 1);

    
    // erstelle Buffer 
    
    for (long t = 0; t < NUM_PLACES; t++)
    {
        buffer[t] = 0;
        last = 0;
    }

    // erstelle die Threads (Producer)

    for (long t = 0; t < NUM_PRODUCER; t++)
    {   
        pthread_create(&ProducerT[t], NULL, Producer, (void*)t);    
    }

    // erstelle die Threads (Consumer)

    for (long t = 0; t < NUM_CONSUMER; t++)
    {   
        pthread_create(&ConsumerT[t], NULL, Consumer, (void*)t);    
    }
    
    // joine die Threads (Producer)
    
    for (int t = 0; t < NUM_PRODUCER; t++)
    {
        pthread_join(ProducerT[t], NULL);
    } 
    
    // Alle Items produziert
    all_produced = 1;
    
    // Wecke alle Consumer auf
    for (int i = 0; i < NUM_CONSUMER; i++) 
    {
        sem_post(&empty);                        
    }
    // joine die Threads (Consumer)
    
    for (int t = 0; t < NUM_CONSUMER; t++)
    {
        pthread_join(ConsumerT[t], NULL);
    } 

    // release Semaphoren

    sem_destroy(&mutex);
    sem_destroy(&empty);
    sem_destroy(&full);

#elif Aufgabe == 2

    sem_init(&empty, 0, 0);
    sem_init(&mutex, 0, 1);

    
    // erstelle Buffer

    bufferDynamic = malloc(NUM_PLACES * sizeof(int));


    // erstelle die Threads (Producer)

    for (long t = 0; t < NUM_PRODUCER; t++)
    {   
        pthread_create(&ProducerT[t], NULL, ProducerNoWait, (void*)t);    
    }

    // erstelle die Threads (Consumer)

    for (long t = 0; t < NUM_CONSUMER; t++)
    {   
        pthread_create(&ConsumerT[t], NULL, Consumer2, (void*)t);    
    }
    
    // joine die Threads (Producer)
    
    for (int t = 0; t < NUM_PRODUCER; t++)
    {
        pthread_join(ProducerT[t], NULL);
    } 
    
    // Alle Items produziert
    all_produced = 1;
    
    // Wecke alle Consumer auf
    for (int i = 0; i < NUM_CONSUMER; i++) 
    {
        sem_post(&empty);                        
    }
    // joine die Threads (Consumer)
    
    for (int t = 0; t < NUM_CONSUMER; t++)
    {
        pthread_join(ConsumerT[t], NULL);
    } 

    // release Semaphoren

    sem_destroy(&mutex);
    sem_destroy(&empty);

#else

    printf("Diese Aufgabe existiert nicht. Schreibe z.B. make Aufgabe1\n");
    return 1;

    #endif

    return 0;

}

void* Producer (void *threadid)
{
    int item;

    while(1)
    {
        sem_wait(&mutex);   // Kritischen Abschnitt eintreten
        
        if (produced_count >= TOTAL_ITEMS) 
        {
            sem_post(&mutex);
            break;  // Ziel erreicht
        }

        item = produced_count;
        produced_count++;

        sem_post(&mutex);
        sem_wait(&full);    // Auf einen freien Platz im Regal warten
        sem_wait(&mutex);   // Kritischen Abschnitt verlassen

        buffer[last] = item;
 
        printf("Producer %d puts %d into buffer at place %d \n", ((long) threadid) + 1, item, last);
        
        last++;

        sem_post(&mutex);   // Kritischen Abschnitt verlassen
        sem_post(&empty);   // Regal ist gefüllt
    }
    
    pthread_exit(NULL);
}

void* Consumer (void *threadid)
{

    while(1)
    {
        sem_wait(&empty);   // Auf das Produkt im Regal warten
        sem_wait(&mutex);   // Kritischen Abschnitt eintreten
        
        if (all_produced && last == 0) 
        {
            sem_post(&mutex);
            sem_post(&empty);  // Signal zurückgeben
            break;
        }

        // Falls Puffer leer, aber noch nicht alles produziert
        if (last <= 0) 
        {
            sem_post(&mutex);
            sem_post(&empty);
            continue;
        }

        // Item vom Ende nehmen
        int pos = last - 1;
        int item = buffer[pos];
        printf("Consumer %d takes %d \n", ((long) threadid) + 1, item);
              
        last--;
        consumed_count++;

        sem_post(&mutex);   // Kritischen Abschnitt verlassen
        sem_post(&full);    // Regal nicht mehr voll 
    }
    pthread_exit(NULL);
}

// 2. Aufgabe: Optimierte Produktion

// Wir lösen das Problem, indem wir ein Dynamic Buffer implementieren:

void* ProducerNoWait(void *threadid)
{
    int item;

    while (1)
    {
        sem_wait(&mutex);

        if (produced_count >= TOTAL_ITEMS)
        {
            sem_post(&mutex);
            break;
        }

        item = produced_count;
        produced_count++;

        
        // Buffer vergrößern wenn voll
        if ((float)last / (float)capacity >= 0.8f)
        {
            float newCapacity = (float)capacity * 1.5;
            bufferDynamic = realloc(bufferDynamic, newCapacity * sizeof(int));
            capacity = newCapacity;

            printf("buffer expanded to new capacity: %d\n", capacity);
        }

        // Schreiben in Buffer
        bufferDynamic[last] = item;

        printf("Producer %ld puts %d at pos %d\n",
               (long)threadid + 1, item, last);

        last++;

        sem_post(&mutex);
        sem_post(&empty);
    }

    pthread_exit(NULL);
}

void* Consumer2 (void *threadid)
{

    while(1)
    {
        sem_wait(&empty);   // Auf das Produkt im Regal warten
        sem_wait(&mutex);   // Kritischen Abschnitt eintreten
        
        if (all_produced && last == 0) 
        {
            sem_post(&mutex);
            sem_post(&empty);  // Signal zurückgeben
            break;
        }

        // Falls Puffer leer, aber noch nicht alles produziert
        if (last <= 0) 
        {
            sem_post(&mutex);
            sem_post(&empty);
            continue;
        }

        // Item vom Ende nehmen
        int pos = last - 1;
        int item = bufferDynamic[pos];
        printf("Consumer %d takes %d \n", ((long) threadid) + 1, item);
              
        last--;
        consumed_count++;

        sem_post(&mutex);   // Kritischen Abschnitt verlassen
        // sem_post(&full);    // wird nicht mehr gebraucht wegen Dynamic Buffer
    }
    pthread_exit(NULL);
}

// 3. Aufgabe: Bewertung der Lösungen in der PDF