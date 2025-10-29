#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdbool.h>
#include <time.h>

#define NUM_THREADS 5  // you can change this

int unfaelle = 0;
int autoAufBruecke = 0;
int uberquerung = 0;
bool crash = false;
int maxUberquerung = 100000;


int level[NUM_THREADS];
int last[NUM_THREADS];

int lockPeterson(long tid);
int unlockPeterson(long tid);
int randomNumber();

void *autoUberquerungLockP(void *car);


int main(void)
{
    srand((unsigned int)time(NULL));

    // Initialize levels
    for (int i = 0; i < NUM_THREADS; i++)
        level[i] = 0;

    pthread_t car[NUM_THREADS];

    for (long t = 0; t < NUM_THREADS; t++)
        pthread_create(&car[t], NULL, autoUberquerungLockP, (void *)t);

    for (int t = 0; t < NUM_THREADS; t++)
        pthread_join(car[t], NULL);

    printf("\nCrashes: %d\n", unfaelle / 2);
    printf("Ueberquerungen: %d\n", uberquerung);

    return 0;
}

int randomNumber()
{
    return (rand() % 10) + 1;
}

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
