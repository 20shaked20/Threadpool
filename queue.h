// /**
//  * Authors: 
//  * Lior Nagar
//  * Shaked Levi
//  * 
//  * This header file is responsible for the queue of our threadpool.
// */
// #include <pthread.h> /*for threads*/ // ask arkadii about it <<

// typedef struct Node
// {
//     pthread_t *next; //the next thread//
//     pthread_t *q_val; // the current thread//

// } node, *pnode;

// typedef struct Queue
// {
//     pnode head;
//     pnode tail;
//     pthread_cond_t cond;
//     pthread_mutex_t mutex;

// } q, *ptr_q;


// /**
//  * Adds object to the queue
// */
// void enqueue(ptr_q queue, pthread_t *elem);

// /**
//  * Removes object from the queue
// */
// void *dequeue(ptr_q queue);

// /**
//  * Shows the top of the queue
// */
// void top();

// /**
//  * Shows the last of the queue
// */
// void tail();

// /**
//  * Free the queue of its members.
// */
// void free_queue(ptr_q queue);