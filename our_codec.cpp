/**
 * Main program of our_codec.h
*/
#include "codec.h"
#include "our_codec.hpp" // our header //
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fstream>
#include <sstream>
#include <iostream>
#include <queue>
#include <string.h>
#include <sys/sysinfo.h>
#include <pthread.h>
#include <dlfcn.h>

using namespace std;

typedef struct Task {

    string tape;
    bool finished;
    char type; // encode - E | decode - D //

} Task;


std::queue<struct Task> task_pool; // seprates the file into chuncks of tasks to encrypt/decrpyt.
std::queue<pthread_t> t_pool; // queue holding all of our threads.
int active_tasks = 0;
int sum_of_tasks = 0;

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t lock2 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t lock3 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t lockWrite = PTHREAD_MUTEX_INITIALIZER;

pthread_cond_t TaskCond = PTHREAD_COND_INITIALIZER;

pthread_t admin_thread;

void init_thread_pool()
{
    int cores = get_nprocs_conf();
    
    for(int i = 0 ; i < cores; ++i){

        pthread_t new_thread;
        t_pool.push(new_thread);

    }
    // cout << "t_pool size: " << t_pool.size() << endl;
}


// void* handle_threads(void *arg)
// {   

//     cout << "bla bla bla 2" << endl;
//     int i = 0;
//     while(i < 1)
//     {   
//         pthread_mutex_lock(&lock3);
//         pthread_mutex_lock(&lock);

//         while(task_pool.empty()){
//             pthread_cond_wait(&TaskCond,&lock);
//         }
        
//         cout << "bla bla bla 3" << endl;

//         pthread_t active_thread = t_pool.front();
//         t_pool.pop();
//         // t_pool.push(active_thread);

//         Task active_task = task_pool.front();
//         task_pool.pop();

//         // cout<< task_pool.size() << " bla bla bla 2" << endl;
//         // pthread_create(&active_thread, NULL, handle_task, &active_task);
//         // cout << "bla bla bla 2" << endl;

//         pthread_mutex_unlock(&lock3);
//         pthread_mutex_unlock(&lock);
//         ++i;

//     }//  pos_thread-> 1,2,3,4,5 | 1,2,3,4,5 ,    6,7,8,9,10
    
//     return 0;
// }

void* handle_task(void *arg)
{
    cout << "ZIBIBIBIBIBB" << endl;
    Task* active_task = (Task*)arg;
    while (true) {
        pthread_mutex_lock(&lock);
        while (task_pool.empty()) {
            pthread_cond_wait(&TaskCond, &lock);
        }
        pthread_mutex_unlock(&lock);
        // if (active_task->finished) {
        //     delete active_task;
        //     break;
        // }
        // do some task with active_task here...
        cout << "blbablabalblabla" << endl;

        pthread_mutex_lock(&lock3);
        t_pool.push(pthread_self());
        pthread_mutex_unlock(&lock3);
    }
    return 0;
}

void* handle_threads(void* arg) {
    cout << "bla 1 " << endl;
    cout << "t_pool size: " << t_pool.size() << endl;
    pthread_t active_thread;
    while (true) {
        pthread_mutex_lock(&lock3);
        if (!t_pool.empty()) {
            active_thread = t_pool.front();
            t_pool.pop();
            pthread_mutex_unlock(&lock3);
            pthread_mutex_lock(&lock);
            if (!task_pool.empty()) {
                Task *active_task = new Task(task_pool.front());
                cout << "DATA OF TAPE : " << active_task->tape << endl;
                task_pool.pop();
                pthread_create(&active_thread, NULL, handle_task, active_task);
            }
            pthread_mutex_unlock(&lock);
        } else {
            pthread_mutex_unlock(&lock3);
        }
    }
    return 0;
}


int main(int argc, char const *argv[])
{
    /* code */

    init_thread_pool();

    pthread_mutex_lock(&lock2);
    pthread_create(&admin_thread, NULL, handle_threads, NULL);
    pthread_mutex_unlock(&lock2);

    pthread_mutex_lock(&lock2);
    if(argc > 1){

        //reads the data//
        std::string data;
        std::ifstream file(argv[2]); // open the input file
        if(file.peek() != EOF){ // case where its a file // 
            std::stringstream buffer; // string stream
            buffer << file.rdbuf(); // read file to the string stream
            data = buffer.str(); // imports the buffer to a normal string.
            // cout << "The data is: " << data << std::endl;
        }
        else // case when we enter a string manually // 
        {
            data = argv[2];
            // std::cout << "The dta is: " << data << std::endl;
        }
        file.close(); // closing file

        ////////////////////

        //split the data into tasks of 1024 bytes//

        int data_size = data.size();
        int pos = 0;

        while((data_size - 1024) > 0){ 
            Task new_task;
            new_task.tape = data.substr(pos,pos+1024);
            new_task.type = *argv[1];
            task_pool.push(new_task);

            pos+= 1024;
            data_size-=1024;

            cout << "Task number: " << active_tasks << " Added to pool" << endl;
            ++active_tasks;

        } 
        // edge case for when we get scenario that data is not splitted by 1024 // 
        Task new_task;
        new_task.tape = data.substr(pos,pos+data_size);
        new_task.type = *argv[1];
        task_pool.push(new_task);

        sum_of_tasks = active_tasks;
        
        cout << "Task number: " << active_tasks << " Added to pool" << endl;

        pthread_cond_broadcast(&TaskCond); // when done with tasks, init the threads.

    }
    pthread_mutex_unlock(&lock2);
    ////////////////////

    printf("Sample code!\n");
    return 0;
}
