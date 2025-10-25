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

#define NUM_THREADS 2 // für Aufgabe 1 & 2 setze NUM_THREAD = 2, für Aufabe 3 kann es beliebig sein

//shared data
int unfaelle = 0;
int autoAufBruecke = 0;
int maxUberquerung = 100000;
int uberquerung = 0;
bool crash = false;

//Twofold Locking
char _lock[NUM_THREADS];


void *autoUberquerung(void *car);
void *autoUberquerungLock(void *car);

int randomNumber();

int main(void) 
{
    srand(time(NULL));

    
    pthread_t car[NUM_THREADS];
    
    // erstellt die Threads
    for (int t = 0; t < NUM_THREADS; t++)
    {   
        pthread_create(&car[t], NULL, autoUberquerung, (void*)t); // für Aufgabe 2 ersetzte autoUberquerung mit autoUberquerungLock 
    }                                                             // für Aufgabe 3 ersetze autoUberquerung mit autoUberquerungLockN

    // Wartet, bis alle Threads beendet sind
    for (int t = 0; t < NUM_THREADS; t++)
    {
        pthread_join(car[t], NULL);
    }   

    printf("Crashes: %d\n", unfaelle/2);        // da Auto1 und Auto2 crashen, aber zusammen es nur 1 crash ist, durch zwei teilen
    printf("uberquerungen: %d\n",uberquerung);
        
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


int randomNumber()
{
    int r = (rand() % 10)+1;  // 1..10
   // printf("%d\n", r);
    return r;
}


// 2. Aufgabe 

// int lock(long tid)
// {
//     while (_lock[NUM_THREADS - 1 - tid])
//     {
//         _lock[tid] = 1;
//         return 0;
//     }   
// }

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
       // check for Lock
       // Lock urself
       if (_lock[tid] == 0)
       {
            lock(tid);

       }
        usleep(randomNumber()*1);
        if (randomNumber() == 5)
        {
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

void *autoUberquerungLockN(void *car)
{
    long tid;
    tid = (long) car;
    
    while (uberquerung < maxUberquerung)
    {
       // check for Lock
       // Lock urself
       if (_lock[tid] == 0)
       {
            lock(tid);

       }
        usleep(randomNumber()*1);
        if (randomNumber() == 5)
        {
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
