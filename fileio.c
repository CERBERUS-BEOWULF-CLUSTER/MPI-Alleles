/* fileio.c
 * Julian Kuk
 * functions for file input and file output
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DEFAULT_SIZE 16 // default size value used for memory allocation of arrays

// test functions
int testreadsamples();
int testwritepossibilities();

// actual functions
void readsamples(char *filename, char ***samplenames, int *numberofsamples, char ****locinames, int **numberofloci, int ****alleles, int ***numberofalleles);
void writepossibilities(char *samplename, char **locinames, int numberofloci, int **possibilities, int *numberofpossibilities);

// utility functions for the reading/writing
int stringtointeger(char *string);
//char *integertostring(int number);

/*
 * main function is here for testing purposes only
 */

int main(int argc, char **argv) {
	if (testreadsamples()) {
		printf("Reading completed successfully.\n\n");
	}
	if (testwritepossibilities()) {
		printf("Writing completed successfully.\n");
		printf("Check the folder for the new file.\n");
	}
	return 0;
}

/*
 * tests the readsamples() function
 * use this example to see how you can actually call the readsamples() function
 */

int testreadsamples() {
	char *filename = "samples.txt"; // filename for the text file that contains the data
	char **samplenames; // 1D array of strings -- contains the names of each sample
	int numberofsamples; // tracks the number of samples
	char ***locinames; // 2D array of strings -- contains the names of each locus for each sample
	int *numberofloci; // 1D array of integers -- tracks the number of loci for each sample
	int ***alleles; // 3D array of integers -- contains each allele of each locus for each sample
	int **numberofalleles; // 2D array of integers -- tracks the number of alleles of each locus for each sample

	/*
	 * pass the pointer of the filename's string directly
	 * pass the address for every other variable
	 */
	readsamples(filename, &samplenames, &numberofsamples, &locinames, &numberofloci, &alleles, &numberofalleles);

	int i, j, k; // variables used for traversing through up to 3D

	printf("NUMBER OF SAMPLES: %d\n", numberofsamples);

	for (i = 0; i < numberofsamples; i++) {
		printf("%s{", samplenames[i]);
		printf(" NUMBER OF LOCI: %d\n", numberofloci[i]);

		for (j = 0; j < numberofloci[i]; j++) {
			printf("\t%s[", locinames[i][j]);
			printf(" NUMBER OF ALLELES: %d\n", numberofalleles[i][j]);

			for (k = 0; k < numberofalleles[i][j]; k++) {
				printf("\t\t%d\n", alleles[i][j][k]);
			}
		}
	}
	return 1; // return 1 if this completes successfully
}

/*
 * tests the wrtiepossibilities() function
 * use this example to see how you can actually call the writepossibilities() function
 */

int testwritepossibilities() {

	// just setting up data for testing
	char *samplename = "test";

	int numberofloci = 3;
	char **locinames = (char **)malloc(sizeof(char) * numberofloci * DEFAULT_SIZE);
	locinames[0] = "locus0";
	locinames[1] = "locus1";
	locinames[2] = "locus2";
	
	int *numberofpossibilities = (int *)malloc(sizeof(int) * numberofloci);
	numberofpossibilities[0] = 4;
	numberofpossibilities[1] = 4;
	numberofpossibilities[2] = 4;
	numberofpossibilities[3] = 4;
	int **possibilities = (int **)malloc(sizeof(int) * numberofloci * DEFAULT_SIZE);

	int i, j, k = 0;
	for (i = 0; i < numberofpossibilities[0]; i++) {
		possibilities[i] = (int *)malloc(sizeof(int) * 4);
	}
	
	for (i = 0; i < numberofloci; i++) {
		for (j = 0; j < 4; j++) {
			possibilities[i][j] = k++;
		}
	}
	// end data set up

	/*
	 * just pass the pointer for all of the variables to the function
	 * keep in mind, this writes for only 1 sample
	 * all of the variables are 1D less than the readsamples() for that reason
	 * presumably, the master would have extracted samples from the array of samples,
	 * and sent the data with 1D less
	 */
	writepossibilities(samplename, locinames, numberofloci, possibilities, numberofpossibilities);

	// free up variable space
	free(locinames);
	free(numberofpossibilities);
	free(possibilities);

	return 1; // return 1 if this completes successfully
}

/*
 * actual function for reading the samples
 * there is no return value, because variables are passed to the function,
 * and the function will modify them to have the proper values.
 * 
 * see test function above for an example
 */
void readsamples(char *filename, char ***samplenames, int *numberofsamples, char ****locinames, int **numberofloci, int ****alleles, int ***numberofalleles) {

	char buffer[2]; // create a string buffer, so it can be used by the fread() function
	buffer[1] = 0; // NULL terminated
	int i, j, k; // variables for traversing up to 3D
	i = j = k = 0;

	// variables used for reallocating memory
	// HAS NOT BEEN IMPLEMENTED YET
	int samplescapacity = DEFAULT_SIZE;
	int locicapacity = DEFAULT_SIZE;
	int allelescapacity = DEFAULT_SIZE;

	// sample related data memory allocation
	*samplenames = (char **)malloc(sizeof(char) * DEFAULT_SIZE * DEFAULT_SIZE);
	*numberofsamples = 0; // set this counter to 0

	// locus related data memory allocation
	*locinames = (char ***)malloc(sizeof(char) * DEFAULT_SIZE * DEFAULT_SIZE * DEFAULT_SIZE);
	for (i = 0; i < DEFAULT_SIZE; i++) {
		locinames[0][i] = (char **)malloc(sizeof(char) * DEFAULT_SIZE * DEFAULT_SIZE);
	}
	*numberofloci = (int *)malloc(sizeof(int) * DEFAULT_SIZE);
	for (i = 0; i < DEFAULT_SIZE; i++) {
		numberofloci[0][i] = 0; // set this counter to 0
	}

	// allele related data memory allocation
	*alleles = (int ***)malloc(sizeof(int) * DEFAULT_SIZE * DEFAULT_SIZE * DEFAULT_SIZE);
	for (i = 0; i < DEFAULT_SIZE; i++) {
		alleles[0][i] = (int **)malloc(sizeof(int) * DEFAULT_SIZE * DEFAULT_SIZE);
		for (j = 0; j < DEFAULT_SIZE; j++) {
			alleles[0][i][j] = (int *)malloc(sizeof(int) * DEFAULT_SIZE);
		}
		
	}
	*numberofalleles = (int **)malloc(sizeof(int) * DEFAULT_SIZE * DEFAULT_SIZE);
	for (i = 0; i < DEFAULT_SIZE; i++) {
		numberofalleles[0][i] = (int *)malloc(sizeof(int) * DEFAULT_SIZE);
		*numberofalleles[0][i] = 0; // set this counter to 0
	}	

	FILE *file = fopen(filename, "r"); // open the file which was passed as an argument
	char tempstring[DEFAULT_SIZE]; // creates a temporary string used for building strings
	int index = 0; // index used for traversing the temporary string
	i = j = k = 0; // reset i, j, k to 0
	while (fread(buffer, 1, sizeof(char), file) != 0) { // read 1 character at a time until you reach the end of the file
		if (buffer[0] == '{') { // if the character read is '{' -- meaning, start of sample
			char *newstring = (char *)malloc(sizeof(char) * DEFAULT_SIZE);
			strcpy(newstring, tempstring);
			samplenames[0][i] = newstring; // add the temporary string as a sample name
			tempstring[index = 0] = 0; // reset index to 0, then reset tempstring to empty
			fread(buffer, 1, sizeof(char), file); // consume the following newline
		}
		else if (buffer[0] == '}') { // if the character read is '}' -- meaning, end of sample
			*numberofsamples = ++i; // increment the numberofsamples counter, and then the i index
			j = 0; // reset j back to 0
			k = 0; // reset k back to 0
			tempstring[index = 0] = 0;
			fread(buffer, 1, sizeof(char), file);
		}
		else if (buffer[0] == '[') { // if the character read is '[' -- meaning, start of locus
			char *newstring = (char *)malloc(sizeof(char) * DEFAULT_SIZE);
			strcpy(newstring, tempstring);
			locinames[0][i][j] = newstring;  // add the temporary string as a locus name
			tempstring[index = 0] = 0;
			fread(buffer, 1, sizeof(char), file);
		}
		else if (buffer[0] == ']') { // if the character read is ']' -- meaning, end of locus
			numberofloci[0][i] = ++j; // increment of numberofloci counter, and then the j index
			k = 0;
			tempstring[index = 0] = 0;
			fread(buffer, 1, sizeof(char), file);
		}
		else if (buffer[0] == '\n') { // if the character read is a '\n' -- meaning, end of allele
			alleles[0][i][j][k] = stringtointeger(tempstring); // convert the temporary string to an integer, and store it in the 3D array
			tempstring[index = 0] = 0;
			numberofalleles[0][i][j] = ++k; // increment the numberofalleles counter, then increment the k index
		}
		else if (buffer[0] == ' ') { // if the character read is a ' '
			// do nothing so the empty space is consumed
		}
		else { // this is where the temporary string is built
			tempstring[index++] = buffer[0]; // take the buffer, and add it to the temporary string, then inrement the temporary string's index
			tempstring[index] = 0; // then NULL terminate the temporary string
		}
	}
}

void writepossibilities(char *samplename, char **locinames, int numberofloci, int **possibilities, int *numberofpossibilities) {
	int i, j, k; // variables for traversing up to 3D

	// create a temporary string for integers, a string for the filename, to be concatenated with an extension string
	char intstring[4], filename[20], extension[5] = ".txt";
	strcpy(filename, samplename);
	strcat(filename, extension);

	FILE *file = fopen(filename, "w"); // open the file for (re)writing
	for (i = 0; i < numberofloci; i++) {
		fwrite(locinames[i], 1, strlen(locinames[i]), file); // write the locus name
		fwrite("[", 1, sizeof(char), file); // followed by '['
		fwrite("\n", 1, sizeof(char), file); // followed by '\n'

		// NOTE: THE NUMBER OF POSSIBILITIES WILL LIKELY HAVE TO BE MULTIPLIED BY 2 BECAUSE THEY COME IN PAIRS
		// I WILL FIX THIS SOON
		for (j = 0; j < *numberofpossibilities; j++) {
			sprintf(intstring, "%d", possibilities[i][j++]); // convert integer representation of allele to string
			fwrite(intstring, 1, strlen(intstring), file); // write the allele

			fwrite(",", 1, sizeof(char), file); // separate the pair with a comma

			sprintf(intstring, "%d", possibilities[i][j]);
			fwrite(intstring, 1, strlen(intstring), file);

			fwrite("\n", 1, sizeof(char), file); // write a newline
		}
		fwrite("]", 1, sizeof(char), file); // write ']' to close the allele
		fwrite("\n", 1, sizeof(char), file); // followed by a '\n'
	}
	fclose(file); // close the file

	/* -- PRESUMABLY, THIS MEMORY HAS BEEN ALLOCATED, BUT MUST BE FREED AFTERWARDS
	 * LEAVING THIS COMMENTED OUT FOR NOW
	free(locinames);
	free(possibilities);
	free(numberofpossibilities)l;
	*/
}


/*
 * this is a utility function for converting a string to its integer representation
 * NOTE: THIS IS NOT FINISHED YET
 */
int stringtointeger(char *string) {
	
	return atoi(string);
	if (strcmp(string, "NS")) {
		return -1;
	}
	if (strcmp(string, "INC")) {
		return 0;
	}
	int i = 0;
	int decimal = 0;
	int greater = 0;
	int less = 0;
	while (string[i] != 0) {
		if (string[i] == '.') {
			break;
		}
		else if (string[i] == '>') {
			break;
		}
		else if (string[i] == '<') {
			break;
		}
		else if (string[i] == '"') {
			break;
		}
		i++;
	}
	if (string[i] == 0) {
		return 10 * atoi(string);
	}
	else {
		return -100;
	}
	// read up to a decimal or non integer character
}
