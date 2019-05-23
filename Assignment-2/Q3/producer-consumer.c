#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdbool.h>
#include <unistd.h>

#define RAND_DIVISOR 100000000
#define numProd 3
#define numCons 2
 
// A structure to represent a queue
struct Queue
{
    int front, rear, size;
    unsigned capacity;
    int* array;
};
 
// function to create a queue of given capacity. 
// It initializes size of queue as 0
struct Queue* createQueue(unsigned capacity)
{
    struct Queue* queue = (struct Queue*) malloc(sizeof(struct Queue));
    queue->capacity = capacity;
    queue->front = queue->size = 0; 
    queue->rear = capacity - 1;  // This is important, see the enqueue
    queue->array = (int*) malloc(queue->capacity * sizeof(int));
    return queue;
}

// Function to add an item to the queue.  
// It changes rear and size
void enqueue(struct Queue* queue, int item)
{
    if (queue->size == queue->capacity)
        return;
    queue->rear = (queue->rear + 1)%queue->capacity;
    queue->array[queue->rear] = item;
    queue->size = queue->size + 1;
}
 
// Function to remove an item from queue. 
// It changes front and size
int dequeue(struct Queue* queue)
{
    if (queue->size == 0)
        return -1;
    int item = queue->array[queue->front];
    queue->front = (queue->front + 1)%queue->capacity;
    queue->size = queue->size - 1;  
    return item;
}

void print(struct Queue* queue){
   if (queue->size > 0) {
      if (queue->rear >= queue->front) {
        for (int i = queue->front; i <= queue->rear; i++)
            printf("%d ",queue->array[i]);
      } else {
        for (int i = queue->front; i < queue->capacity; i++)
            printf("%d ", queue->array[i]);

        for (int i = 0; i <= queue->rear; i++)
            printf("%d ", queue->array[i]);
      }
   }
}



struct Queue* buffer;
struct Queue* produceOverflow;
struct Queue* consumeOverflow;

/* The mutex lock */
pthread_mutex_t mutex;

int count = 0;

pthread_t producer_threads[3], consumer_threads[2];       //Thread ID
pthread_attr_t attr; //Set of thread attributes

sem_t producer_sem[numProd], consumer_sem[numCons], prod_main[numProd], cons_main[numCons];

int prod[numProd], cons[numCons];

void addOverflowToBuffer() {
   while (buffer->size < buffer->capacity && produceOverflow->size > 0) {
      enqueue(buffer, dequeue(produceOverflow));
   }
}

void produce(int val) {
   addOverflowToBuffer();
   while (val--) {
      if (buffer->size < buffer->capacity) {
         enqueue(buffer, ++count);
      }
      else {
         enqueue(produceOverflow, ++count);
      }
   }
   // printf("\n Buffer- ");
   // print(buffer);
   // printf("\nProduce Overflow- ");
   // print(produceOverflow);

}


void consume(int val) {
   while (consumeOverflow->size > 0 && buffer->size > 0) {
      dequeue(buffer);
      dequeue(consumeOverflow);
      addOverflowToBuffer();
   }
   while (val > 0 && buffer->size > 0) {
      dequeue(buffer);
      addOverflowToBuffer();
      val -= 1;
   }

   while (val--) {
      enqueue(consumeOverflow, 1);
   }

   // printf("\n Buffer- size=%d  ", buffer->size);
   // print(buffer);
   // printf("\nConsume Overflow- ");
   // print(consumeOverflow);
   // printf("\nProduce Overflow- ");
   // print(produceOverflow);
}

/* Producer Thread */
void *producer(void *id) {
   int number = (int*)id;

   while(true) {

      sem_wait(&producer_sem[number]);

      pthread_mutex_lock(&mutex);

      // printf("\nIn producer %d, read %d\n", number, prod[number]);

      produce(prod[number]);


      pthread_mutex_unlock(&mutex);

      sem_post(&prod_main[number]);
   }
}

/* Consumer Thread */
void *consumer(void *id) {
   int number = (int*)id;

   while(true) {

      sem_wait(&consumer_sem[number]);
      
      pthread_mutex_lock(&mutex);

      // printf("\nIn consumer %d, read %d\n", number, cons[number]);

      consume(cons[number]);

      pthread_mutex_unlock(&mutex);

      sem_post(&cons_main[number]);
   }
}

int main(int argc, char *argv[]) {

  int buffer_size;

  scanf("%d", &buffer_size);

  /* Loop counter */
  int i;

  for (i = 0; i < numProd; i++) {
    sem_init(&producer_sem[i], 0, 0);
    sem_init(&prod_main[i], 0, 0);
  }

  for (i = 0; i < numCons; i++) {
    sem_init(&consumer_sem[i], 0, 0);
    sem_init(&cons_main[i], 0, 0);
  }

  /* Create the mutex lock */
  pthread_mutex_init(&mutex, NULL);

  /* Get the default attributes */
  pthread_attr_init(&attr);

  // Create the producer threads 
  for (i = 0; i < numProd; i++) {
    pthread_create(&producer_threads[i], &attr, producer, (void *)i);
  }

  // Create the consumer threads 
  for (i = 0; i < numCons; i++) {
    pthread_create(&consumer_threads[i], &attr, consumer, (void *)i);
  }

  buffer = createQueue(buffer_size);
  produceOverflow = createQueue(1000);
  consumeOverflow = createQueue(1000);


  char line[100];
  bool new_district = true;

  //Open the input file
  FILE *file;
  file = fopen("input.txt", "r");

  for (i = 0; i < numProd; i++) {
    // close(producer_pipe[i][0]);
  }
  for (i = 0; i < numCons; i++) {
    // close(consumer_pipe[i][0]);
  }

  //Read each line and increase wins
  while (fgets(line, sizeof(line), file)) {
    // note that fgets doesn't strip the terminating \n
    sscanf(line,"%d %d %d %d %d", &prod[0], &prod[1], &prod[2], &cons[0], &cons[1]);

    // printf("\n%d %d %d %d %d\n", prod[0], prod[1], prod[2], cons[0], cons[1]);

    for (i = 0; i < numProd; i++) {
       sem_post(&producer_sem[i]);
    }

    for (i = 0; i < numProd; i++) {
       sem_wait(&prod_main[i]);
    }

    for (i = 0; i < numCons; i++) {
       sem_post(&consumer_sem[i]);
    }

    for (i = 0; i < numCons; i++) {
       sem_wait(&cons_main[i]);
    }

    printf("Buffer Items: ");
    print(buffer);
    printf("\nOverflow Items: ");
    print(produceOverflow);
    printf("\n\n");
  }

  exit(0);
}