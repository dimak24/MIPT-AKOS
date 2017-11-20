#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>


const long long infinity = 1e8;

long long *primes;
long long *notPrime;
long long N, K, n, last, last1;
pthread_mutex_t mutex;


long long min(long long a, long long b) {
    return a <= b ? a : b;
}


struct Segment {
    long long start, finish;
};


void* findPrimeNumbersOnSegment(void* args) {
    struct Segment* seg = args;
    for (long long i = 0; i < last1; ++i)
        for (long long j = seg->start - seg->start % primes[i]; 
            j <= seg->finish; j += primes[i]) {
            if (j < seg->start)
                continue;
            notPrime[j] = 1;
        }
    for (long long j = seg->start; j <= seg->finish; ++j)
        if (!notPrime[j]) {
            pthread_mutex_lock(&mutex);
            primes[last++] = j;
            pthread_mutex_unlock(&mutex);
        }
}


int main(int argc, char** argv) {
    pthread_mutex_init(&mutex, 0);
    if (argc < 2) {
        printf("error: the number of threads is not given\n");
        exit(11);
    }
    K = atoll(argv[1]);
    N = argc > 2 ? atoll(argv[2]) : infinity;
    primes = calloc(N + 1, sizeof(long long));
    if (!primes) {
        perror("calloc");
        exit(1);
    }
    notPrime = calloc(N + 1, sizeof(long long));
    if (!notPrime) {
        perror("calloc");
        exit(1);
    }
    notPrime[1] = 1;
    primes[0] = 2;
    last = 1;
    last1 = 1;
    n = 2;

    while (n < N) {
        long long up = min(n * n, N);
        long long k = min(K, up - n);
        long long size = (up - n) / k;
        long long x = up - n - size * k;
        pthread_t* threads = calloc(k, sizeof(pthread_t));
        if (!threads) {
            perror("calloc");
            exit(1);
        }
        for (int i = 0; i < k; ++i) {
            struct Segment* seg = calloc(1, sizeof(struct Segment));
            if (!seg) {
                perror("calloc");
                exit(1);
            }
            if (i < k - x) {
                seg->start = n + 1LL + size * i;
                seg->finish = min(up, seg->start + size - 1LL);
            }
            else {
                seg->start = n + 1LL + size * (k - x) + (size + 1LL) * (i - k + x);
                seg->finish = min(up, seg->start + size);
            }
            if (pthread_create(threads + i, NULL,
                findPrimeNumbersOnSegment, seg)) {
                    perror("pthread_create");
                    exit(1);
                }
        }
        for (long long i = 0; i < k; ++i)
            pthread_join(threads[i], NULL);

        //вывод найденных простых
        for (long long i = n; i <= up; ++i)
            if (!notPrime[i])
                printf("%lld ", i);

        n = up;
        last1 = last;
        free(threads);
    }

    free(primes);
    free(notPrime);
}