#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX 100
#define MAX_NAME 15
#define MAX_CAPTION 10

#define DNAME GEMS4
#define DSIZE GEMS4_SIZE
extern unsigned char GEMS4[];
extern size_t GEMS4_SIZE;

// the struct to be read from the department's file
typedef struct { 
    int ID;
    char name[MAX_NAME + 2];
    int isProject;
    int coeff;
    int isTP;
    int mod;
} Matter;

typedef struct {
    int ID;
    char name[MAX_NAME + 2];
    int isProject;
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

void die(char *);
int isVowel(char);
char *strlower(char *);
void printTable(Mod **, int, float);
void printInfo(void);

int main(void)
{   
    // system("chcp 1254");
    FILE *dep = fopen("temp.bin", "wb+");
    if (!dep)
        die("ERROR: Cannot create the temoprary file!");

    fwrite(DNAME, DSIZE, 1, dep);
    rewind(dep);

    int nMatters;
    if (1 != fread(&nMatters, sizeof nMatters, 1, dep))
    {
        fclose(dep);
        remove("temp.bin");
        die("ERROR: The corresponding file is empty!");
    }

    system("cls");
    Matter mat;
    FullMatter fmat[nMatters];
    for (int i = 0; i < nMatters; i++)
    {
        // cpy mat to fmat[i]
        fread(&mat, sizeof mat, 1, dep);
        fmat[i].ID = mat.ID;
        strncpy(fmat[i].name, mat.name, MAX_NAME + 2);
        fmat[i].coeff = mat.coeff;
        fmat[i].mod = mat.mod;
        fmat[i].isProject = mat.isProject; 
        if (fmat[i].isProject)
        {
            printf("Saisir la note du %s: ", strlower(fmat[i].name));
            fflush(stdin);
            scanf("%f", &fmat[i].elm);
            continue;
        }

        fmat[i].isTP = mat.isTP;
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

    fclose(dep); 
    remove("temp.bin"); // we're done with it!

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
        int isValidated = 0;
        if (mods[modCounter]->note >= 10)
        {
            isValidated = 1;
            for (int j = i; j < i + mods[modCounter]->nElm; j++)
            {
                if (mods[modCounter]->elms[j - i].elm < 7)
                {
                    isValidated = 0;
                    break;
                }
            }
        }
        mods[modCounter]->caption = isValidated;

        totalSum += mods[modCounter]->note * mods[modCounter]->coeff;
        totalCoeff += mods[modCounter]->coeff;

        i += mods[modCounter]->nElm; // skip to the next modules
        modCounter++;
    } 
    float moy = totalSum / totalCoeff;

    printTable(mods, nMods, moy);
    printInfo();

    // free the memory allocated for the modules
    for (int i = 0; i < nMods; i++)
        free(mods[i]);

    return EXIT_SUCCESS;    
}

void die(char *msg)
{
    fprintf(stderr, "%s\n", msg);
    exit(EXIT_FAILURE);
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
        case 'h':
        case 'A':
        case 'E':
        case 'I':
        case 'O':
        case 'U':
        case 'H':
            return 1;
        default:
            return 0;
    }
}

char *strlower(char *str)
{
    if (!strlen(str))
        return NULL;

    char *lowstr = malloc(MAX_NAME + 1);
    for (int i = 0; i < MAX_NAME; i++)
        lowstr[i] = tolower(str[i]);

    lowstr[MAX_NAME + 1] = '\0';
    return lowstr;
}

void printTable(Mod *mods[], int nMods, float moy)
{
    printf(" _______________________________________________\n");
    printf("|               |      |      |     |     |     |\n");
    printf("|    Élements   |Devoir|Examen| TP  |Note |Coeff|\n");
    printf("|_______________|______|______|_____|_____|_____|\n");
    printf("|               |      |      |     |     |     |\n");

    for (int i = 0; i <nMods; i++)
    {
        for (int j = 0; j < mods[i]->nElm; j++)
        {
            if (!mods[i]->elms[j].isProject)
            {
                printf("|%-*s| %05.2f| %05.2f|", MAX_NAME, mods[i]->elms[j].name, mods[i]->elms[j].dev, mods[i]->elms[j].ex);
                if (mods[i]->elms[j].isTP)
                    printf("%05.2f|", mods[i]->elms[j].TP);
                else
                    printf("  -  |");

                printf("%05.2f|", mods[i]->elms[j].elm);
                printf("  %d  |\n", mods[i]->elms[j].coeff);
            }
            else
            {
                printf("|%-*s|         -         |%05.2f|  %d  |\n", MAX_NAME, mods[i]->elms[j].name, mods[i]->elms[j].elm, mods[i]->elms[j].coeff); 
            }

            if (j + 1 < mods[i]->nElm)
                printf("|---------------+------+------+-----+-----+-----|\n");
            else
                printf("|---------------+-------------+-----------------|\n");
        }

        printf("|               |     Note    |     Mention     |\n");
        printf("|    Module %d   |-------------|-----------------|\n", i + 1);
        printf("|               |    %05.2f    |   %-*s    |\n", mods[i]->note, MAX_CAPTION, mods[i]->caption? "  Validé  " : "Non Validé");
            
        if (i + 1 < nMods) 
            printf("|---------------+-------------+-----------------| \n");
        else
            printf("|_______________|_____________|_________________|\n");
    }

    printf("|               |                               |\n");
    printf("|    Moyenne    |           %05.2f               |\n", moy);
    printf("|_______________|_______________________________|\n");    
}

void printInfo(void)
{
    printf(" ______________________________________________\n");
    printf("|                                               |\n");
    printf("|Ce programme a été écrit par Oussama Med Teyib |\n");
    printf("|______________________________________________ |\n");
    printf("|                                               |\n");
    printf("| Le code source du projet est disponible sur:  |\n");
    printf("| https://github.com/OussamaTeyib/ISET          |\n");
    printf("|_______________________________________________|\n");
}

unsigned char GEMS4[] = {0x08, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x56, 0x75, 0x6C, 0x67, 0x61, 0x72, 0x69, 0x73, 0x61, 0x74, 0x69, 0x6F, 0x6E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x56, 0x65, 0x6E, 0x74, 0x69, 0x6C, 0x61, 0x74, 0x69, 0x6F, 0x6E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x4D, 0x6F, 0x74, 0x65, 0x75, 0x72, 0x73, 0x00, 0x00, 0x6F, 0x6E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x45, 0x6C, 0x65, 0x63, 0x74, 0x72, 0x6F, 0x74, 0x65, 0x63, 0x68, 0x2E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x05, 0x00, 0x00, 0x00, 0x49, 0x6E, 0x73, 0x74, 0x72, 0x75, 0x6D, 0x65, 0x6E, 0x74, 0x61, 0x74, 0x69, 0x6F, 0x6E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x4D, 0x61, 0x74, 0x65, 0x72, 0x69, 0x61, 0x75, 0x78, 0x00, 0x00, 0x74, 0x69, 0x6F, 0x6E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x07, 0x00, 0x00, 0x00, 0x43, 0x6F, 0x6E, 0x73, 0x74, 0x2E, 0x20, 0x4D, 0x65, 0x63, 0x2E, 0x00, 0x00, 0x6F, 0x6E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x53, 0x74, 0x61, 0x67, 0x65, 0x00, 0x00, 0x4D, 0x65, 0x71, 0x2E, 0x00, 0x00, 0x6F, 0x6E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x07, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x05, 0x00, 0x00, 0x00};
size_t GEMS4_SIZE = 324;