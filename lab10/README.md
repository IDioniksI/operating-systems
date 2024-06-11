## Work with file descriptors
[The main task was](lab10.cpp): Demonstrate working with file descriptors in a multi-process environment. The basic idea is to create multiple child processes that open files and inherit file descriptors from the parent processes.
- Process 0 opens two files (total number of open files, including standard files, equals five);
- Process 1 opens two files (total number of open files, including standard files, equals five);
- Process 2 opens two files (total number of open files, including standard files, equals five);
- Process 0 spawns process 3, which inherits the table open process files 0;
- Process 1 spawns process 4, which inherits the table open process files 1;
- Process 4 additionally opened two more files.