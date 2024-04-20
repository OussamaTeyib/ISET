#include <stdio.h>
#include <stdlib.h>

#define MAX 64

void die(char *msg)
{
    fprintf(stderr, "%s\n", msg);
    exit(EXIT_FAILURE);
}

int main(int argc, char *argv[])
{
    system("cls");

    if (2 != argc)
        die("Usage: setup <dep-name>");

    char depName[MAX];
    snprintf(depName, MAX, "../src/Departements/%s.c", argv[1]);

    FILE *output = fopen(depName, "w");
    if (!output)
        die("ERROR: cannot open the file!");

    fprintf(output, "#include <stdio.h>\n");
    fprintf(output, "#include <stdlib.h>\n\n");
    fprintf(output, "#define DNAME %s\n", argv[1]);
    fprintf(output, "#define DSIZE %s_SIZE\n", argv[1]);
    fprintf(output, "extern unsigned char %s[];\n", argv[1]);
    fprintf(output, "extern size_t %s_SIZE;\n\n", argv[1]);

    fclose(output);

    char cmd[MAX];
    snprintf(cmd, MAX, "type ..\\src\\main.c >> ..\\src\\Departements\\%s.c", argv[1]);
    system(cmd);

    output = fopen(depName, "a");
    if (!output)
        die("ERROR: cannot open the file!");

    snprintf(depName, MAX, "../res/%s.bin", argv[1]);

    FILE *dep = fopen(depName, "rb");
    if (!dep)
        die("ERROR: cannot open the file!");
  
    // get the size of the input file
    fseek(dep, 0, SEEK_END);
    size_t size = ftell(dep);
    rewind(dep);

    unsigned char byte;
    size_t count = 0;
    fprintf(output, "\n\n");
    fprintf(output, "unsigned char %s[] = {", argv[1]); 
    while (1 == fread(&byte, 1, 1, dep))
    {
        fprintf(output, "0x%02X", byte);
        if (count < size - 1) // '-1' to avoid the last byte
            fprintf(output, ", ");
        count++;
    }
    fprintf(output, "};\n");
    fprintf(output, "size_t %s_SIZE = %zu;", argv[1], count);

    fclose(dep);
    fclose(output);

    printf("'%s.c' is created!\n", argv[1]);
    return EXIT_SUCCESS;
}