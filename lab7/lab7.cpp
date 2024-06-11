#include <iostream>
#include <pthread.h>
#include <unistd.h>

void cleanupMessage(void *arg) {
    std::cout << "!!! Child thread is being canceled !!!" << std::endl;
}

void *createThread(void *arg) {
    pthread_cleanup_push(cleanupMessage, NULL);

    int count = 0;
    while (true) {
        std::cout << "Child Thread: " << count++ << std::endl;
        sleep(1);
    }

    pthread_cleanup_pop(0);
    pthread_exit(NULL);
}

int main() {
    pthread_t childThread;

    if (pthread_create(&childThread, NULL, createThread, NULL) != 0) {
        std::cerr << "Error creating thread" << std::endl;
        return 1;
    }

    sleep(2);

    if (pthread_cancel(childThread) != 0) {
        std::cerr << "Error canceling thread" << std::endl;
        return 1;
    }

    std::cout << "Parent Thread: Child thread canceled" << std::endl;

    pthread_join(childThread, NULL);

    return 0;
}
