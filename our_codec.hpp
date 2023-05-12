/**
 * Authors: 
 * Lior Nagar
 * Shaked Levi
*/

typedef struct Task
{
    char tape[1024]; // the string the task holds.
    bool finished;   // is the task finished doing the ecnrpyt/decrpyt
    char type[2];    // encode -> '-e' | decode -> '-d' //
    int index;       // n.o of the task in the pool
    int key;

} Task;

/**
 * Init thread pool method
 */
void init_thread_pool();

/**
 * This is taken from stackoverflow:
 * https://stackoverflow.com/questions/17081131/how-can-a-shared-library-so-call-a-function-that-is-implemented-in-its-loader
 * i didnt know how to call the functions by its own, because it didnt work.
 * so google helped.
 */
int call_library_functions();


/**
 * Main thread handler, is used by admin_thread which assings the threads on diffrent tasks.
 */
void *handle_threads(void *arg);

/**
 * This method handles the task and simply encrpyt/decrpyts and prints the data.
 */
void *handle_task(void *arg);