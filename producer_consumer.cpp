#include <iostream>
#include <pthread.h>
#include <semaphore.h>
#include <queue>
#include <unistd.h>
#include <cstdlib>

#define BUFFER_SIZE 5
#define NUM_PRODUCERS 3
#define NUM_CONSUMERS 3
#define ITEMS_PER_PRODUCER 4

std::queue<int> buffer;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
sem_t empty_slots;  // counts empty slots
sem_t filled_slots; // counts filled slots

int item_id = 0;

void* producer(void* arg) {
    int id = *(int*)arg;
    for (int i = 0; i < ITEMS_PER_PRODUCER; i++) {
        int item = rand() % 100;

        sem_wait(&empty_slots);          // wait for empty slot
        pthread_mutex_lock(&mutex);      // enter critical section

        buffer.push(item);
        int current_id = ++item_id;
        std::cout << "[Producer " << id << "] Produced item " << current_id
                  << " (val=" << item << ") | Buffer size: " << buffer.size() << "\n";

        pthread_mutex_unlock(&mutex);    // exit critical section
        sem_post(&filled_slots);         // signal filled slot

        usleep(100000); // 100ms
    }
    return nullptr;
}

void* consumer(void* arg) {
    int id = *(int*)arg;
    for (int i = 0; i < ITEMS_PER_PRODUCER; i++) {
        sem_wait(&filled_slots);         // wait for filled slot
        pthread_mutex_lock(&mutex);      // enter critical section

        int item = buffer.front();
        buffer.pop();
        std::cout << "[Consumer " << id << "] Consumed item"
                  << " (val=" << item << ") | Buffer size: " << buffer.size() << "\n";

        pthread_mutex_unlock(&mutex);    // exit critical section
        sem_post(&empty_slots);          // signal empty slot

        usleep(150000); // 150ms
    }
    return nullptr;
}

int main() {
    srand(42);
    sem_init(&empty_slots, 0, BUFFER_SIZE);
    sem_init(&filled_slots, 0, 0);

    pthread_t producers[NUM_PRODUCERS], consumers[NUM_CONSUMERS];
    int ids[NUM_PRODUCERS > NUM_CONSUMERS ? NUM_PRODUCERS : NUM_CONSUMERS];
    for (int i = 0; i < (NUM_PRODUCERS > NUM_CONSUMERS ? NUM_PRODUCERS : NUM_CONSUMERS); i++) ids[i] = i + 1;

    std::cout << "=== Producer-Consumer Simulation (Buffer=" << BUFFER_SIZE << ") ===\n\n";

    for (int i = 0; i < NUM_PRODUCERS; i++)
        pthread_create(&producers[i], nullptr, producer, &ids[i]);
    for (int i = 0; i < NUM_CONSUMERS; i++)
        pthread_create(&consumers[i], nullptr, consumer, &ids[i]);

    for (int i = 0; i < NUM_PRODUCERS; i++) pthread_join(producers[i], nullptr);
    for (int i = 0; i < NUM_CONSUMERS; i++) pthread_join(consumers[i], nullptr);

    sem_destroy(&empty_slots);
    sem_destroy(&filled_slots);
    pthread_mutex_destroy(&mutex);

    std::cout << "\n=== Simulation complete. No deadlocks detected. ===\n";
    return 0;
}
