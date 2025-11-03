#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdbool.h>
#include <time.h>
#include <unistd.h>

// 3. Übungszettel - NPVP
// Hristo Popovski & Tomy Duong 

// 1. Aufgabe
// Idee: Zwei Autos, Random Nummer zwischen 1 und 10, wenn Random Nummer = 5, dann geht das Auto auf die Brücke
// z.B. Auto1 bekommt Random Nummer = 5, wir setzen länge = 0, bei jeder Iteration fährt das Auto weiter (Länge +25)
// bekommt Auto2 aber nun auch Random Nummer = 5 und autoAufBruecke = true, dann Unfälle +=1
// wenn nicht, mache weiter, wenn Auto1 Länge = 100 hat, fertig und setze Überführungen +=1

#define NUM_THREADS 5 

//shared data
int unfaelle = 0;
int autoAufBruecke = 0;
int maxUberquerung = 100000;
int uberquerung = 0;
bool crash = false;

// mutex
pthread_mutex_t lockP;

void *autoUberquerungMutex(void *car);

int randomNumber();

int main(void) 
{
    srand(time(NULL));
    
    pthread_t car[NUM_THREADS];
    pthread_mutex_init(&lockP, NULL);
    
    // erstellt die Threads
    for (int t = 0; t < NUM_THREADS; t++)
    {   
        pthread_create(&car[t], NULL, autoUberquerungMutex, (void*)t);    
    }                                                               

    // Wartet, bis alle Threads beendet sind
    for (int t = 0; t < NUM_THREADS; t++)
    {
        pthread_join(car[t], NULL);
    }   

    pthread_mutex_destroy(&lockP);

    printf("Crashes: %d\n", unfaelle/2);        // da Auto1 und Auto2 crashen, aber zusammen es nur 1 crash ist, durch zwei teilen
    printf("uberquerungen: %d\n",uberquerung);
        
    return 0;
    
}

void *autoUberquerungMutex(void *car)
{
    long tid;
    tid = (long) car;

    while (true)
    {
        // lock
        pthread_mutex_lock(&lockP);
        if (uberquerung >= maxUberquerung)
        {   
            // unlock
            pthread_mutex_unlock(&lockP);
            break;
        }
        // unlock
        pthread_mutex_unlock(&lockP);

       
        if (randomNumber() == 5)
        {
            // lock
            pthread_mutex_lock(&lockP);

            autoAufBruecke++;

            for (int distance = 0; distance < 100; distance += 25)
            {

                if (autoAufBruecke > 1)
                {
                    unfaelle++;
                    printf("Crash! \n");
                    crash = true;
                    // unlock
                    pthread_mutex_unlock(&lockP);
                    break;
                }

            }
            
            if(crash == false)
            {
                autoAufBruecke--;
                uberquerung++;
                printf("uberquerung \n");
                // unlock
                pthread_mutex_unlock(&lockP);
            }
            else
            {
                crash = false;
                autoAufBruecke--;
                // unlock
                pthread_mutex_unlock(&lockP);
            }      
        }
    }
}

int randomNumber()
{
    int r = (rand() % 10)+1;  // 1..10
   // printf("%d\n", r);
    return r;
}



/*2. Aufgabe 




*/

