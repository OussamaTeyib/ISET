/* Departements' Files Manager
 * Written by Oussama Teyib
 * April 2023                  
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX 128
#define MAX_NAME 15 // chars only no '\n' nor '\0'

typedef struct
{
    int ID;
    // +1: for the initial '\0' that terminates the string
    // +1: for '\0' that terminates the string to remove the '\n' (or a char if the input is &gt; than the buffer)
    char name[MAX_NAME + 2];
    int coeff;
    int isSpecial; // for 'stage' or 'projet'
    int isPractical;
    float dev;
    float ex;
    float TP;
    float note;
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

int main(int argc, char *argv[])
{
    system("chcp 65001");
    system("cls");

    if (2 != argc)
        die("Usage: depman <dep-name>");

    char dName[MAX];
    snprintf(dName, MAX, "../res/%s.bin", argv[1]);
    
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
        Mod **mods;
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
                        fgets(mod->elms[i].name, MAX_NAME + 2, stdin);
                        mod->elms[i].name[strlen(mod->elms[i].name) - 1] = '\0';
                        
                        printf("Enter the coefficient: ");
                        fflush(stdin);
                        scanf("%d", &mod->elms[i].coeff);
                        mod->coeff += mod->elms[i].coeff;

                        printf("Is this element a 'projet' or a 'stage'? (1/0): ");
                        fflush(stdin);
                        scanf("%d", &mod->elms[i].isSpecial);
                        if (mod->elms[i].isSpecial)
                            continue;

                        printf("Does this element have a TP? (1/0): ");
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

            case 2: // modify a departement
                dep = fopen(dName, "rb+");
                if (!dep)
                    die("Cannot open the file!");

                if (1 != fread(&nMods, sizeof nMods, 1, dep))
                {
                    printf("\nThis file is EMPTY!\n");
                    fclose(dep);
                    break;
                }

                mods = malloc(nMods * sizeof (Mod *));
                if (!mods)
                    die("Cannot allocate memory!");

                for (int i = 0; i < nMods; i++)
                {
                    int nElms;
                    if (1 != fread(&nElms, sizeof nElms, 1, dep))
                        break;

                    mods[i] = malloc(sizeof (Mod) + sizeof (Element[nElms]));
                    if (!mods[i])
                        die("Cannot allocate memory!");

                    fread(mods[i], sizeof (Mod) + sizeof (Element[nElms]), 1, dep);
                }

                int modID;
                printf("Enter the ID of the module: ");
                do
                {
                    fflush(stdin);
                    scanf("%d", &modID);
                    if (modID <= 0 || modID > nMods)
                    {
                        printf("Invalid ID!\n");
                        printf("Again: ");
                    }
                } while (modID <= 0 || modID > nMods);

                modID--; // for indexing

                printf("\nModule #%d\n", mods[modID]->ID);
                printf("	Coeff: %d\n", mods[modID]->coeff);
                printf("	Number of elements: %d\n", mods[modID]->nElms);
                printf("	Elements:\n");

                for (int i = 0; i < mods[modID]->nElms; i++)
                    printf("	Element #%d: %s\n", mods[modID]->elms[i].ID, mods[modID]->elms[i].name);

                int elmID;
                printf("\nEnter the ID of the element: ");
                do
                {
                    fflush(stdin);
                    scanf("%d", &elmID);
                    if (elmID <= 0 || elmID > mods[modID]->nElms)
                    {
                        printf("Invalid ID!\n");
                        printf("Again: ");
                    }
                } while (elmID <= 0 || elmID > mods[modID]->nElms);

                elmID--; // for indexing

                printf("\nElement #%d:\n", mods[modID]->elms[elmID].ID);
                printf("	Name: %s\n", mods[modID]->elms[elmID].name);
                printf("	Coeff: %d\n", mods[modID]->elms[elmID].coeff);
                printf("	Is a 'stage' or a 'projet': %s\n", mods[modID]->elms[elmID].isSpecial? "Yes": "No");

                if (!mods[modID]->elms[elmID].isSpecial)
                    printf("	Is practical: %s\n", mods[modID]->elms[elmID].isPractical? "Yes": "No");

                int option; 
                printf("\nWhat do you wanna modifie:\n");
                printf("	1. Name\n");
                printf("	2. Coeff\n");
                printf("	3. Is special\n");
                printf("	4. Is practical\n");
                printf("	0. Nothing\n");
                printf("Enter Your choice: ");
                do
                {
                    fflush(stdin);
                    scanf("%d", &option);
                    if (option < 0 || option > 4)
                        printf("ERROR!\nEnter a valid choice [0, 4]: ");
                } while (option < 0 || option > 4);
                        
                switch (option)
                {
                    case 1:
                        printf("\nEnter the new name: ");
                        fflush(stdin);
                        fgets(mods[modID]->elms[elmID].name, MAX_NAME + 2, stdin);
                        mods[modID]->elms[elmID].name[strlen(mods[modID]->elms[elmID].name) - 1] = '\0';
                        break;

                    case 2:
                        mods[modID]->coeff -= mods[modID]->elms[elmID].coeff;
                        printf("Enter the new coefficient: ");
                        fflush(stdin);
                        scanf("%d", &mods[modID]->elms[elmID].coeff);
                        mods[modID]->coeff += mods[modID]->elms[elmID].coeff;
                        break;

                    case 3:
                        printf("Is this element a 'projet' or a 'stage'? (1/0): ");
                        fflush(stdin);
                        scanf("%d", &mods[modID]->elms[elmID].isSpecial);
                        break;

                    case 4:
                        printf("Does this element have a 'TP'? (1/0): ");
                        fflush(stdin);
                        scanf("%d", &mods[modID]->elms[elmID].isPractical);
                        break;

                    case 0: 
                        printf("Be sure the next time!\n"); // ;)
                }

                fseek(dep, sizeof nMods, SEEK_SET);

                for (int i = 0; i < nMods; i++)
                {
                    fwrite(&mods[i]->nElms, sizeof mods[i]->nElms, 1, dep);
                    fwrite(mods[i], sizeof (Mod) + sizeof (Element[mods[i]->nElms]), 1, dep);
                    free(mods[i]);
                }
                free(mods);

                if (option)
                    printf("Done!\n");

                fclose(dep);
                break;
        
            case 3: // print the content of a departement
                dep = fopen(dName, "rb");
                if (!dep)
                    die("Cannot open the file!");

                if (1 != fread(&nMods, sizeof nMods, 1, dep))
                {
                    printf("\nThis file is EMPTY!\n");
                    fclose(dep);
                    break;
                }

                mods = malloc(nMods * sizeof (Mod *));
                if (!mods)
                    die("Cannot allocate memory!");

                for (int i = 0; i < nMods; i++)
                {
                    int nElms;
                    if (1 != fread(&nElms, sizeof nElms, 1, dep))
                        break;

                    mods[i] = malloc(sizeof (Mod) + sizeof (Element[nElms]));
                    if (!mods[i])
                        die("Cannot allocate memory!");

                    fread(mods[i], sizeof (Mod) + sizeof (Element[nElms]), 1, dep);
                }  

                printf(" ______________________________________________________\n");
                printf("|   |     |                |     |         |           |\n");
                printf("|Mod|Coeff|    Élements    |Coeff|Special? |Practical? |\n");
                printf("|___|_____|________________|_____|_________|___________|\n");

                for (int i = 0; i < nMods; i++)
                {
                    printf("|   |     |                |     |         |           |\n");
                    for (int j = 0; j < mods[i]->nElms; j++)
                    {
                        // if there is one or three elements in the module
                        if (1 == mods[i]->nElms || (3 == mods[i]->nElms && j + 1 == 2))
                        {
                            printf("|#%d |", mods[i]->ID);
                            printf("%s %d  |", (mods[i]->coeff >= 10)? "": " ", mods[i]->coeff);
                        }
                        else
                            printf("|   |     |");

                        printf(" %-*s|", MAX_NAME, mods[i]->elms[j].name);
                        printf("%s %d  |", (mods[i]->elms[j].coeff >= 10)? "": " ", mods[i]->elms[j].coeff);
                        // if this is a 'stage' or a 'projet'
                        if (mods[i]->elms[j].isSpecial)
                            printf("   YES   |     -     |\n");
                        else
                            printf("   NO    |    %s    |\n", mods[i]->elms[j].isPractical? "YES": "NO ");

                        // if this is not the last element in the module
                        if (j + 1 < mods[i]->nElms)
                        {
                            if (2 == mods[i]->nElms)
                            {
                                printf("|#%d |", mods[i]->ID);
                                printf("%s %d  |", (mods[i]->coeff >= 10)? "": " ", mods[i]->coeff);
                            }
                            else
                                printf("|   |     |");
                         
                            printf("----------------+-----+---------+-----------|\n");
                        }
                        else
                            printf("|___|_____|________________|_____|_________|___________|\n");
                    }

                    free(mods[i]);
                }

                free(mods);
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