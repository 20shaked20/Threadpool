#include <pthread.h> /*for threads*/

// move to header // 
typedef struct Node
{
    pthread_t *next; //the next thread//
    pthread_t *q_val; // the current thread//

} node, *pnode;

typedef struct Queue
{
    pnode head;
    pnode tail;
    pthread_cond_t cond;
    pthread_mutex_t mutex;

} q, *ptr_q;

/////////////////////////////////

static ptr_q init_queue()
{

    /*init queue details*/
    ptr_q queue = (ptr_q)malloc(sizeof(q));
    queue->head = NULL;
    queue->tail = NULL;

    /*init cond & mutex*/
    pthread_mutex_init(&queue->mutex, NULL);
    pthread_cond_init(&queue->cond, NULL);

    return queue;
}

void free_queue(ptr_q queue)
{

    while (queue->head != NULL)
    {
        /*this will free all the elements in the queue.*/
        pnode to_delete = queue->head;
        queue->head = queue->head->next;
        free(to_delete);
    }
    /*destroy mutex and cont, also free the queue it self*/
    pthread_mutex_destroy(&queue->mutex);
    pthread_cond_destroy(&queue->cond);
    free(queue);
    queue = NULL;
}

void enqueue(ptr_q queue, pthread_t *elem)
{
    /*Create new element*/
    pnode new_thread_node = (pnode)malloc(sizeof(pnode));
    new_thread_node->q_val = elem;
    new_thread_node->next = NULL;

    if (queue->head == NULL)
    {
        /*in case head i empty, we will simply state the new element to be head & tail.*/
        queue->head = new_thread_node;
        queue->tail = new_thread_node;
    }
    else
    {
        /*in case queue i not empty, we will head the element to be last*/
        queue->tail->next = new_thread_node;
        queue->tail = new_thread_node;
    }
}

void *dequeue(ptr_q queue)
{
    /*simly check if the queue is empty*/
    if (queue->head == NULL)
    {
        printf("Error: Queue is empty\n");
        return NULL;
    }
    /*fetch the node we want to get and then return its value*/
    /*CHECK FREE*/
    pnode return_node = queue->head;
    queue->head = queue->head->next;
    if (queue->head == NULL)
    {
        queue->tail = NULL;
    }
    return return_node->q_val;
}



//SEE if this is needed while implementing//
/**
 * @brief Queue cond implementation
 *
 */

struct CondQueue
{
    ptr_q queue;
    void *value;
};

pthread_mutex_t mutex_init = PTHREAD_MUTEX_INITIALIZER;

void *enQ(void *elem)
{
    /*init a cond queue*/
    struct CondQueue *cond_queue = (struct CondQueue *)elem;
    ptr_q queue = cond_queue->queue; /*the existense queue*/
    void *val = cond_queue->value;

    /*lock*/
    pthread_mutex_lock(&queue->mutex);

    enqueue(val, queue);
    printf("enqueue(cond) -> %d \n", *(int *)val); /*prints data for debuggin*/

    /*unlock*/
    pthread_mutex_unlock(&queue->mutex);

    /*condition variable*/
    pthread_cond_broadcast(&queue->cond);
}

char *deQ(void *elem)
{
    /*init a cond queue*/
    struct CondQueue *cond_queue = (struct CondQueue *)elem;
    ptr_q queue = cond_queue->queue; /*the existense queue*/
    void *val = cond_queue->value;

    /*lock*/
    pthread_mutex_lock(&queue->mutex);
    void *q_node;

    /*Wait condition*/
    while ((q_node = dequeue(queue)) == NULL)
    {
        printf("dequeue(cond) -> queue is waiting... \n");
        /*condi*/
        pthread_cond_wait(&queue->cond, &queue->mutex);
    }

    printf("dequeue(cond) -> %d\n", *(int *)q_node); /*debug*/
    val = q_node;

    /*unlock*/
    pthread_mutex_unlock(&queue->mutex);
    return val;
}

ptr_q createQ()
{
    /*lock*/
    pthread_mutex_lock(&mutex_init);

    ptr_q q = init_queue();
    printf("createQ(cond)... \n");

    /*unlock*/
    pthread_mutex_unlock(&mutex_init);

    return q;
}
void destroyQ(ptr_q queue)
{
    /*lock*/
    pthread_mutex_lock(&mutex_init);

    printf("destroyQ(cond)... \n");
    free_queue(queue);

    /*unlock*/
    pthread_mutex_unlock(&mutex_init);
}
