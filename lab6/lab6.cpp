#include <iostream>
#include <string>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>

using namespace std;

int globalVariable = 2;

void Main()
{
	string sIdentifier;
	int iStackVariable = 20;

	pid_t pID = fork();

		if (pID == 0)
			{
				sIdentifier = "Child Process: ";
				globalVariable++;
				iStackVariable++;
				// sleep(5);
			}
		else if (pID < 0)
			{
				cerr << "Failed to fork" << endl;
				exit(1);
			}
		else
			{
				sIdentifier = "Parent Procces: ";
				// sleep(5);

				execl("/bin/bash", "/bin/bash", "-c", "sleep 60 &", (char *)0);
			}

		cout << sIdentifier;
		cout << "\n    Global variable: " << globalVariable << "\n";
		cout << "    Stack variable: " << iStackVariable << "\n";
		cout << "Child Process PID: " << getpid() << "\n";
		cout << "Parent Process PID: " << getppid() << "\n";
	}

int main()
{
    Main();
    return 0;
}
