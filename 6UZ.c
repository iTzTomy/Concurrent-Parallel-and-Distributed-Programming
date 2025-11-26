// 6. Übungszettel - NPVP
// Hristo Popovski & Tomy Duong 

// 1. Aufgabe: Produktion in C
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define NUM_THREADS 5 
#define NUM_PLACES 3
#define TOTAL_ITEMS 1000

#define NUM_PRODUCER 10
#define NUM_CONSUMER 5

#define NUM_TAKE 5 // Aufgabe 3, gewählte Anzahl der Produkte welche die Konsumenten abholen können 

// Zähler
int produced_count = 0; 
int consumed_count = 0; 
int all_produced = 0;

int last;
int buffer[NUM_PLACES];

// Monitor 
pthread_cond_t notEmpty;
pthread_cond_t notFull;

// Mutex
pthread_mutex_t mutex;

void* Producer(void *threadid);
void* Consumer(void *threadid);
void* Consumer2(void *threadid);
void* Producer2(void *threadid);

void ablegen(int item, void *threadid);
int entnehmen(void *threadid);

void ablegen2(int item, void *threadid);
void enqueue(int item);

int main(void)
{
    pthread_t ProducerT[NUM_PRODUCER];
    pthread_t ConsumerT[NUM_CONSUMER];

#if Aufgabe ==  1

    // Initialisere Mutex
    pthread_mutex_init(&mutex, NULL);

    // Initialisere Monitor
    pthread_cond_init(&notEmpty, NULL);
    pthread_cond_init(&notFull, NULL);
    
    // erstelle Buffer 
    
    for (long t = 0; t < NUM_PLACES; t++)
    {
        buffer[t] = 0;
    
    }

    // Erzeuge Threads
    
    for (long t = 0; t < NUM_PRODUCER; t++)
    {   
        pthread_create(&ProducerT[t], NULL, Producer, (void*)t);    
    }

    for (long t = 0; t < NUM_CONSUMER; t++)
    {   
        pthread_create(&ConsumerT[t], NULL, Consumer, (void*)t);    
    }

    // Joine Threads (Producers)
    for (int t = 0; t < NUM_PRODUCER; t++)
    {
        pthread_join(ProducerT[t], NULL);
    } 

    // Alle Items produziert & wecke alle Consumer auf

    pthread_mutex_lock(&mutex);
    all_produced = 1;
    pthread_cond_broadcast(&notEmpty);
    pthread_mutex_unlock(&mutex);
    
    // Joine Threads (Consumer)
    for (int t = 0; t < NUM_CONSUMER; t++)
    {
        pthread_join(ConsumerT[t], NULL);
    }

    // Destroy Monitor
    pthread_cond_destroy(&notEmpty);
    pthread_cond_destroy(&notFull);

    // Destroy Mutex
    pthread_mutex_destroy(&mutex);

    printf("%d Items Produced and %d Items consumed. \n", produced_count, consumed_count);
    

    return 0;

#elif Aufgabe == 2

    // Initialisere Mutex
    pthread_mutex_init(&mutex, NULL);

    // Initialisere Monitor
    pthread_cond_init(&notEmpty, NULL);
    pthread_cond_init(&notFull, NULL);
    
    // erstelle Buffer 
    
    for (long t = 0; t < NUM_PLACES; t++)
    {
        buffer[t] = 0;
    
    }

    // Erzeuge Threads
    
    for (long t = 0; t < NUM_PRODUCER; t++)
    {   
        pthread_create(&ProducerT[t], NULL, Producer2, (void*)t);    
    }

    for (long t = 0; t < NUM_CONSUMER; t++)
    {   
        pthread_create(&ConsumerT[t], NULL, Consumer, (void*)t);    
    }

    // Joine Threads (Producers)
    for (int t = 0; t < NUM_PRODUCER; t++)
    {
        pthread_join(ProducerT[t], NULL);
    } 

    // Alle Items produziert & wecke alle Consumer auf

    pthread_mutex_lock(&mutex);
    all_produced = 1;
    pthread_cond_broadcast(&notEmpty);
    pthread_mutex_unlock(&mutex);
    
    // Joine Threads (Consumer)
    for (int t = 0; t < NUM_CONSUMER; t++)
    {
        pthread_join(ConsumerT[t], NULL);
    }

    // Destroy Monitor
    pthread_cond_destroy(&notEmpty);
    pthread_cond_destroy(&notFull);

    // Destroy Mutex
    pthread_mutex_destroy(&mutex);

    printf("%d Items Produced and %d Items consumed. \n", produced_count, consumed_count);


    return 0;

#elif Aufgabe == 3

    // Initialisere Mutex
    pthread_mutex_init(&mutex, NULL);

    // Initialisere Monitor
    pthread_cond_init(&notEmpty, NULL);
    pthread_cond_init(&notFull, NULL);
    
    // erstelle Buffer 
    
    for (long t = 0; t < NUM_PLACES; t++)
    {
        buffer[t] = 0;
    
    }

    // Erzeuge Threads
    
    for (long t = 0; t < NUM_PRODUCER; t++)
    {   
        pthread_create(&ProducerT[t], NULL, Producer2, (void*)t);    
    }

    for (long t = 0; t < NUM_CONSUMER; t++)
    {   
        pthread_create(&ConsumerT[t], NULL, Consumer2, (void*)t);    
    }

    // Joine Threads (Producers)
    for (int t = 0; t < NUM_PRODUCER; t++)
    {
        pthread_join(ProducerT[t], NULL);
    } 

    // Alle Items produziert & wecke alle Consumer auf

    pthread_mutex_lock(&mutex);
    all_produced = 1;
    pthread_cond_broadcast(&notEmpty);
    pthread_mutex_unlock(&mutex);
    
    // Joine Threads (Consumer)
    for (int t = 0; t < NUM_CONSUMER; t++)
    {
        pthread_join(ConsumerT[t], NULL);
    }

    // Destroy Monitor
    pthread_cond_destroy(&notEmpty);
    pthread_cond_destroy(&notFull);

    // Destroy Mutex
    pthread_mutex_destroy(&mutex);

    printf("%d Items Produced and %d Items consumed. \n", produced_count, consumed_count);


    return 0;

#else

    
    printf("Diese Aufgabe existiert nicht. Schreibe z.B. make Aufgabe1\n");
    return 1;

    #endif

    return 0;

}


// Aufgabe 1

void ablegen(int item, void *threadid)
{
    pthread_mutex_lock(&mutex);

    while (last == NUM_PLACES)
    {
        pthread_cond_wait(&notFull, &mutex); // wenn Voll, warte bis nicht mehr Voll
    }
    
    buffer[last] = item;
    last++;
    printf("Producer %d puts %d at place %d \n", ((long) threadid) + 1, item, last);
    
    pthread_cond_signal(&notEmpty); // signalisiere nicht mehr leer 
    pthread_mutex_unlock(&mutex);
}



int entnehmen(void *threadid)
{   
    pthread_mutex_lock(&mutex);

    while (last == 0)
    {
        if (all_produced)                   // wenn bereits alle Produkte produziert, mache nichts 
        {
            pthread_mutex_unlock(&mutex);
            return -1;
        }
    
        pthread_cond_wait(&notEmpty, &mutex); // wenn leer, warte bis nicht mehr leer
        
    }
    

    last--;
    int item = buffer[last];
    printf("Consumer %d takes %d \n", ((long) threadid) + 1, item);

    pthread_cond_signal(&notFull);  // signalisiere nicht mehr Voll
    pthread_mutex_unlock(&mutex);
    
    return item;
}


void* Producer (void *threadid)
{
    int item;

    while(1)
    {
        pthread_mutex_lock(&mutex);
        
        if (produced_count >= TOTAL_ITEMS) 
        {
            pthread_mutex_unlock(&mutex);
            break;                          // Ziel erreicht alle Items fertig
        }

        item = produced_count;
        produced_count++;

        pthread_mutex_unlock(&mutex);
        
        ablegen(item, threadid);  // Item in Puffer Legen

        
    }
    
    pthread_exit(NULL);
}


void* Consumer (void *threadid)
{
    while(1)
    {

        int item = entnehmen(threadid);

        if (item == -1) // wenn alle Items schon Produziert 
        {
            break;
        }
        
        pthread_mutex_lock(&mutex);
        
        consumed_count++;

        pthread_mutex_unlock(&mutex);

    }
    pthread_exit(NULL);
}

// Aufgabe 2
// Idee: FIFO Warteschlange, wenn wir Platz haben, rücken wir jedes Element um eins nach vorne uns fügen das aktuelle Element am Anfang ein
// Entnommen wird am Ende des Buffers, als das erste was in den Buffer rein ging. 

void ablegen2(int item, void *threadid)
{
    pthread_mutex_lock(&mutex);

    while (last == NUM_PLACES)
    {
        pthread_cond_wait(&notFull, &mutex); // wenn Voll, warte bis nicht mehr Voll
    }
    
    
    enqueue(item); // füge Item am Ende der FIFO Warteschlange hinzu
    printf("Producer %d puts %d at place %d \n", ((long) threadid) + 1, item, last);


    pthread_cond_signal(&notEmpty); // signalisiere nicht mehr leer 
    pthread_mutex_unlock(&mutex);
}

void enqueue(int item)
{
    if (last >= NUM_PLACES)
    {
        printf("Buffer Overflow!\n");
        return;
    }

    // Rücke jedes Item eins nach rechts
    for (int i = last; i > 0; i--)
    {
        buffer[i] = buffer[i - 1];
    }

    // Für akteulle Item am Anfang ein
    buffer[0] = item;
    last++;
}


void* Producer2 (void *threadid)
{
    int item;

    while(1)
    {
        pthread_mutex_lock(&mutex);
        
        if (produced_count >= TOTAL_ITEMS) 
        {
            pthread_mutex_unlock(&mutex);
            break;                          // Ziel erreicht alle Items fertig
        }

        item = produced_count;
        produced_count++;

        pthread_mutex_unlock(&mutex);
        
        ablegen2(item, threadid);  // Item in Puffer Legen
        
    }
    
    pthread_exit(NULL);
}


// Aufgabe 3


void* Consumer2(void *threadid)
{
    while (1)
    {
        int taken = 0;  // tracke wie viel schon genommen wurde 

        while (taken < NUM_TAKE)
        {
            int item = entnehmen(threadid);

            // Wenn schon alle Produkte produziert und kein Produkt mehr im Buffer, exit
            if (item == -1)
            {
                pthread_exit(NULL);
            }
                
            pthread_mutex_lock(&mutex);
            consumed_count++;
            pthread_mutex_unlock(&mutex);

            taken++;
        }
    }
    pthread_exit(NULL);
}
