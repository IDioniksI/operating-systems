#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <vector>
#include <random>
#include <chrono>
#include <ctime>

const int FERRY_CAPACITY = 30;
const int HALF_CAPACITY = FERRY_CAPACITY / 2;

std::mutex mtx;
std::condition_variable cv;
std::queue<std::string> queue;

int airborne_count = 0;
int marine_count = 0;
int total_on_ferry = 0;
int total_created = 0;
int total_in_queue = 0;
int total_processed = 0;
int airborne_q = 0;
int marine_q = 0;

std::random_device rd;
std::mt19937 gen(rd());
std::uniform_int_distribution<> dist(1, 5);

std::string current_time() {
    auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    std::string time_str = std::ctime(&now);
    time_str.pop_back();
    return time_str;
}

void soldier(const std::string& type) {
    std::this_thread::sleep_for(std::chrono::seconds(dist(gen)));
    std::unique_lock<std::mutex> lock(mtx);

    queue.push(type);
    total_in_queue++;
    if (type == "Airborne") {
        airborne_q++;
    } else {
        marine_q++;
    }
    std::cout << current_time() << " " << type << " get in the queue\n";
    std::cout << "Now are in the queue: " << total_in_queue << ", on the ferry: " << total_on_ferry << "\n";

    cv.wait(lock, [&type]() {
        if (total_on_ferry >= FERRY_CAPACITY) {
            return false;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        if (type == "Airborne") {
            if ((airborne_count >= HALF_CAPACITY && total_on_ferry + 1 <= FERRY_CAPACITY) || (marine_count == 0 && total_on_ferry + 1 <= FERRY_CAPACITY && total_on_ferry + 1 <= HALF_CAPACITY) || (marine_count == HALF_CAPACITY && marine_q == 0 && total_on_ferry + 1 <= FERRY_CAPACITY)) {
                return true;
            }
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        } else if (type == "Marine") {
            if ((marine_count >= HALF_CAPACITY && total_on_ferry + 1 <= FERRY_CAPACITY) || (airborne_count == 0 && total_on_ferry + 1 <= FERRY_CAPACITY && total_on_ferry + 1 <= HALF_CAPACITY) || (airborne_count == HALF_CAPACITY && airborne_q == 0 && total_on_ferry + 1 <= FERRY_CAPACITY)) {
                return true;
            }
        }

        return false;
    });

    if (type == "Airborne") {
        airborne_count++;
        airborne_q--;
    } else {
        marine_count++;
        marine_q--;
    }
    total_on_ferry++;
    total_in_queue--;
    std::cout << current_time() << " " << type << " took the ferry\n";
    std::cout << "Now are in the queue: " << total_in_queue << ", on the ferry: " << total_on_ferry << "\n";
    queue.pop();
    total_processed++;

    if (total_on_ferry == FERRY_CAPACITY) {
        std::cout << current_time() << " The ferry departed from " << airborne_count << " airbornes and " << marine_count << " marines\n";
        lock.unlock();
        std::this_thread::sleep_for(std::chrono::seconds(dist(gen)));
        lock.lock();
        std::cout << current_time() << " The ferry has come back\n";
        total_on_ferry = 0;
        airborne_count = 0;
        marine_count = 0;
        std::cout << "Now are in the queue: " << total_in_queue << ", on the ferry: " << total_on_ferry << "\n";
        cv.notify_all();
    } else if (total_on_ferry >= HALF_CAPACITY && airborne_count == marine_count && total_on_ferry + HALF_CAPACITY <= FERRY_CAPACITY) {
        cv.notify_all();
    } else if (total_on_ferry <= HALF_CAPACITY && airborne_count == marine_count && total_in_queue >= HALF_CAPACITY) {
        cv.notify_all();
    } else {
        cv.notify_all();
    }
}

int main() {
    int num_airborne, num_marines;

    std::cout << "Enter the number of airbornes: ";
    std::cin >> num_airborne;
    std::cout << "Enter the number of marines: ";
    std::cin >> num_marines;

    total_created = num_airborne + num_marines;

    std::vector<std::thread> soldiers;

    for (int i = 0; i < num_airborne; ++i) {
        soldiers.emplace_back(soldier, "Airborne");
    }

    for (int i = 0; i < num_marines; ++i) {
        soldiers.emplace_back(soldier, "Marine");
    }

    for (auto& th : soldiers) {
        th.join();
    }

    {
        std::unique_lock<std::mutex> lock(mtx);
        cv.wait(lock, []() {
            return total_in_queue == 0 && total_on_ferry == 0 && total_processed == total_created;
        });
    }

    std::cout << "All military personnel have been transferred.\n";

    return 0;
}
