#include <iostream>
#include <pthread.h>
#include <cstdlib>
#include <cmath>

struct ThreadData {
    int thread_id;
    long num_iterations;
    int num_threads;
    double partial_sum;
};

void* calculate_pi(void* arg) {
    ThreadData* data = static_cast<ThreadData*>(arg);
    int thread_id = data->thread_id;
    long num_iterations = data->num_iterations;
    int num_threads = data->num_threads;
    double partial_sum = 0.0;

    for (long i = thread_id; i < num_iterations; i += num_threads) {
        double term = 1.0 / (2.0 * i + 1.0);
        if (i % 2 == 1) {
            term = -term;
        }
        partial_sum += term;
    }

    data->partial_sum = partial_sum;

    pthread_exit(static_cast<void*>(arg));
}

int main() {
    int num_threads;
    long num_iterations;

    std::cout << "Enter number of threads: ";
    std::cin >> num_threads;
    if (num_threads <= 0 || std::cin.fail()) {
        std::cerr << "Number of threads must be a positive integer." << std::endl;
        return 1;
    }

    std::cout << "Enter number of iterations: ";
    std::cin >> num_iterations;
    if (num_iterations <= 0 || std::cin.fail()) {
        std::cerr << "Number of iterations must be a positive integer." << std::endl;
        return 1;
    }

    pthread_t* threads = new pthread_t[num_threads];
    ThreadData* thread_data = new ThreadData[num_threads];

    for (int i = 0; i < num_threads; ++i) {
        thread_data[i].thread_id = i;
        thread_data[i].num_iterations = num_iterations;
        thread_data[i].num_threads = num_threads;
        thread_data[i].partial_sum = 0.0;

        if (pthread_create(&threads[i], nullptr, calculate_pi, &thread_data[i]) != 0) {
            std::cerr << "Error creating thread " << i << std::endl;
            return 1;
        }
    }

    double pi = 0.0;
    for (int i = 0; i < num_threads; ++i) {
        void* status;
        if (pthread_join(threads[i], &status) != 0) {
            std::cerr << "Error joining thread " << i << std::endl;
            return 1;
        }
        ThreadData* data = static_cast<ThreadData*>(status);
        pi += data->partial_sum;
    }

    pi *= 4.0;

    std::cout << "Calculated value of pi: " << pi << std::endl;

    delete[] threads;
    delete[] thread_data;

    return 0;
}
