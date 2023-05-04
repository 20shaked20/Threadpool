/**
 * Main program of our_codec.h
 */
#include "codec.h"
#include "our_codec.hpp" // our header //

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <string.h> 
#include <sys/sysinfo.h> // used in getting the cores//
#include <pthread.h>

#include <fstream>
#include <sstream>
#include <iostream> 
#include <queue> // used in task and thread pools


using namespace std;

typedef struct Task
{

    string tape;   // the string the task holds.
    bool finished; // is the task finished doing the ecnrpyt/decrpyt
    char type;     // encode -> '-e' | decode -> '-d' //
    int index;     // n.o of the task in the pool

} Task;

std::queue<struct Task> task_pool; // seprates the file into chuncks of tasks to encrypt/decrpyt.
std::queue<pthread_t> t_pool;      // queue holding all of our threads.
int active_tasks = 0;              // the current active tasks
int sum_of_tasks = 0;              // how many tasks we added at the start
int task_number = 0;               // number of current task we operate

// lock inits//
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t lock3 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t lockPrint = PTHREAD_MUTEX_INITIALIZER;

pthread_cond_t TaskCond = PTHREAD_COND_INITIALIZER;
pthread_cond_t PrintCond = PTHREAD_COND_INITIALIZER;
pthread_cond_t WaitCond = PTHREAD_COND_INITIALIZER;

pthread_t admin_thread; // the main thread that handles the 'mini' threads that operate on the tasks.

/**
 * Init thread pool method
 */
void init_thread_pool()
{
    int cores = get_nprocs_conf();

    for (int i = 0; i < cores; ++i)
    {

        pthread_t new_thread;
        t_pool.push(new_thread);
    }
}

/**
 * This method handles the task and simply encrpyt/decrpyts and prints the data.
 */
void *handle_task(void *arg)
{
    Task *active_task = (Task *)arg; // import the given task to a Task type

    //TODO: add the encrypt/decrypt on the tape of the task here//

    pthread_mutex_lock(&lockPrint);

    while (task_number < active_task->index) // this loop is used in order to make sure we print accordinly, and not in some messed up order.
    {
        pthread_cond_wait(&WaitCond, &lockPrint);
    }

    cout << "DATA: " << active_task->tape << endl; // prints the data on tape.

    fflush(stdout); //clears stdout, is important to make sure the buffer is cleared, so we dont get some junk.

    task_number++; //increse the task number, is important for the loop above.
    pthread_cond_broadcast(&WaitCond);

    t_pool.push(pthread_self()); //thread is done, and we push it to the pool again so it can pick up new tasks.

    pthread_cond_broadcast(&TaskCond); //tells that a task is ongoing
    pthread_mutex_unlock(&lockPrint);
}

/**
 * Main thread handler, is used by admin_thread which assings the threads on diffrent tasks.
 */
void *handle_threads(void *arg)
{

    while (true)
    {
        pthread_mutex_lock(&lock3);
        pthread_mutex_lock(&lock);

        while (task_pool.empty() || t_pool.empty())
        { // simply wait untill there are tasks in the pool and start working.
            pthread_cond_wait(&TaskCond, &lock);
        }

        // take a task to work from the pool//
        Task *active_task = (struct Task *)malloc(sizeof(struct Task));
        *active_task = task_pool.front();
        task_pool.pop();

        // init a thread from the pool on the taken task//
        pthread_create(&t_pool.front(), NULL, handle_task, (void *)active_task);
        t_pool.pop(); // removes the thread from the pool (we add it later, in order for it not to be used until finished)

        pthread_mutex_unlock(&lock3);
        pthread_mutex_unlock(&lock);
    }
}

int main(int argc, char const *argv[])
{
    /* code */

    init_thread_pool();

    pthread_create(&admin_thread, NULL, handle_threads, NULL); // creates the thread that handles all the threads.

    if (argc > 1)
    {

        // reads the data//
        std::string data;
        std::ifstream file(argv[2]); // open the input file
        if (file.peek() != EOF)
        {                             // case where its a file //
            std::stringstream buffer; // string stream
            buffer << file.rdbuf();   // read file to the string stream
            data = buffer.str();      // imports the buffer to a normal string.
        }
        else // case when we enter a string manually //
        {
            data = argv[2];
        }
        file.close(); // closing file

        // split the data into tasks of 1024 bytes//
        int data_size = data.size();
        int pos = 0; // 0 -> 1024 , 1024 -> 2048

        while ((data_size - 1024) > 0 && pos + 1024 < data.size())
        {

            /*building new task to add*/
            Task new_task;
            new_task.tape = data.substr(pos, pos + 1024);
            new_task.type = *argv[1];
            new_task.finished = false;
            new_task.index = active_tasks;
            task_pool.push(new_task);

            pos += 1024;
            data_size -= 1024;

            ++active_tasks;

            pthread_cond_broadcast(&TaskCond); // when done with tasks, init the threads.
        }
        // edge case for when we get scenario that data is not splitted by 1024 //
        if (data_size > 0)
        {

            /*building new task to add*/
            Task new_task;
            new_task.tape = data.substr(pos, pos + data_size);
            new_task.type = *argv[1];
            new_task.finished = false;
            new_task.index = active_tasks;
            task_pool.push(new_task);

            ++active_tasks;
            sum_of_tasks = active_tasks;

            pthread_cond_broadcast(&TaskCond); // when done with tasks, init the threads.
        }
    }

    /*this loop tells the program to stop when all active tasks are done, its a plaster fix, but this is how we find it how to work.*/
    while (1)
    {
        if (active_tasks == task_number)
        {
            break;
        }
    }
    return 0;
}
