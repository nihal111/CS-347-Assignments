#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ioctl.h>

typedef int bool;
#define true 1
#define false 0

int main()
{
	int n;
	printf("%s", "Enter the value of n - ");
	scanf("%d", &n);
	printf("Primes until %d - \n", n);

	int primes[] = {2,3,5,7,11};

	if (n < 12) {
		for (int x = 0; primes[x], primes[x] <= n; x++) {
			printf("%d\n", primes[x]);
		}
		return 0;
	}

	printf("2\n3\n5\n7\n11\n");

	int pipe0[2], pipe1[2];
	pipe(pipe0);
	pipe(pipe1);
	
	pid_t pid0 = fork();
	if (pid0 > 0) {					// parent
		pid_t pid1 = fork();
		if (pid1 > 0) {				// parent
			int current_fork_index = 0;
			close(pipe0[0]);		// close reading ends for parent
			close(pipe1[0]);

			for (int i=2 ; i<n ; i++) {
				if (i%2 && i%3 && i%5 && i%7 && i%11) {
					if (current_fork_index == 0) {
						write(pipe0[1], &i, sizeof(i));
						// printf("Sent %d\n", i);
					} else if (current_fork_index == 1) {
						write(pipe1[1], &i, sizeof(i));
						// printf("Sent %d\n", i);
					}
					current_fork_index = 1 - current_fork_index;
				}
			}
			write(pipe0[1], &n, sizeof(n));
			write(pipe1[1], &n, sizeof(n));
		} else if (pid1 == 0) {		// child 2
			close(pipe1[1]);		// close writing end for child
			int a;
			printf("In Child 2\n");
			do {
				read(pipe1[0], &a, sizeof(a));
				bool factorFound = false;
				for (int i=12 ; i<=a/2 ; i++) {
					if (a%i == 0) {
						factorFound = true;
					}
				}
				if (factorFound == false) {
					printf("%d <- Child 2\n", a);
				}
			} while (a != n);
		}
	} else if (pid0 == 0) {		// child 1
		close(pipe0[1]);		// close writing end for child
		int a;
		printf("In Child 1\n");
		do {
			read(pipe0[0], &a, sizeof(a));
			bool factorFound = false;
			for (int i=12 ; i<=a/2 ; i++) {
				if (a%i == 0) {
					factorFound = true;
				}
			}
			if (factorFound == false) {
				printf("%d <- Child 1\n", a);
			}
		} while (a != n);
	}
	return 0;
}