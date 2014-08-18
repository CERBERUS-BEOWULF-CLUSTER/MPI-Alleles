/* Richard Hayes
*/

#include <stdio.h>
#include <string.h>

int atoi(const char *nptr);
int probability(void);

int main(int argc, char* argv[]) {
    //int numsamples = atoi(argv[1]);
    int numsamples = atoi("16");
    char* filename = "samples.txt";
    char buffer[4];
    int i;
    const char* loci[15] = {"D8S1179", "D21S11", "D7S820", "CSF1PO", "D3S1358", "TH01", "D13S317", 				    "D16S539", "D2S1338", "D19S433", "vWA", "TPOX", "D18S5I", "D5S818", "FGA"};
    const char* symbols[5] = {">", "<", "\"", "INC", "NS"};

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
            fwrite(loci[j], 1, strlen(loci[j]), file);
            fwrite("[", 1, sizeof(char), file);
            fwrite("\n", 1, sizeof(char), file);
            int k;
            int l = probability();
	    if(l > 0) {
            	for(k = 0; k < l; k++) {
                    int a = rand()%30+1;
                    sprintf(buffer, "%d", a);
                    fwrite(buffer, 1, strlen(buffer), file);
		    if(rand()%100+1 >= 92) {
			if(rand()%2+1 == 1) {
			    fwrite("<", 1, sizeof(char), file);
			}
			else {
                    	    fwrite(">", 1, sizeof(char), file);
			}
		    }
                    fwrite("\n", 1, sizeof(char), file);
            	}
	    }
	    else if(l == 0) {
		sprintf(buffer, "%s", symbols[4]);
                fwrite(buffer, 1, strlen(buffer), file);
                fwrite("\n", 1, sizeof(char), file);
	    }
	    else {
		sprintf(buffer, "%s", symbols[2]);
                fwrite(buffer, 1, strlen(buffer), file);
		int a = rand()%30+1;
                sprintf(buffer, "%d", a);
                fwrite(buffer, 1, strlen(buffer), file);
		sprintf(buffer, "%s", symbols[2]);
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

int probability(void) {
    int r = rand()%100+1;
    if(r <= 85) {
	return 2; //has two alleles
    }
    else if(r <= 90) {
	return 1; //has only one allele
    }
    else if(r <= 97) {
	return rand()%4+3; //has three or more alleles
    }
    else if(r <= 98){
	return -1; //Inconclusive
    }
    else {
	return 0; //None seen
    }
}
