/* Departements' Files Manager
   Written by Oussama Teyib
   April 2023
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>

#define MAX 100
#define MAX_NAME 15 // chars only no '\n' nor '\0'

typedef struct
{
    int ID;
    // +1: for the initial '\0' that terminates the string
    // +1: for '\0' that terminates the string to remove the '\n' (or a char if the input is &gt; the buffer)
    wchar_t name[MAX_NAME + 2];
    int coeff;
    int isSpecial; // for 'stage' or 'projet'
    int isPractical;
    float dev;
    float ex;
    float TP;
    float elm;
} Element;

typedef struct {
    int ID;
    int nElms;
    int coeff;
    float note;
    int isValidated;
    Element elms[];
} Mod;

void die(char *msg)
{
    fprintf(stderr, "%s\n", msg);
    exit(EXIT_FAILURE);
}

int main(void)
{  
    system("chcp 1254");
    system("cls");
    char temp[MAX];
    printf("Enter the name of the departement: ");
    fgets(temp, MAX, stdin);
    temp[strlen(temp) - 1] = '\0';

    char dName[MAX];
    snprintf(dName, MAX, "../res/%s.bin", temp);
    
    int choice, again, firstTime = 1; // 'add' choice is given only one time.
    printf("What do you want:\n");
    printf("	1. Add a departement\n");
    printf("	2. Modify a departement\n");
    printf("	3. Print a departement\n");
    printf("	0. Exist\n");
    printf("Enter Your choice: ");
    do
    {
        fflush(stdin);
        scanf("%d", &choice);
        if (choice < 0 || choice > 3)
            printf("ERROR!\nEnter a valid choice [0, 3]: ");
    } while (choice < 0 || choice > 3);

    while (1)
    {
        if (!firstTime)
        {
            system("cls");
            printf("What do you want again:\n");
            printf("	1. Modify the departement\n");
            printf("	2. Print the departement\n");
            printf("	0. Exist\n");
            printf("Enter Your choice: ");
            do
            {
                fflush(stdin);
                scanf("%d", &choice);
                if (choice < 0 || choice > 2)
                    printf("ERROR!\nEnter a valid choice [0, 2]: ");
            } while (choice < 0 || choice > 2);
            if (choice) // must be incresed because 'add' option was removed.
                choice++;
        }
        firstTime = 0;

        FILE *dep;
        Mod *mod;
        int nMods;

        switch (choice)
        {
            case 1: // add a departement        
                dep = fopen(dName, "wb");
                if (!dep)
                    die("Cannot create the file!");
            
                printf("Enter the number of modules: ");
                do
                {
                    fflush(stdin);
                    scanf("%d", &nMods);
                    if (nMods <= 0)
                    {
                        printf("Number of modules can't be less than or equal to zero!\n");
                        printf("Again: ");
                    }
                } while (nMods <= 0);

                fwrite(&nMods, sizeof nMods, 1, dep);

                int counter = 1;
                while (counter <= nMods)
                {
                    int nElms;
                    printf("\nEnter the number of elements in module #%d: ", counter);
                    do
                    {
                        fflush(stdin);
                        scanf("%d", &nElms);
                        if (nElms <= 0)
                        {
                            printf("Number of elements can't be less than or equal to zero!\n");
                            printf("Again: ");
                        }
                    } while (nElms <= 0);

                    mod = malloc(sizeof (Mod) + sizeof (Element[nElms]));
                    if (!mod)
                        die("Cannot allocate memory!");

                    mod->ID = counter;
                    mod->nElms = nElms;
                    mod->coeff = 0; // just an initialization

                    for (int i = 0; i < mod->nElms; i++)
                    {
                        mod->elms[i].ID = i + 1;

                        printf("\nEnter the name of element #%d: ", mod->elms[i].ID);
                        fflush(stdin);
                        fgetws(mod->elms[i].name, MAX_NAME + 2, stdin);
                        mod->elms[i].name[wcslen(mod->elms[i].name) - 1] = L'\0';
                        
                        printf("Enter the coefficient: ");
                        fflush(stdin);
                        scanf("%d", &mod->elms[i].coeff);
                        mod->coeff += mod->elms[i].coeff;

                        printf("Is this element a 'projet' or a 'stage'? (1/0): ");
                        fflush(stdin);
                        scanf("%d", &mod->elms[i].isSpecial);
                        if (mod->elms[i].isSpecial)
                            continue;

                        printf("Does this element have TP? (1 or 0): ");
                        fflush(stdin);
                        scanf("%d", &mod->elms[i].isPractical);
                    }

                    fwrite(&nElms, sizeof nElms, 1, dep);
                    fwrite(mod, sizeof (Mod) + sizeof (Element[mod->nElms]), 1, dep);
                    
                    free(mod); 
                    counter++;
                }
  
                fclose(dep); // must be closed in order to use it again!           
                break;

            // case 2: // modify a departement
        
            case 3: // print the content of a departement
                dep = fopen(dName, "rb");
                if (!dep)
                    die("Cannot open the file!");

                if (1 == fread(&nMods, sizeof nMods, 1, dep))
                {
                    printf("\nNumber of modules in %s is: %d\n", temp, nMods);
                }
                else
                {
                    printf("\nThis file is EMPTY!\n");
                    fclose(dep);
                    break;
                }   

                while (1)
                {
                    int nElms;
                    if (1 != fread(&nElms, sizeof nElms, 1, dep))
                        break;

                    mod = malloc(sizeof (Mod) + sizeof (Element[nElms]));
                    if (!mod)
                        die("Cannot allocate memory!");

                    fread(mod, sizeof (Mod) + sizeof (Element[nElms]), 1, dep);

                    printf("\nModule #%d\n", mod->ID);
                    printf("	Coeff: %d\n", mod->coeff);
                    printf("	Number of elements: %d\n", mod->nElms);
                    printf("	Elements:\n");

                    for (int i = 0; i < mod->nElms; i++)
                    {
                        printf("	Element #%d:\n", mod->elms[i].ID);
                        wprintf(L"		Name: %ls\n", mod->elms[i].name);
                        printf("		Coeff: %d\n", mod->elms[i].coeff);
                        printf("		Is a 'stage' or a 'projet': %s\n", mod->elms[i].isSpecial? "Yes": "No");

                        if (!mod->elms[i].isSpecial)
                            printf("		Is practical: %s\n", mod->elms[i].isPractical? "Yes": "No");
                    }

                    free(mod);
                }

                fclose(dep);
                break;           

            case 0:
                again = 0;                
        }

        if (choice)
        {
	    printf("\nAgain? (1 or 0): ");
            fflush(stdin);
            scanf("%d", &again);
        }

        if (!again)
        {
            printf("\nGood Bye!\n");
            return EXIT_SUCCESS;
        } 
    }      
}