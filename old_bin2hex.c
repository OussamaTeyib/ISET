// Convert a departement's bin file into a C array

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX 50

void die(char *msg)
{
    fprintf(stderr, "%s\n", msg);
    exit(EXIT_FAILURE);
}

int main(int argc, char* argv[])
{
    system("chcp 1254");
    system("cls");

    char input[MAX], output[MAX], arrName[MAX];
    if (argc == 3)
    {
        char tempInput[MAX], tempOutput[MAX];
        strncpy(tempInput, argv[1], MAX);
        snprintf(input, MAX, "../res/bin/%s", tempInput);
 
        strncpy(tempOutput, argv[2], MAX);
        snprintf(output, MAX, "../res/hex/%s", tempOutput); 

        strncpy(arrName, tempOutput, MAX);
        arrName[strcspn(arrName, ".")] = '\0';
    }
    else
    {
        char tempInput[MAX], tempOutput[MAX];
        printf("Enter the name of the input file: ");
        fgets(tempInput, MAX, stdin);
        tempInput[strcspn(tempInput, "\n")] = '\0';
        snprintf(input, MAX, "../res/bin/%s", tempInput);

        printf("Enter the name of the output file: ");
        fgets(tempOutput, MAX, stdin);
        tempOutput[strcspn(tempOutput, "\n")] = '\0';
        snprintf(output, MAX, "../res/hex/%s", tempOutput);

        strncpy(arrName, tempOutput, MAX);
        arrName[strcspn(arrName, ".")] = '\0';
    }

    FILE* fin = fopen(input, "rb");
    if (!fin)
        die("ERROR: cannot open the input file!");
       
    // get the size of the input file
    fseek(fin, 0, SEEK_END);
    int size = ftell(fin);
    rewind(fin);
    
    FILE* fout = fopen(output, "w");
    if(!fout)
    {
        fclose(fin);
        die("ERROR: cannot create the output file!");
    }   

    unsigned char byte;
    size_t count = 0;
    fprintf(fout, "unsigned char %s[] = {", arrName); 
    while (1 == fread(&byte, 1, 1, fin))
    {
        fprintf(fout, "0x%02X", byte);
        if (count < size - 1) // '-1' to avoid the last byte
            fprintf(fout, ", ");
        count++;
    }
    fprintf(fout, "};\n");
    fprintf(fout, "size_t %s_SIZE = %zu;\n", arrName, count);

    printf("%zu bytes successfully printed!\n", count);
    fclose(fin);
    fclose(fout);
    return EXIT_SUCCESS;
}