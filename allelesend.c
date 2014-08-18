/* allelesend.c
 * Justin Jiang
 * parallel processing of allele combinations
 */
 
 #include <stdio.h>
 #include <stdlib.h>
 #include <time.h>
 #include <mpi.h>
 
 int main(int argc, char **argv)
 {
 	int my_id, root_process = 0, ierr, num_procs;
 	MPI_Status status;
 	
  	int a_size1, a_size2, *numbers1, *numbers2, offset1, offset2, proc, num_to_send1, 			num_to_send2, *localnums1, *localnums2;
 	char *localresult1, *localresult2, localresult3, *globalresult;
 	double start, end;	
 	
 	ierr = MPI_Init(&argc, &argv);
 	ierr = MPI_Comm_rank(MPI_COMM_WORLD, &my_id);
 	ierr = MPI_Comm_size(MPI_COMM_WORLD, &num_procs);
 	
 	if(my_id == 0)
 	{

 		
 		printf("Enter size of array1:\n");
 		scanf("%d", &a_size1);
 		printf("Enter size of array2:\n");
 		scanf("%d", &a_size2);
 		
 		srand(time(0));
 		
 		numbers1 = malloc(sizeof(int) * a_size1);
 		numbers2 = malloc(sizeof(int) * a_size2);
 		localnums1 = malloc(sizeof(int) * a_size1 / (num_procs - 1) + a_size1 % (num_procs - 				1));
 		localnums2 = malloc(sizeof(int) * a_size2 / (num_procs - 1) + a_size2 % (num_procs - 				1));
 		localresult1 = malloc(sizeof(char) * 8 * a_size1 * a_size1 / (num_procs - 1) + 				a_size1 * a_size1 % (num_procs -1);
 		localresult2 = malloc(sizeof(char) * 8 * a_size1 * a_size2 / (num_procs - 1) + 				a_size1 * a_size2 % (num_procs -1);
 		localresult3 = malloc(sizeof(char) * 8 * a_size2 * a_size2 / (num_procs - 1) + 				a_size2 * a_size2 % (num_procs - 1);
 		
 		for(int i = 0; i < a_size1; i++)
 			numbers1[i] = rand() % 50 + 1;
 		for(int j = 0; j < a_size2; j++)
 			numbers2[j] = rand() % 50 + 1;
 		
 		start = MPI_Wtime();
 		
 		for(proc = 1; proc < num_procs; proc++)
 		{
 			if(proc < num_procs -1)
 			{
 				num_to_send1 = a_size1 / (num_procs - 1);
 				num_to_send2 = a_size2 / (num_procs - 1);
 			}
 			else
 			{
 				num_to_send1 = a_size1 / (num_procs - 1) + a_size1 % (num_procs - 1);
 				num_to_send2 = a_size2 / (num_procs - 1) + a_size2 % (num_procs - 1);
 			}
 			
 			ierr = MPI_Send(&num_to_send1, 1, MPI_INT, proc, 1, MPI_COMM_WORLD);
 			ierr = MPI_Send(&num_to_send2, 1, MPI_INT, proc, 1, MPI_COMM_WORLD);
 			
 			ierr = MPI_Send(&numbers1[offset1], num_to_send1, MPI_INT, proc, 1, 				MPI_COMM_WORLD);
 			ierr = MPI_Send(&numbers2[offset2], num_to_send2, MPI_INT, proc, 1, 				MPI_COMM_WORLD);
 			offset1 += num_to_send1;
 			offset2 += num_to_send2;
 		}
 		
 		for(proc = 1; proc < num_procs; proc++)
 		{
 			ierr = MPI_Recv(localresult1, 1, MPI_CHAR, proc, 1, MPI_COMM_WORLD, &status);
 			ierr = MPI_Recv(localresult2, 1, MPI_CHAR, proc, 1, MPI_COMM_WORLD, &status);
 			ierr = MPI_Recv(localresult3, 1, MPI_Char, proc, 1, MPI_COMM_WORLD, &status);
 			
 			printf("%s\n", localresult1);
 			printf("%s\n", localresult2);
 			printf("%s\n", localresult3);
 		}
 		
 		end = MPI_Wtime();
 		
 		printf("\n");
 		printf("Elapsed time: %f\n", end - start);
 		
 		free(numbers1);
 		free(numbers2);
 		free(localresult1);
 		free(localresult2);
 		free(localresult3);
 	}
 	
 	else	//Slave process
 	{
 		int a_size1, a_size2;
 		
 		ierr = MPI_Recv(&a_size1, 1, MPI_INT, root_process, 0, MPI_COMM_WORLD, &status);
 		ierr = MPI_Recv(&a_size2, 1, MPI_INT, root_process, 0, MPI_COMM_WORLD, &status);
 		ierr = MPI_Recv(localnums1, a_size1, MPI_INT, root_process, 1, MPI_COMM_WORLD, 				&status);
 		ierr = MPI_Recv(localnums2, a_size2, MPI_INT, root_process, 1, MPI_COMM_WORLD, 				&status);
 		for(int i= 0; i < a_size1; i++)
 		{
 			for int j = 0, j < a_size2; j++)
 			{
 				//localresult1 gets a string?
 				//localresult2 gets a string?
 				//localresult3 gets a string?
 			}
 		}
 		
 		MPI_Send(localresult1, 1, MPI_CHAR, root_process, 0, MPI_COMM_WORLD);
 		MPI_Send(localresult2, 1, MPI_CHAR, root_process, 0, MPI_COMM_WORLD);
 		MPI_Send(localresult3, 1, MPI_CHAR, root_process, 0, MPI_COMM_WORLD);
 		
 		free(localnums1);
 		free(localnums2);
 	}
 	MPI_Finalize();
 	
 	return 0;
 }
