#include <iostream>
#include <unistd.h>
#include <fcntl.h>
#include <vector>
#include <string>
#include <sys/wait.h>
#include <cstdio>
#include <cstring>

std::vector<int> open_files(const std::vector<std::string>& filenames) {
    std::vector<int> fds;
    for (const auto& filename : filenames) {
        int fd = open(filename.c_str(), O_CREAT | O_WRONLY, 0644);
        if (fd == -1) {
            std::cerr << "Error opening file " << filename << std::endl;
        } else {
            fds.push_back(fd);
            std::cout << "Process " << getpid() << " opened file " << filename << " with fd " << fd << std::endl;
        }
    }
    return fds;
}

void print_fds(int pid) {
    std::cout << "Open files for process " << pid << ":\n";
    std::string command = "ls -l /proc/" + std::to_string(pid) + "/fd";
    FILE* fp = popen(command.c_str(), "r");
    if (!fp) {
        std::cerr << "Error opening pipe for command: " << command << std::endl;
        return;
    }
    char buffer[1024];
    while (fgets(buffer, sizeof(buffer), fp)) {
        if (strstr(buffer, "pipe") == nullptr) {
            std::cout << buffer;
        }
    }
    pclose(fp);
}

int main() {
    std::vector<std::string> filenames = {"/tmp/file1", "/tmp/file2", "/tmp/file3", "/tmp/file4", "/tmp/file5"};

    pid_t pid0 = fork();
    if (pid0 == 0) {
        std::cout << "Process " << getpid() << " (Process 0) is opening files.\n";
        auto fds0 = open_files({filenames[0], filenames[1]});
        print_fds(getpid());

        pid_t pid3 = fork();
        if (pid3 == 0) {
            std::cout << "Process " << getpid() << " (Process 3) inherited files from Process 0.\n";
            print_fds(getpid());
            return 0;
        }
        waitpid(pid3, nullptr, 0);
        return 0;
    }

    waitpid(pid0, nullptr, 0);

    pid_t pid1 = fork();
    if (pid1 == 0) {
        std::cout << "Process " << getpid() << " (Process 1) is opening files.\n";
        auto fds1 = open_files({filenames[2], filenames[3]});
        print_fds(getpid());

        pid_t pid4 = fork();
        if (pid4 == 0) {
            std::cout << "Process " << getpid() << " (Process 4) inherited files from Process 1.\n";
            print_fds(getpid());

            std::cout << "Process " << getpid() << " (Process 4) is opening additional files.\n";
            auto new_fds = open_files({filenames[1], filenames[2]});
            print_fds(getpid());
            return 0;
        }
        waitpid(pid4, nullptr, 0);
        return 0;
    }

    waitpid(pid1, nullptr, 0);

    pid_t pid2 = fork();
    if (pid2 == 0) {
        std::cout << "Process " << getpid() << " (Process 2) is opening files.\n";
        auto fds2 = open_files({filenames[4], filenames[0]});
        print_fds(getpid());
        return 0;
    }

    waitpid(pid2, nullptr, 0);

    return 0;
}
