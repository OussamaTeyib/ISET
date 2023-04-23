/* Departements' Files Manager
   Written by Oussama Teyib
   April 2023
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAX 100
#define MAX_NAME 15 // chars only no '\n' nor '\0'

void die(char *msg)
{
    fprintf(stderr, "%s\n", msg);
    exit(EXIT_FAILURE);
}

typedef struct
{
    int ID;
    // +1: for the initial '\0' that terminates the string
    // +1: for '\0' that terminates the string to remove the '\n' (or a char if the input is &gt; the buffer)
    char name[MAX_NAME + 2];
    int isProject;
    int coeff;
    int TP;
    int mod;
} Matter;

int main(void)
{  
    system("cls");
    char temp[MAX];
    printf("Enter the name of the departement: ");
    fgets(temp, MAX, stdin);
    temp[strlen(temp) - 1] = '\0';

    char dName[MAX];
    snprintf(dName, MAX, "../res/Departements/%s.bin", temp);
    
    int choice, again, firstTime = 1; // 'add' choice must be provided only one time.
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
            if (choice) // must be incresed because 'adding' option is removed.
                choice++;
        }
        firstTime = 0;

        Matter mat;
        FILE *dep;
        int nMatters;
        switch (choice)
        {
            case 1: // add a departement          
                dep = fopen(dName, "wb");
                if(!dep)
                    die("Cannot create the file!");
            
                printf("Enter the number of matters (includes project): ");
                do
                {
                    fflush(stdin);
                    scanf("%d", &nMatters);
                    if (nMatters < 0)
                    {
                        printf("Number of matters can't be less than or equal to zero!\n");
                        printf("Again: ");
                    }
                } while(nMatters < 0);

                fwrite(&nMatters, sizeof nMatters, 1, dep);     
                int counter = 1;
                while (counter <= nMatters)
                {
                    mat.ID = counter;
                    printf("\nThis is a project? (1/0): ");
                    fflush(stdin);
                    scanf("%d", &mat.isProject);
                    
                    if (!mat.isProject)
                    {
                        printf("Enter the name of the matter #%d: ", mat.ID);
                        fflush(stdin);
                        fgets(mat.name, MAX_NAME + 2, stdin);
                        mat.name[strlen(mat.name) - 1] = '\0';
                        
                        printf("Enter the coefficient: ");
                        fflush(stdin);
                        scanf("%d", &mat.coeff);

                        printf("This matter have TP? (1 or 0): ");
                        fflush(stdin);
                        scanf("%d", &mat.TP);
                    }
                    else
                    {                        
                        printf("Enter the name of the Project: ");
                        fflush(stdin);
                        fgets(mat.name, MAX_NAME + 2, stdin);
                        mat.name[strlen(mat.name) - 1] = '\0';

                        printf("Enter the coefficient: ");
                        fflush(stdin);
                        scanf("%d", &mat.coeff);
                    }

                    printf("Enter the number of module: ");
                    fflush(stdin);
                    scanf("%d", &mat.mod);

                    if (1 == fwrite(&mat, sizeof mat, 1, dep))
                        counter++;
                }  
                fclose(dep);  // must be closed in order to use it again!           
                break;

            case 2: // modify a departement
                dep = fopen(dName, "rb+");
                if (!dep)
                    die("Cannot open the file!");

                int tempID;
                printf("Enter the ID of the matter (includes the project): ");
                fflush(stdin);
                scanf("%d", &tempID);

                fseek(dep, (long) sizeof(int), SEEK_SET); // skip nMaters;
                int found = 0;
                while (1 == fread(&mat, sizeof mat, 1, dep))
                {
                    if (tempID == mat.ID)
                    {
                        int tempChoice;
                        found = 1;
                        if (!mat.isProject)
                        {
                            printf("\nMatter #%d\n", mat.ID);
                            printf("	Name: %s\n", mat.name);
                            printf("	Coeff: %d\n", mat.coeff);
                            printf("	TP: %s\n", (mat.TP? "Yes" : "Non"));
                            printf("	Module #%d\n", mat.mod);      
                            
                            printf("\nWhat do you wanna modifie:\n");
                            printf("	1. Name\n");
                            printf("	2. Coeff\n");
                            printf("	3. TP\n");
                            printf("	4. Module\n");
                            printf("	5. Project\n");
                            printf("	0. Nothing\n");
                            printf("Enter Your choice: ");
                            do
                            {
                                fflush(stdin);
                                scanf("%d", &tempChoice);
                                if (tempChoice < 0 || tempChoice > 5)
                                    printf("ERROR!\nEnter a valid choice [0, 5]: ");
                            } while (tempChoice < 0 || tempChoice > 5);
                        
                            switch (tempChoice)
                            {
                                case 1:
                                    printf("\nEnter the new name: ");
                                    fflush(stdin);
                                    fgets(mat.name, MAX_NAME + 2, stdin);
                                    mat.name[strlen(mat.name) - 1] = '\0';
                                    break;

                                case 2:
                                    printf("Enter the new coefficient: ");
                                    fflush(stdin);
                                    scanf("%d", &mat.coeff);
                                    break;

                                case 3:
                                    printf("This matter have TP? (1 or 0): ");
                                    fflush(stdin);
                                    scanf("%d", &mat.TP);
                                    break;

                                case 4:
                                    printf("Enter the new number of module: ");
                                    fflush(stdin);
                                    scanf("%d", &mat.mod);
                                    break;
                            
                                case 5:
                                    printf("This is a project? (1/0): ");
                                    fflush(stdin);
                                    scanf("%d", &mat.isProject);
                                    if (mat.isProject)
                                    {
                                        printf("Enter the name of the Project: ");
                                        fflush(stdin);
                                        fgets(mat.name, MAX_NAME + 2, stdin);
                                        mat.name[strlen(mat.name) - 1] = '\0';

                                        printf("Enter the coefficient: ");
                                        fflush(stdin);
                                        scanf("%d", &mat.coeff);

                                        printf("Enter the number of module: ");
                                        fflush(stdin);
                                        scanf("%d", &mat.mod);
                                    }
                                    break;
                            
                                case 0: 
                                    printf("Be sure the next time!\n"); // ;)
                            }
                        }
                        else
                        {
                            printf("Project #%d:\n", mat.ID);
                            printf("	Name: %s\n", mat.name);
                            printf("	Coeff: %d\n", mat.coeff);
                            printf("	Module #%d\n", mat.mod);
                        
                            printf("\nWhat do you wanna modifie:\n");
                            printf("	1. Name\n");
                            printf("	2. Coeff\n");
                            printf("	3. Module\n");
                            printf("	4. Project\n");
                            printf("	0. Nothing\n");
                            printf("Enter Your choice: ");
                            do
                            {
                                fflush(stdin);
                                scanf("%d", &tempChoice);
                                if (tempChoice < 0 || tempChoice > 4)
                                    printf("ERROR!\nEnter a valid choice [0, 4]: ");
                            } while (tempChoice < 0 || tempChoice > 4);
                        
                            switch (tempChoice)
                            {
                                case 1:
                                    printf("\nEnter the new name: ");
                                    fflush(stdin);
                                    fgets(mat.name, MAX_NAME + 2, stdin);
                                    mat.name[strlen(mat.name) - 1] = '\0';
                                    break;

                                case 2:
                                    printf("Enter the new coefficient: ");
                                    fflush(stdin);
                                    scanf("%d", &mat.coeff);
                                    break;

                                case 3:
                                    printf("Enter the new number of module: ");
                                    fflush(stdin);
                                    scanf("%d", &mat.mod);
                                    break;
                            
                                case 4:
                                    printf("This is a project? (1/0): ");
                                    fflush(stdin);
                                    scanf("%d", &mat.isProject);
                                    if (!mat.isProject)
                                    {
                                        printf("Enter the name of the matter #%d: ", mat.ID);
                                        fflush(stdin);
                                        fgets(mat.name, MAX_NAME + 2, stdin);
                                        mat.name[strlen(mat.name) - 1] = '\0';

                                        printf("Enter the coefficient: ");
                                        fflush(stdin);
                                        scanf("%d", &mat.coeff);

                                        printf("This matter have TP? (1 or 0): ");
                                        fflush(stdin);
                                        scanf("%d", &mat.TP);

                                        printf("Enter the number of module: ");
                                        fflush(stdin);
                                        scanf("%d", &mat.mod);
                                    }
                                    break; 
                            
                                case 0: 
                                    printf("Be sure the next time!\n"); // ;)
                            }
                        }

                        fseek(dep, - (long) sizeof mat, SEEK_CUR);
                        fwrite(&mat, sizeof mat, 1, dep);
                        if(tempChoice)
                            printf("Done!\n");
                        break;    
                    }
                }

                if (!found)
                    fprintf(stderr, "The ID is incorrect!\n");

                fclose(dep);        
                break;
        
            case 3: // print the content of a departement
                dep = fopen(dName, "rb");
                if (!dep)
                    die("Cannot open the file!");

                int empty = 1;
                if (1 == fread(&nMatters, sizeof nMatters, 1, dep))
                {
                    empty = 0;
                    printf("\nNumber of matters in %s is: %d\n", temp, nMatters); 
                }   
      
                while (1 == fread(&mat, sizeof mat, 1, dep))
                {
                    if (mat.isProject)
                    {
                        printf("\nProject #%d:\n", mat.ID);
                        printf("	Name: %s\n", mat.name);
                        printf("	Coeff: %d\n", mat.coeff);
                    }
                    else
                    {
                        printf("\nMatter #%d\n", mat.ID);
                        printf("	Name: %s\n", mat.name);
                        printf("	Coeff: %d\n", mat.coeff);
                        printf("	TP: %s\n", (mat.TP? "Yes" : "Non"));
                    }
                    printf("	Module #%d\n", mat.mod);
                }

                if (empty)
                    fprintf(stderr, "\nThis file is EMPTY!\n");

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