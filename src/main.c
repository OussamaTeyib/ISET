#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX 100
#define MAX_NAME 21
#define MAX_CAPTION 10
// the struct to be read from the department's file
typedef struct { 
    int ID;
    char name[MAX_NAME];
    int coeff;
    int isTP;
    int mod;
} Matter;

typedef struct {
    int ID;
    char name[MAX_NAME];
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
char *strlower(char *);
void print(Mod **, int, float);
void printTable(Mod **, int, float);

int main(int argc, char *argv[])
{   
    char dName[MAX];  
    if (argc > 2)
    {
        fprintf(stderr, "Invalid number of arguments!\n");    
        fprintf(stderr, "Usage: main [departement-name]\n");
        return EXIT_FAILURE;
    } 
    else if (1 == argc)
    {
        char temp[MAX];
        printf("Saisir le nom du departement et le semestre: ");
        fgets(temp, MAX, stdin);
        temp[strlen(temp) - 1] = '\0'; 
        snprintf(dName, MAX, "../Departements/%s.bin", temp);
    }
    else // if the name is passed
        snprintf(dName, MAX, "../Departements/%s.bin", argv[1]);

    FILE *dep = fopen(dName, "rb");
    if (!dep)
    {
        fprintf(stderr, "Cannot open the file!\n");
        return EXIT_FAILURE;
    }

    int nMatters;
    if (1 != fread(&nMatters, sizeof nMatters, 1, dep))
    {
        fprintf(stderr, "The file is empty!\n");
        fclose(dep);
        return EXIT_FAILURE;
    }

    system("cls");
    Matter mat;
    FullMatter fmat[nMatters];
    for (int i = 0; i < nMatters; i++)
    {
        // cpy mat to fmat[i]
        fread(&mat, sizeof mat, 1, dep);
        fmat[i].ID = mat.ID;
        strncpy(fmat[i].name, mat.name, MAX_NAME);
        fmat[i].coeff = mat.coeff;
        fmat[i].isTP = mat.isTP;
        fmat[i].mod = mat.mod;
   
        printf("Saisir la note d%s%s\n", (isVowel(fmat[i].name[0])? "'" : "e "), strlower(fmat[i].name));
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
    for (int i = 0; i < nMatters;)
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
            mods[modCounter]->elms[j - i] = fmat[j]; // 'j - i' for indexing  
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
        mods[modCounter]->caption = validated;

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

char *strlower(char *str)
{
    if (!strlen(str))
        return NULL;

    char *lowstr = malloc(MAX_NAME);
    for (int i = 0; i < MAX_NAME; i++)
        lowstr[i] = tolower(str[i]);

    lowstr[MAX_NAME - 1] = '\0';
    return lowstr;
}

void printTable(Mod *mods[], int nMods, float moy)
{
    char name[MAX];
    printf("Saisir un nom pour le ficher: ");
    fflush(stdin);
    fgets(name, MAX, stdin);
    name[strlen(name) - 1] = '\0';
    
    char fName[MAX];
    snprintf(fName, MAX, "../results/%s.txt", name); 
    FILE *result = fopen(fName, "w+");
    if (!result)
    {
        fprintf(stderr, "Cannot create the output file!\n");
        exit(EXIT_FAILURE);
    }

    fprintf(result, " _______________________________________________________________________\n");
    fprintf(result, "|                     |      |      |     |     |     |      |          |\n");
    fprintf(result, "|      Élements       |Devoir|Examen| TP  |Note |Coeff|Module|  Mention |\n");
    fprintf(result, "|_____________________|______|______|_____|_____|_____|______|__________|\n");
    fprintf(result, "|                     |      |      |     |     |     |      |          |\n");

    for (int i = 0; i <nMods; i++)
    {
        for (int j = 0; j < mods[i]->nElm; j++)
        {
            fprintf(result, "|%-*s| %05.2f| %05.2f|", MAX_NAME, mods[i]->elms[j].name, mods[i]->elms[j].dev, mods[i]->elms[j].ex);
            if (mods[i]->elms[j].isTP)
                fprintf(result, "%05.2f|", mods[i]->elms[j].TP);
            else
                fprintf(result, "  -  |");
            fprintf(result, "%05.2f|", mods[i]->elms[j].elm);
            fprintf(result, "  %d  |", mods[i]->elms[j].coeff);
            if ((3 == mods[i]->nElm && j + 1 == 2) || 1 == mods[i]->nElm) 
                fprintf(result, " %05.2f|%-*s|\n", mods[i]->note, MAX_CAPTION, mods[i]->caption? "  Validé  " : "Non Validé");
            else
                fprintf(result, "      |          |\n");
            
            if (j + 1 < mods[i]->nElm)
            {
                fprintf(result, "|---------------------+------+------+-----+-----+-----|");
                if (2 == mods[i]->nElm)
                    fprintf(result, " %05.2f|%-*s|\n", mods[i]->note, MAX_CAPTION, mods[i]->caption? "  Validé  " : "Non Validé");
                else
                    fprintf(result, "      |          |\n");
            }
            else if (i + 1 < nMods) 
                fprintf(result, "|---------------------+------+------+-----+-----+-----+------+----------| \n");
            else
                fprintf(result, "|_____________________|______|______|_____|_____|_____|______|__________|\n");
        }
    }

    fprintf(result, "|                     |                                                 |\n");
    fprintf(result, "|       Moyenne       |                    %05.2f                        |\n", moy);
    fprintf(result, "|_____________________|_________________________________________________|\n");

    // print the file on the screen
    system("cls");
    fseek(result, 0, SEEK_SET);
    int c;
    while((c = fgetc(result)) != EOF)
        putchar(c);

    fclose(result);
}