#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX 100
// the struct to be read from the department's file
typedef struct { 
    int ID;
    char name[21];
    int coeff;
    int isTP;
    int mod;
} Matter;

typedef struct {
    int ID;
    char name[21];
    int coeff;
    int isTP;
    int mod;
    float dev;
    float ex;
    float TP;
    float elm;
} FullMatter; 

typedef struct {
    int ID;
    int nElm;
    int coeff;
    float note;
    int caption;
    FullMatter elms[];
} Mod;

int isVowel(char);
void print(Mod **, int, float);
void printTable(Mod **, int, float);

int main(int argc, char *argv[])
{   
    char *dName = malloc(MAX);  
    if (argc > 2)
    {
        fprintf(stderr, "Invalid number of arguments!\n");    
        fprintf(stderr, "Usage: main [departement-name]\n");
        return EXIT_FAILURE;
    } 
    else if (1 == argc)
    {
        char *temp = malloc(MAX);
        printf("Saisir le nom du departement et le semestre: ");
        fgets(temp, MAX, stdin);
        temp[strlen(temp) - 1] = '\0'; 
        snprintf(dName, MAX, "Departements/%s.bin", temp);
        free(temp);
    }
    else // if the name is passed
        snprintf(dName, MAX, "Departements/%s.bin", argv[1]);

    FILE *dep = fopen(dName, "rb");
    if (!dep)
    {
        fprintf(stderr, "Cannot open the file!\n");
        free(dName);
        return EXIT_FAILURE;
    }

    free(dName); // we're done with it

    int nMatters = 0;
    if(1 != fread(&nMatters, sizeof nMatters, 1, dep) || !nMatters)
    {
        fprintf(stderr, "The file is empty!\n");
        fclose(dep);
        return EXIT_FAILURE;
    }

    Matter mat;
    FullMatter fmat[nMatters];
    for (int i = 0; i < nMatters; i++)
    {
        // cpy mat to fmat[i]
        fread(&mat, sizeof mat, 1, dep);
        fmat[i].ID = mat.ID;
        strncpy(fmat[i].name, mat.name, 21);
        fmat[i].coeff = mat.coeff;
        fmat[i].isTP = mat.isTP;
        fmat[i].mod = mat.mod;
   
        printf("Saisir la note d%s%s\n", (isVowel(fmat[i].name[0])? "'" : "e "), fmat[i].name);
        printf("Devoir: ");
        scanf("%f", &fmat[i].dev);
        printf("Examen: ");
        scanf("%f", &fmat[i].ex);
        if (fmat[i].isTP)
        {
            printf("TP: ");
            scanf("%f", &fmat[i].TP);
            fmat[i].elm = (fmat[i].ex * 3 + fmat[i].dev * 2 + fmat[i].TP) / 6;
        }
        else
            fmat[i].elm = (fmat[i].ex * 3 + fmat[i].dev * 2) / 5;
    }

    fclose(dep); // we're done with it
    int nMods = fmat[nMatters - 1].mod; // the module of last matter is the number of modules in the departement

    Mod *mods[nMods];
    float totalSum = 0.0f;
    int totalCoeff = 0, modCounter = 0;
    for (int i = 0; i <nMatters;)
    {
        int nElm = 0; // count number of elements in this module
        for (int j = i; fmat[j].mod == fmat[i].mod; j++, nElm++);
        // Allocate memory for nElm of elements
        mods[modCounter] = malloc(sizeof(Mod) + sizeof(FullMatter[nElm]));

        mods[modCounter]->nElm = nElm;
        mods[modCounter]->ID = modCounter + 1; // '+1' because that modCounter is an index statrs with 0
        mods[modCounter]->coeff = 0; // just an initialisation
        float tempSum = 0.0f;
        for (int j = i; j < i + mods[modCounter]->nElm; j++)
        {                    
            mods[modCounter]->coeff += fmat[j].coeff;
            tempSum += fmat[j].elm * fmat[j].coeff; 
            mods[modCounter]->elms[j - i] = fmat[j];   
        }
        // note of the module
        mods[modCounter]->note = tempSum / mods[modCounter]->coeff;
        // is this module validated?
        int validated = 0;
        if (mods[modCounter]->note >= 10)
        {
            validated = 1;
            for (int j = i; j < i + mods[modCounter]->nElm; j++)
            {
                if (mods[modCounter]->elms[j - i].elm < 7)
                {
                    validated = 0;
                    break;
                }
            }
        }
        mods[modCounter]->caption = validated; // must be modified

        totalSum += mods[modCounter]->note * mods[modCounter]->coeff;
        totalCoeff += mods[modCounter]->coeff;

        i += mods[modCounter]->nElm; // skip to the next module
        modCounter++;
    } 
    float moy = totalSum / totalCoeff;

    printTable(mods, nMods, moy);

    // free the memory allocated for the modules
    for (int i = 0; i < nMods; i++)
        free(mods[i]);

    return EXIT_SUCCESS;    
}

int isVowel(char c)
{
    switch(c)
    {
        case 'a':
        case 'e':
        case 'i':
        case 'o':
        case 'u':
        case 'A':
        case 'E':
        case 'I':
        case 'O':
        case 'U':
            return 1;
        default:
            return 0;
    }
}

void print(Mod *mods[], int nMods, float moy)
{
    FILE *result = fopen("result.txt", "w");
    if (!result)
    {
        fprintf(stderr, "Cannot create the output file!\n");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < nMods; i++)
    {
        fprintf(result, "Module #%d\n", mods[i]->ID);
        fprintf(result, "	Coeff: %d\n", mods[i]->coeff);
        fprintf(result, "	Note: %5.2f\n", mods[i]->note);
        fprintf(result, "	Mention: %s\n", mods[i]->caption? "Valide" : "Non Valide");
        fprintf(result, "	Nombres des elements: %d\n", mods[i]->nElm);
        fprintf(result, "	Elements:");
        for (int j = 0; j < mods[i]->nElm; j++)
        {
            fprintf(result, "\n		Element #%d\n", mods[i]->elms[j].ID);
            fprintf(result, "			Nom: %s\n", mods[i]->elms[j].name);
            fprintf(result, "			Coeff: %d\n", mods[i]->elms[j].coeff);
            fprintf(result, "			TP: %s\n", mods[i]->elms[j].isTP? "Oui" : "Non");
            fprintf(result, "			Module: %d\n", mods[i]->elms[j].mod);
            fprintf(result, "			Devoir: %5.2f\n", mods[i]->elms[j].dev);
            fprintf(result, "			Examen: %5.2f\n", mods[i]->elms[j].ex);
            if (mods[i]->elms[j].isTP) 
                fprintf(result, "			TP: %5.2f\n", mods[i]->elms[j].TP);
            fprintf(result, "			Note d'element: %5.2f\n", mods[i]->elms[j].elm);
        }
        fprintf(result, "--------------------------------------------\n");
    }
    fprintf(result, "La moyenne generale est: %5.2f\n", moy);
    fclose(result);
}

void printTable(Mod *mods[], int nMods, float moy)
{
    printf(" _________________________________________________________________________\n");
    printf("|                     |      |      |     |     |     |      |            |\n");
    printf("|      Élements       |Devoir|Examen| TP  |Note |Coeff|Module|  Mention   |\n");
    printf("|_____________________|______|______|_____|_____|_____|______|____________|\n");
    printf("|                     |      |      |     |     |     |      |            |\n");

    for (int i = 0; i <nMods; i++)
    {
        for (int j = 0; j < mods[i]->nElm; j++)
        {
            printf("|%21s|%6.2f|%6.2f|", mods[i]->elms[j].name, mods[i]->elms[j].dev, mods[i]->elms[j].ex);
            if (mods[i]->elms[j].isTP)
                printf("%5.2f|", mods[i]->elms[j].TP);
            else
                printf("  -  |");
            printf("%5.2f|", mods[i]->elms[j].elm);
            printf("%5d|", mods[i]->elms[j].coeff);
            if (3 == mods[i]->nElm && j + 1 == 2) 
                printf("%6.2f|%12s|\n", mods[i]->note, mods[i]->caption? "Valide" : "Non Valide");
            else
                printf("      |            |\n");
            
            if (j + 1 < mods[i]->nElm)
            {
                printf("|---------------------+------+------+-----+-----+-----|");
                if (2 == mods[i]->nElm)
                    printf("%6.2f|%12s|\n", mods[i]->note, mods[i]->caption? "Valide" : "Non Valide");
                else
                    printf("      |            |\n");
            }
            else if (i + 1 < nMods) 
                printf("|---------------------+------+------+-----+-----+-----+------+------------| \n");
            else
                printf("|_____________________|______|______|_____|_____|_____|______|____________|\n");
        }
    }

    printf("|                     |                                                   |\n");
    printf("|       Moyenne       |                     %5.2f                         |\n", moy);
    printf("|_____________________|___________________________________________________|\n");
}