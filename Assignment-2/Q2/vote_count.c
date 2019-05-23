#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <stdbool.h>

int wins[20];
int total = 0,total_div50 = 0;

pthread_mutex_t id_mutex, count_mutex;
pthread_cond_t id_condvar;

//Task for each thread
void *count_file(void *id){
	//Set an ID for each thread
	//Require a conditional variable so that each thread is passed a unique ID
	int thread_id = *(int*)id;
	pthread_cond_signal(&id_condvar);

	//Open the input file
	FILE *file;
	char filename[10];
	sprintf(filename, "file%d.txt", thread_id);
	file = fopen(filename, "r");
	
	char line[256];
	bool new_district = true;
	int party,seats,district;

	//Read each line and increase wins
	while (fgets(line, sizeof(line), file)) {
        // note that fgets doesn't strip the terminating \n
		//If line indicates a new district
        if (new_district){
			sscanf(line,"%d", &district);
			new_district = false;
		}
		// if line is "\n"
		else if (strcmp(line, "\n") == 0){
			new_district = true;
		}
		else{
			sscanf(line,"%d %d", &party, &seats);
			//Lock this section for updating the shared variables and displaying counts
			pthread_mutex_lock(&count_mutex);
			wins[party-1] += seats;
			total += seats;
			if(total/50 > total_div50){
				total_div50 = total/50;
				for (int j=0;j<20;j++){
					printf("%d ",wins[j]);
				}
				printf("\n");
			}
			pthread_mutex_unlock(&count_mutex);
		}
	}

    fclose(file);
	pthread_exit(NULL);
}

int main()
{
	//Initialize thread parameters, mutex, condvar
	int i = 0;
	pthread_t threads[10];
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
	pthread_mutex_init(&id_mutex,NULL);
	pthread_cond_init(&id_condvar,NULL);
	pthread_mutex_init(&count_mutex,NULL);
	
	//Create threads
	for(i=1; i<=10;){
		pthread_create(&threads[i-1],&attr,count_file,&i);
		pthread_cond_wait(&id_condvar,&id_mutex);
		i++;
	}
	//Wait for threads to complete
	for(i=1; i<=10; i++){
		pthread_join(threads[i-1],NULL);
	}
	//Show final votes
	for (int j=0;j<20;j++){
		printf("%d ",wins[j]);
	}
	printf("\n");

	//Clean up
	pthread_attr_destroy(&attr);
	pthread_mutex_destroy(&id_mutex);
	pthread_mutex_destroy(&count_mutex);
	pthread_cond_destroy(&id_condvar);
	pthread_exit(NULL);
}

