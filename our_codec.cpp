/**
 * Main program of our_codec.h
 */
#include "codec.h"
#include "our_codec.hpp"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <string.h>
#include <sys/sysinfo.h> // used in getting the cores//
#include <pthread.h>
#include <cstring>
#include <fstream>
#include <sstream>
#include <iostream>
#include <queue> // used in task and thread pools
#include <dlfcn.h>

void (*encrypt_data)(char *s, int key);
void (*decrypt_data)(char *s, int key);
void *handle_task(void *arg);
void *handle_threads(void *arg);

using namespace std;


std::queue<struct Task> task_pool; // seprates the file into chuncks of tasks to encrypt/decrpyt.
std::queue<pthread_t> t_pool;      // queue holding all of our threads.
int active_tasks = 0;              // the current active tasks
int sum_of_tasks = 0;              // how many tasks we added at the start
int task_number = 0;               // number of current task we operate

// lock inits//
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t lockPrint = PTHREAD_MUTEX_INITIALIZER;

pthread_cond_t TaskCond = PTHREAD_COND_INITIALIZER;
pthread_cond_t PrintCond = PTHREAD_COND_INITIALIZER;
pthread_cond_t WaitCond = PTHREAD_COND_INITIALIZER;

pthread_t admin_thread; // the main thread that handles the 'mini' threads that operate on the tasks.

void init_thread_pool()
{
    int cores = get_nprocs_conf();

    for (int i = 0; i < cores; ++i)
    {
        pthread_t new_thread;
        t_pool.push(new_thread);
    }
}

int call_library_functions()
{
    void *handle = NULL;
    handle = dlopen("./libCodec.so", RTLD_NOW | RTLD_GLOBAL);
    if (handle == NULL)
    {
        fprintf(stderr, "Unable to open lib: %s\n", dlerror());
        return -1;
    }
    encrypt_data = (void (*)(char *s, int key))dlsym(handle, "encrypt");
    decrypt_data = (void (*)(char *s, int key))dlsym(handle, "decrypt");

    if (encrypt_data == NULL || decrypt_data == NULL)
    {
        fprintf(stderr, "Unable to get function\n");
        return -1;
    }

    return 0;
}

void *handle_threads(void *arg)
{
    while (true)
    {

        pthread_mutex_lock(&lock);

        struct Task *active_task = (struct Task *)malloc(sizeof(struct Task));

        while (task_pool.empty())
        { // simply wait untill there are tasks in the pool and start working.
            pthread_cond_wait(&TaskCond, &lock);
        }

        while (t_pool.empty())
        { // simply wait untill there are tasks in the pool and start working.
            pthread_cond_wait(&TaskCond, &lock);
        }

        // take a task to work from the pool //
        *active_task = task_pool.front();
        task_pool.pop();

        // init a thread from the pool on the taken task//
        pthread_create(&t_pool.front(), NULL, handle_task, (void *)active_task);

        t_pool.pop(); // removes the thread from the pool (we add it later, in order for it not to be used until finished)

        pthread_mutex_unlock(&lock);
    }
}

void *handle_task(void *arg)
{
    struct Task *active_task = (struct Task *)arg; // import the given task to a Task type

    char *charArray = &active_task->tape[0];
    if (active_task->type[1] == 'e')
    {
        encrypt_data(charArray, active_task->key);
    }
    else if (active_task->type[1] == 'd')
    {
        decrypt_data(charArray, active_task->key);
    }

    pthread_mutex_lock(&lockPrint);

    while (task_number < active_task->index) // this loop is used in order to make sure we print accordinly, and not in some messed up order.
    {
        pthread_cond_wait(&WaitCond, &lockPrint);
    }

    cout << active_task->tape << endl; // prints the data on tape.
    fflush(stdout);                                // clears stdout, is important to make sure the buffer is cleared, so we dont get some junk.

    task_number++; // increse the task number, is important for the loop above.
    pthread_cond_broadcast(&WaitCond);

    t_pool.push(pthread_self()); // thread is done, and we push it to the pool again so it can pick up new tasks.

    pthread_cond_broadcast(&TaskCond); // tells that a task is ongoing
    pthread_mutex_unlock(&lockPrint);

    return 0;
}

int main(int argc, char const *argv[])
{

    call_library_functions();

    init_thread_pool();

    pthread_create(&admin_thread, NULL, handle_threads, NULL); // creates the thread that handles all the threads.

    if (argc > 1)
    {

        // reads the data//
        std::string data;

        for (std::string line; std::getline(std::cin, line);) {
            data+=line;
        }

        // split the data into tasks of 1024 bytes//
        int data_size = data.size();
        // printf("DATA SIZE : %d \n", data_size);
        int pos = 0; // 0 -> 1024 , 1024 -> 2048

        while ((data_size - 1024) > 0 && pos + 1024 < data.size())
        {

            /*building new task to add*/
            struct Task *new_task = (struct Task *)malloc(sizeof(struct Task));
            char dataArray[1024];
            memset(dataArray, '\0', 1024);
            std::strncpy(dataArray, data.substr(pos, 1024).c_str(), 1024);
            memset(new_task->tape, '\0', 1024);
            strcpy(new_task->tape, dataArray);
            strcpy(new_task->type, argv[2]);
            new_task->key = atoi(argv[1]);
            new_task->finished = false;
            new_task->index = active_tasks;
            task_pool.push(*new_task);

            pos += 1024;
            data_size -= 1024;

            ++active_tasks;

            pthread_cond_broadcast(&TaskCond); // when done with tasks, init the threads.
        }
        // edge case for when we get scenario that data is not splitted by 1024 //
        if (data_size > 0)
        {

            /*building new task to add*/
            struct Task *new_task = (struct Task *)malloc(sizeof(struct Task));
            char dataArray2[1024];
            memset(dataArray2, '\0', 1024);
            std::strncpy(dataArray2, data.substr(pos, data_size).c_str(), data_size);
            memset(new_task->tape, '\0', data_size);
            strcpy(new_task->tape, dataArray2);
            strcpy(new_task->type, argv[2]);
            new_task->key = atoi(argv[1]);
            new_task->finished = false;
            new_task->index = active_tasks;
            task_pool.push(*new_task);
            ++active_tasks;
            sum_of_tasks = active_tasks;

            pthread_cond_broadcast(&TaskCond); // when done with tasks, init the threads.
        }
    }
    while (true)
    {
        if (sum_of_tasks == task_number)
            break;
    }
    return 0;
}
