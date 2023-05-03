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

void init_thread_pool()
{
    int cores = get_nprocs_conf();
    
    for(int i = 0 ; i < cores; ++i){

        pthread_t new_thread;
        t_pool.push(new_thread);

    }
}

void handle_threads()
{
    while(active_tasks > 0)
    {
            
    }//  pos_thread-> 1,2,3,4,5 | 1,2,3,4,5 ,    6,7,8,9,10

}


int main(int argc, char const *argv[])
{
    /* code */
   
    init_thread_pool();

    
    if(argc > 1){

        //reads the data//
        std::string data;
        std::ifstream file(argv[2]); // open the input file
        if(file.peek() != EOF){ // case where its a file // 
            std::stringstream buffer; // string stream
            buffer << file.rdbuf(); // read file to the string stream
            data = buffer.str(); // imports the buffer to a normal string.
            cout << "The data is: " << data << std::endl;
        }
        else // case when we enter a string manually // 
        {
            data = argv[2];
            std::cout << "The dta is: " << data << std::endl;
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


    }
    ////////////////////

    handle_threads();

    
    printf("Sample code!\n");
    return 0;
}
