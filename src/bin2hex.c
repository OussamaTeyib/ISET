// Convert a departement's bin file into a C array

#include <stdio.h>
#include <stdlib.h>

#define MAX 64

void die(char *msg)
{
    fprintf(stderr, "%s\n", msg);
    exit(EXIT_FAILURE);
}

int main(int argc, char* argv[])
{
    system("chcp 1254");
    system("cls");

    char depName[MAX];
    if (2 == argc)
    {
        snprintf(depName, MAX, "../res/%s.bin", argv[1]);
    }
    else
    {
        die("Usage: dep2hex <dep-name>");
    }

    FILE* dep = fopen(depName, "rb");
    if (!dep)
        die("ERROR: cannot open the input file!");
       
    // get the size of the input file
    fseek(dep, 0, SEEK_END);
    size_t size = ftell(dep);
    rewind(dep);

    unsigned char byte;
    size_t count = 0;
    printf("unsigned char %s[] = {", argv[1]); 
    while (1 == fread(&byte, 1, 1, dep))
    {
        printf("0x%02X", byte);
        if (count < size - 1) // '-1' to avoid the last byte
            printf(", ");
        count++;
    }
    printf("};\n");
    printf("size_t %s_SIZE = %zu;\n", argv[1], count);

    fclose(dep);
    return EXIT_SUCCESS;
}