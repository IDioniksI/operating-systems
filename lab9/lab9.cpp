#include <iostream>
#include <fstream>
#include <vector>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/wait.h>
#include <unistd.h>
#include <semaphore.h>
#include <fcntl.h>
#include <cstring>
#include <cstdlib>
#include <ctime>

#define MSG_KEY 1234
#define MSG_SIZE 256

struct mymsgbuf {
    long mtype;
    int index;
    int process_id;
    char mtext[MSG_SIZE];
};

void read_poem(const std::string &filename, std::vector<std::string> &lines) {
    std::ifstream file(filename);
    std::string line;
    while (std::getline(file, line)) {
        lines.push_back(line);
    }
    file.close();
}

void child_process(int id, int msgid, sem_t *sem, const std::vector<std::string> &lines) {
    int start_line = id - 1;
    for (int i = start_line; i < lines.size(); i += 4) {
        sem_wait(sem);
        mymsgbuf msg;
        msg.mtype = 1;
        msg.index = i;
        msg.process_id = id;
        std::string text = "Process " + std::to_string(id) + ": " + lines[i];
        strncpy(msg.mtext, text.c_str(), MSG_SIZE);
        msgsnd(msgid, &msg, sizeof(msg), 0);
        sem_post(sem);
        usleep((rand() % 3 + 1) * 100000);
    }
    exit(0);
}

int main() {
    std::vector<std::string> lines;
    read_poem("poem.txt", lines);

    int msgid = msgget(MSG_KEY, IPC_CREAT | 0666);
    sem_t *sem = sem_open("/sem", O_CREAT, 0644, 1);

    srand(time(0));

    for (int i = 1; i <= 4; ++i) {
        if (fork() == 0) {
            child_process(i, msgid, sem, lines);
        }
    }

    int total_lines = lines.size();
    std::vector<std::string> poem(total_lines);

    for (int i = 0; i < total_lines; ++i) {
        mymsgbuf msg;
        msgrcv(msgid, &msg, sizeof(msg), 0, 0);
        poem[msg.index] = msg.mtext;
    }

    for (const auto &line : poem) {
        std::cout << line << std::endl;
    }

    for (int i = 0; i < 4; ++i) {
        wait(NULL);
    }

    msgctl(msgid, IPC_RMID, NULL);
    sem_close(sem);
    sem_unlink("/sem");

    return 0;
}
