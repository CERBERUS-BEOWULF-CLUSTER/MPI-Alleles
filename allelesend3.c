/* allelesend3.c
 * Justin Jiang
 * parallel processing of allele combinations
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>
#include <string.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/types.h>
#include <errno.h>
#include <getopt.h>

#define SIZE 2000
#define True 1
#define False 0
#define MYPORT "1337"
#define IP_ADDRESS "127.0.0.1"
#define INET_ADDRSTLEN 10 

#define DEFAULT_SIZE 512
#define DEFAULT_SIZE_SMALL 4
#define FILENAME_LENGTH 20
#define EXTENSION_LENGTH 5
#define EXTENSION ".txt"
 
int readsample(FILE *file, char **sample, int *numberofelements);
void writesample(char **sample);

void reallocatestringarray(char ***array, int *capacity);
void reallocatememory(char **sample, int *capacity);

int generatesamples(int argc, char* argv[]);
 
int main(int argc, char **argv)
{
	int my_id, root_pid = 0, ierr, num_procs;
	MPI_Status status;

	int sample_size, proc, current_capacity;
	char *sample, *filename;
	FILE *file;
	double buildstart, start, end;

	ierr = MPI_Init(&argc, &argv);
	ierr = MPI_Comm_rank(MPI_COMM_WORLD, &my_id);
	ierr = MPI_Comm_size(MPI_COMM_WORLD, &num_procs);

	if(my_id == 0)	//Master process
	{
		int maxsamples = atoi(argv[1]);
		float parts = 10.0;
		float interval = maxsamples / parts;
		
		buildstart = MPI_Wtime();	//start time of random sample generation
		
		generatesamples(argc, argv);
	
		int socket_fd, rv;
		struct addrinfo hints, *res, *p;

		memset(&hints, 0, sizeof(hints));

		hints.ai_socktype = SOCK_DGRAM;
		hints.ai_family = AF_INET;
		
		char ip[INET_ADDRSTLEN] = IP_ADDRESS;
        
		char port[5] = MYPORT;
		struct sockaddr_inv;
        
		memset(ip, 0, INET_ADDRSTRLEN);
		memset(port, 0, 5);
   
		rv = getaddrinfo("127.0.0.1", MYPORT, &hints, &res);
		if (rv != 0) {
    		fprintf(stderr, "Error [getaddrinfo]\n");
   	 		return 2;
		}

		for (p = res; p != NULL; p = p->ai_next) {
    		if ((socket_fd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
        		fprintf(stderr, "Error [socket]\n");
        		continue;
			}
			break;
		}

		freeaddrinfo(res);

		if (p == NULL) {
    		fprintf(stderr, "Error [socket allocation]\n");
        	return -2;
		}
		
		char *message = argv[1];
		
		int len = strlen(message);	
		int bytesSent = sendto(socket_fd, message, len > 1024 ? 1024 : len, 0, p->ai_addr, p->ai_addrlen);
        if (bytesSent == -1) {
               	fprintf(stderr, "Error [socket send]\n");
               	//return -2;
		}
 	
 		int slave_id;
 		
 		filename = "samples.txt";
 		file = fopen(filename, "r");
 		
 		start = MPI_Wtime();
 		proc = 1;
 		while(proc < num_procs)
 		{
 			if (readsample(file, &sample, &sample_size)){
 				ierr = MPI_Send(&sample_size, 1, MPI_INT, proc, 0, MPI_COMM_WORLD);
 				ierr = MPI_Send(sample, sample_size, MPI_CHAR, proc, 0, MPI_COMM_WORLD);
 				free(sample);
 			}
 			proc++;
 		}
 		
 		int samplecounter = 0;
 		int sendcounter = 1;
 		
 		while(readsample(file, &sample, &sample_size) != 0)
 		{
 			ierr = MPI_Recv(&slave_id, 1, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);
 			proc = slave_id;
 			
 			samplecounter++;	
 			if (samplecounter == (int)(sendcounter * interval)) {
 				 int len = strlen(message);			
        		int bytesSent = sendto(socket_fd, message, len > 1024 ? 1024 : len, 0, p->ai_addr, p->ai_addrlen);
        		if (bytesSent == -1) {
                	fprintf(stderr, "Error [socket send]\n");
                	//return -2;
				}
				sendcounter++;
 			}

 			ierr = MPI_Send(&sample_size, 1, MPI_INT, proc, 0, MPI_COMM_WORLD);
 			ierr = MPI_Send(sample, sample_size, MPI_CHAR, proc, 0, MPI_COMM_WORLD);
 			free(sample);
 		}
 		
 		// send message to all slaves that there are no more samples to be calculated
 		sample_size = 0;
 		for (proc = 1; proc < num_procs; proc++) {
 			ierr = MPI_Send(&sample_size, 1, MPI_INT, proc, 0, MPI_COMM_WORLD);
 		}
 		
 		for (sendcounter; sendcounter <= parts; sendcounter++) {
 			len = strlen(message);			
			bytesSent = sendto(socket_fd, message, len > 1024 ? 1024 : len, 0, p->ai_addr, p->ai_addrlen);
			if (bytesSent == -1) {
				fprintf(stderr, "Error [socket send]\n");
				//return -2;
			}
		}
 			
 		end = MPI_Wtime();
 		fclose(file);
 		printf("Finished building in %f seconds.\n", start-buildstart);
 		printf("Finished computing in %f seconds.\n", end-start);
 		printf("Total time elapsed: %f seconds.\n", end-buildstart);
 	}
 	
 	else	//Slave processes
 	{
 		int ld_size, i, j ,k, num_alleles;
 		char *local_data;
 		char *token, *result, **strings;

		int rcap = DEFAULT_SIZE * 4;
		int selemcap = sizeof(char) * 8;
		int scap = sizeof(char) * DEFAULT_SIZE_SMALL * selemcap;
		
 		//token = (char *)malloc(tcap);
		
 		while (1)
 		{
 			ierr = MPI_Recv(&ld_size, 1, MPI_INT, root_pid, 0, MPI_COMM_WORLD, &status);
 			if (ld_size == 0) {
 				break;
 			}
 			else if (ld_size > 0)
 			{
 				local_data = (char *)malloc(sizeof(char) * (ld_size));
 				ierr = MPI_Recv(local_data, ld_size, MPI_CHAR, root_pid, 0, MPI_COMM_WORLD, &status);

 				result = (char *)calloc(rcap, sizeof(char));
				int resultsize = 0;
 				strings = (char **)malloc(scap);

 				token = strtok(local_data, "\n");

				// added memory reallocation
				resultsize += strlen(token);
				if (resultsize > rcap) {
					reallocatememory(&result, &rcap);
				}

 				strcat(result, token);
 		 		while((token = strtok(NULL, "\n")) != NULL)
 				{

 					if((strrchr(token, '{') != NULL) || (strrchr(token, '[') != NULL) || (strrchr(token, '}') != NULL))
 					{
						// added memory reallocation
						resultsize += strlen(token);
						if (resultsize > rcap) {
							reallocatememory(&result, &rcap);
						}

 						strcat(result, token);
 					}
 					else
 					{
 						num_alleles = k = 0;
 						strings[k++] = token;
 						num_alleles = k;
 					
 						while((token = strtok(NULL, "\n")) != NULL)
 						{
 							if((strrchr(token, ']')) == NULL)
 							{
								if (num_alleles + 1 > scap / selemcap) {
									reallocatestringarray(&strings, &scap);
								}
 								strings[k++] = token;
 								num_alleles = k;
 							}
 							else
 							{
 								break;
 							}
 						}
 						for(i = 0; i < num_alleles; i++)
 						{
 							for(j = 0; j < num_alleles; j++)
 							{
								// added memory reallocation
								resultsize += 3 + strlen(strings[i]) + strlen(strings[j]);
								if (resultsize + 1 > rcap) { // check for one space extra for potential following ']'
									reallocatememory(&result, &rcap);
								}

 								strcat(result, "(");
 								strcat(result, strings[i]);
 								strcat(result, ",");
 								strcat(result, strings[j]);
 								strcat(result, ")");
 							}
 						}
						// added memory reallocation
 						strcat(result, "]");
						resultsize++;
 					}
 					
 				}
 			if (argc > 2) {
 				if (strcmp(argv[2], "-x") != 0) {
 				writesample(&result);
 				}
 			}
 			
 			free(result);
 			free(local_data);
 			free(strings); // free up 2d memory, each string does not need to be freed
 			//free(token);
 			
 			ierr = MPI_Send(&my_id, 1, MPI_INT, root_pid, 0, MPI_COMM_WORLD);
 			}
 		}
 	}
 	ierr = MPI_Finalize();
 	return 0;
}

/*
 * @author Julian Kuk
 *
 * function for reading the samples.txt, one sample at a time
 * each sample will be turned into a 1D array of characters, that must be
 * later tokenized in order to be used.
 *
 * there is no return value, because variables are passed to the function,
 * and the function will modify them to have the proper values.
 * 
 * the calling function must fopen a FILE
 * and then that FILE must be passed as a parameter,
 * and then once the calling function has finished,
 * it must fclose the FILE
 */
 
int readsample(FILE *file, char **sample, int *numberofelements) { // ex: readsample(file, &sample, &numberofelements);
	char buffer[1]; // the buffer to read the characters into
	
	int capacity = sizeof(char) * DEFAULT_SIZE; // the capacity of the array
	
	*sample = (char *)malloc(capacity); // allocate memory
	*numberofelements = 0; // initialize the number of elements

	int i = 0; // i is the index of the sample character array
	while (1) {
		if (fread(buffer, 1, sizeof(char), file) == 0) { // if you reach the end of the file
			return 0; // return 0
		}
		else {
			if (i * sizeof(char) >= capacity) { // if the capacity is too small
				reallocatememory(sample, &capacity); // reallocate memory
			}
			sample[0][i++] = buffer[0]; // add the character in buffer to the sample character array
			*numberofelements += 1; // increment the number of elements
			if (buffer[0] == '}') { // if you reach the end of the sample
				sample[0][i] = 0; // terminate the string with NULL
				*numberofelements += 1; // increment the number of elements
				return 1; // return 1
			}
		}
	}
}

/*
 * @author Julian Kuk
 *
 * write a sample to a text file
 * the sample must be written as a 1D array of characters
 * and the address of that array must be passed as an argument.
 *
 * the text file will be named after the sample's name.
 */

void writesample(char **sample) { // ex: writesample(&sample);
	int i = 0; // index for traversing array
	char filename[FILENAME_LENGTH], extension[EXTENSION_LENGTH] = EXTENSION; // space for filename string, and ".txt" string to be used as extension

	while (sample[0][i] != '{') { // loop through until you reach '{'
		filename[i++] = sample[0][i]; // copy the sample name, one character at a time
	}
	filename[i] = '\0'; // NULL terminate the string
	strcat(filename, extension); // concatenate the sample name with ".txt"

	FILE *file = fopen(filename, "w"); // open the file for (re)writing
	i = 0; // reset index
	char buffer[1]; // buffer to read the characters into
	while (sample[0][i] != '\0') { // while the end of the sample hasn't been reached
		buffer[0] = sample[0][i++]; // store the character in the buffer
		fwrite(buffer, 1, sizeof(buffer), file); // then write the character to the file
		fflush(file);
	}
	fclose(file); // close the file
}

/*
 * @author Julian Kuk
 *
 * utility function for reallocating 2d memory
 * simply doubles the number of sub arrays, however
 * the calling function must keep track of the current capacity of the actual array
 *
 * note: each sub array will have the same capacity as it did before
 */

void reallocatestringarray(char ***array, int *capacity) { // ex: reallocatememory(&array, &capacity, elementcapacity);
	*capacity *= 2; // double the capacity
	array[0] = realloc(array[0], *capacity); // reallocate the memory with the new capacity
}

/*
 * @author Julian Kuk
 *
 * utility function for reallocating memory
 * simply doubles the capacity, however
 * the calling function must keep track of the current capacity of the actual array
 */

void reallocatememory(char **array, int *capacity) { // ex: reallocatememory(&array, &capacity);
	*capacity *= 2; // double the capacity
	array[0] = realloc(array[0], *capacity); // reallocate the memory with the new capacity
}


int generatesamples(int argc, char* argv[]) {
    int numsamples = atoi(argv[1]);
    //int numsamples = atoi("1000");
    char* filename = "samples.txt";
    char buffer[4];
    int i;

    FILE *file = fopen(filename, "w");
    srand(time(0));

    for(i = 0; i < numsamples; i++) {
        fwrite("sample", 1, strlen("sample"), file);
        sprintf(buffer, "%d", i);
        fwrite(buffer, 1, strlen(buffer), file);
        fwrite("{", 1, sizeof(char), file);
        fwrite("\n", 1, sizeof(char), file);
        int j;
        for(j = 0; j < 15; j++) {
            fwrite("locus", 1, strlen("locus"), file);
            sprintf(buffer, "%d", j);
            fwrite(buffer, 1, strlen(buffer), file);
            fwrite("[", 1, sizeof(char), file);
            fwrite("\n", 1, sizeof(char), file);
            int k;
            int l = rand()%7+1;
            for(k = 0; k < l; k++) {
                int a = rand()%30+1;
                sprintf(buffer, "%d", a);
                fwrite(buffer, 1, strlen(buffer), file);
                fwrite("\n", 1, sizeof(char), file);
            }
            fwrite("]", 1, sizeof(char), file);
            fwrite("\n", 1, sizeof(char), file);
        }
        fwrite("}", 1, sizeof(char), file);
        fwrite("\n", 1, sizeof(char), file);
    }
    fclose(file);
}
