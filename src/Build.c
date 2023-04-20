// Automate build process

#include <stdio.h>
#include <stdlib.h>
#define MAX 100

int main(void)
{
    // compile depMan.c
    if (system("gcc src//depMan.c -o bin//depMan"))
    {
        fprintf(stderr, "\nFailed to compile 'depMan.c'!\n");
        return EXIT_FAILURE;
    }
    printf("'depMan.c' is compiled successfully.\n");

    // compile main.c
    if (system("gcc src//main.c -o bin//main"))
    {
        fprintf(stderr, "\nFailed to compile 'main.c'!\n");
        return EXIT_FAILURE;
    }
    printf("'main.c' is compiled successfully.\n");
 
    return EXIT_SUCCESS;
}