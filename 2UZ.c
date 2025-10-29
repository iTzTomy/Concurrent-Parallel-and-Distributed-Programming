#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdbool.h>
#include <time.h>
#include <unistd.h>

// 2. Übungszettel - NPVP
// Hristo Popovski & Tomy Duong 

// 1. Aufgabe
// Idee: Zwei Autos, Random Nummer zwischen 1 und 10, wenn Random Nummer = 5, dann geht das Auto auf die Brücke
// z.B. Auto1 bekommt Random Nummer = 5, wir setzen länge = 0, bei jeder Iteration fährt das Auto weiter (Länge +25)
// bekommt Auto2 aber nun auch Random Nummer = 5 und autoAufBruecke = true, dann Unfälle +=1
// wenn nicht, mache weiter, wenn Auto1 Länge = 100 hat, fertig und setze Überführungen +=1

// für Aufgabe 1 & 2 setze NUM_THREAD = 2, für Aufabe 3 kann es beliebig sein
#ifndef NUM_THREADS
#define NUM_THREADS 2
#endif


// shared data
int unfaelle = 0;
int autoAufBruecke = 0;
int maxUberquerung = 100000;
int uberquerung = 0;
bool crash = false;

// Peterson
int level[NUM_THREADS];
int last[NUM_THREADS];

int lockPeterson(long tid);
int unlockPeterson(long tid);

// Twofold Locking
char _lock[NUM_THREADS];

void *autoUberquerung(void *car);
void *autoUberquerungLock(void *car);
void *autoUberquerungLockP(void *car);


int randomNumber();

int main(void) 
{
    srand(time(NULL));
    
    pthread_t car[NUM_THREADS];

#if Aufgabe == 1

    for (long t = 0; t < NUM_THREADS; t++)
        pthread_create(&car[t], NULL, autoUberquerung, (void*)t);

    for (int t = 0; t < NUM_THREADS; t++)
        pthread_join(car[t], NULL);

#elif Aufgabe == 2

    for (long t = 0; t < NUM_THREADS; t++)
        pthread_create(&car[t], NULL, autoUberquerungLock, (void*)t);

    for (int t = 0; t < NUM_THREADS; t++)
        pthread_join(car[t], NULL);

#elif Aufgabe == 3

    for (long t = 0; t < NUM_THREADS; t++)
        pthread_create(&car[t], NULL, autoUberquerungLockP, (void*)t);

    for (int t = 0; t < NUM_THREADS; t++)
        pthread_join(car[t], NULL);

#else
    printf("Diese Aufgabe existiert nicht. Schreibe z.B. make Aufgabe1\n");
    return 1;

    #endif

    printf("Crashes: %d\n", unfaelle / 2);
    printf("Ueberquerungen: %d\n", uberquerung);

    return 0;
    
}

void *autoUberquerung(void *car)
{
    while (uberquerung < maxUberquerung)
    {
       
        usleep(randomNumber()*10);
        if (randomNumber() == 5)
        {
            autoAufBruecke++;

            for (int distance = 0; distance < 100; distance += 25)
            {
                
                usleep(randomNumber()*5);
                if (autoAufBruecke > 1)
                {
                    unfaelle++;
                    printf("Crash! \n");
                    crash = true;
                    break;
                }
                
            }

            if(crash == false)
            {
                autoAufBruecke--;
                uberquerung++;
                printf("uberquerung \n");
            }
            else
            {
                crash = false;
                autoAufBruecke--;
            }      
            
            
        }
        
    usleep(randomNumber()*10); 
    }
}


int randomNumber()
{
    int r = (rand() % 10)+1;  // 1..10
   // printf("%d\n", r);
    return r;
}


// 2. Aufgabe 

int lock(long tid)
{
    _lock[tid] = 1;

    while (_lock[NUM_THREADS - 1 - tid])
    {
        _lock[tid] = 0;
        usleep(randomNumber()*1);
        _lock[tid] = 1;
    }
    return 0;
}

int unlock (long tid)
{
    _lock[tid] = 0;
    return 0;
}


void *autoUberquerungLock(void *car)
{
    long tid;
    tid = (long) car;
    
    while (uberquerung < maxUberquerung)
    {
       
       
        usleep(randomNumber()*1);
        if (randomNumber() == 5)
        {
            // check for Lock
            // Lock urself
            if (_lock[tid] == 0)
            {
                lock(tid);
            }

            autoAufBruecke++;

            for (int distance = 0; distance < 100; distance += 25)
            {
                
                usleep(randomNumber()*1);
                if (autoAufBruecke > 1)
                {
                    unfaelle++;
                    printf("Crash! \n");
                    crash = true;
                    break;
                }
                
            }

            if(crash == false)
            {
                autoAufBruecke--;
                uberquerung++;
                printf("uberquerung \n");
                // unlock
                unlock(tid);
            }
            else
            {
                crash = false;
                autoAufBruecke--;    
            }            
            
        }
        
    usleep(randomNumber()*1); 
    }
}

// 3. Aufgabe

int lockPeterson(long tid)
{
    int i, j, loop;

    for(i = 1; i < NUM_THREADS; i++)
    {
        level[tid] = i;
        last[i] = tid;
        loop = 1;

        while((loop) && (last[i] == tid))
        {
            j = 0;
            loop = 0;
            
            while ((j < NUM_THREADS) && ((level[j] < i) || (j == tid)))
                j++;
            
            if (j < NUM_THREADS)
                loop = 1;
        }
    }
    return 0;
}

int unlockPeterson(long tid)
{
    level[tid] = 0;
    return 0;    
}


void *autoUberquerungLockP(void *car)
{
    long tid;
    tid = (long)car;

    while (true)
    {
        lockPeterson(tid);
        if (uberquerung >= maxUberquerung)
        {
	    //unlock
            unlockPeterson(tid);
            break;
        }


        if (randomNumber() == 5)
        {
            autoAufBruecke++;

            for (int distance = 0; distance < 100; distance += 25)
            {
                if (autoAufBruecke > 1)
                {
                    unfaelle++;
                    printf("Crash!\n");
                    crash = true;
                    break;
                }
            }

            if (crash == false)
            {
                autoAufBruecke--;
                uberquerung++;
                printf("uberquerung \n");
            }
            else
            {
                crash = false;
                autoAufBruecke--;
            }
        }

        unlockPeterson(tid);
    }

    return 0;
}


/*
Aufgabe 4

1. Keine Locks:
Da wir keine Locks haben die verhindern, dass mehr als ein Auto (Thread) auf der Brücke ist, haben wir viele crashes.
Deshalb läuft das Programm auch viel länger, da es länger braucht um die Anzahl der geforderten Überquerungen zu erreichen.

Maßnahme:
Implementiere Twofold locking oder POSIX Mutex, Dekker, Peterson, Lamport locking

2. Twofold Lock with Mutual Precedence:
Funktioniert gut mit zwei Autos (Threads), etwas langsam, aber keine crashes.
Wenn wir mehr als zwei Autos (Threads) hinzufügen, kommt es zu einem Deadlock, da Twofold Lock with Mutual Precedence
nur für 2 Autos (Threads) funktioniert. 

Maßnahme:
Implementiere POSIX Mutex locking oder Dekker, Peterson, Lamport

3. Multiple Locks with Mutual Access – Peterson
Funktioniert mit beliebiger Anzahl von Autos (Threads), keine Deadlocks durch, Programm läuft auch schneller.
*/ 
