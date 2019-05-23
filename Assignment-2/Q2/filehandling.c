#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <stdbool.h>

int main()
{
	int thread_id = 1;
	FILE *file;
	char filename[9];
	sprintf(filename, "file%d.txt", thread_id);
	file = fopen(filename, "r");
	printf("Opened file%d\n", thread_id);
	

	char line[256];
	bool new_district = true;
	int party,seats,district;
	
	while (fgets(line, sizeof(line), file)) {
        // note that fgets doesn't strip the terminating \n
        printf("\n%s", line); 

        if (new_district){
        	printf("new_district is true\n");
			sscanf(line,"%d", &district);
			printf("district number- %d", district);
			new_district = false;
		}
		// if line is "\n"
		else if (strcmp(line, "\n") == 0){
			printf("new_district set to false"); 
			new_district = true;
		}
		else{
			sscanf(line,"%d %d", &party, &seats);
			printf("Party- %d  Seats- %d\n", party, seats);
			// pthread_mutex_lock(&count_mutex);
			// wins[party] += seats;
			// total += seats;
			// pthread_mutex_unlock(&count_mutex);
			// if(total%50 > total_mod50){
			// 	for (int j=0;j<20;j++){
			// 		printf("%d ",wins[j]);
			// 	}
			// 	printf("\n");
			// 	total_mod50 = total%50;
			// }
		}
	}

    fclose(file);

}
