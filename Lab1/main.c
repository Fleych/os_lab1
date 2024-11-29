#include <stdio.h>
#include <pthread.h>

pthread_cond_t cond1 = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond2 = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
int event = 0;
int i = 0;

void provider() {
    while(1) {
        sleep(1);
        pthread_mutex_lock(&mutex);
        while (event == 1) {
            pthread_cond_wait(&cond1, &mutex);
            printf("Поставщик проснулся\n");
        }
        i += 1;
        event = 1;
        printf("Сообщение %d отправлено\n", i);
        pthread_cond_signal(&cond2);
        pthread_mutex_unlock(&mutex);
    }
}

void consumer() {
    while(1) {
        pthread_mutex_lock(&mutex);
        while (event == 0) {
            pthread_cond_wait(&cond2, &mutex);
            printf("Потребитель проснулся\n", i);
        }
        event = 0;
        printf("Сообщение %d принято\n", i);
        pthread_cond_signal(&cond1);
        pthread_mutex_unlock(&mutex);
    }
}

int main() {
    pthread_t p, c;
    pthread_create(&p, NULL, provider, NULL);
    pthread_create(&c, NULL, consumer, NULL);
    pthread_join(p, NULL);
    pthread_join(c, NULL);
    
    return 0;
}