// C++ program to demonstrate creating processes using fork()
#include <unistd.h>
#include <stdio.h>

int main()
{
	// Creating first child
	int n1 = fork();


	if (n1 != 0) {
		printf("parent\n");
		printf(" my id is %d \n", getpid());
        execl("/usr/bin/vi", "vi", "./open_vim.cpp".(char*)0);
	}
	else
	{
		printf("First child\n");
		printf("my id is %d \n", getpid());
	}

	return 0;
}
