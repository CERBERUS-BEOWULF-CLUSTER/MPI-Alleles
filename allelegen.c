/* Richard Hayes
*/

#include <stdio.h>

int atoi(const char *nptr);

int main(int argc, char* argv[]) {
    //int numsamples = atoi(argv[1]);
    int numsamples = atoi("16");
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
