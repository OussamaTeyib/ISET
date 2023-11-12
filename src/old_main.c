#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DNAME GEMS4
#define DSIZE GEMS4_SIZE
extern unsigned char GEMS4[];
extern size_t GEMS4_SIZE;

#define MAX 100
#define MAX_NAME 15
#define MAX_CAPTION 10

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

void printTable(Mod **, int, float);
void printInfo(void);
void GetNotes(FullMatter *fmat, int nMatters, FILE *dep);
void CalculateAve(FullMatter *fmat, int nMatters, Mod **mods, int nMods, float *ave);

int main(void)
{ 
    // Turn the array into a file  
    FILE *dep = fopen("temp.bin", "wb+");
    if (!dep)
    {
        fprintf(stderr, "Cannot Create a temprary file!\n");
        return EXIT_FAILURE;
    }
    fwrite(DNAME, DSIZE, 1, dep);
    rewind(dep);

    // Get the number of mattters
    int nMatters;
    fread(&nMatters, sizeof nMatters, 1, dep);

    // Get the notes
    FullMatter fmat[nMatters];
    GetNotes(fmat, nMatters, dep);

    // Clean-up
    fclose(dep); 
    remove("temp.bin"); // we're done with it!

    // Get the number of modules
    // the module of the last matter is the number of modules
    int nMods = fmat[nMatters - 1].mod;

    // Calculate the averages
    float ave;
    Mod *mods[nMods];
    CalculateAve(fmat, nMatters, mods, nMods, &ave);

    // Output the results
    printTable(mods, nMods, ave);
    printInfo();

    // Clean-up
    for (int i = 0; i < nMods; i++)
        free(mods[i]);

    return EXIT_SUCCESS;    
}

void GetNotes(FullMatter *fmat, int nMatters, FILE *dep)
{
    system("cls");
    Matter mat;
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
            printf("Saisir la note de `%s`: ", fmat[i].name);
            fflush(stdin);
            scanf("%f", &fmat[i].elm);
            continue;
        }

        fmat[i].isTP = mat.isTP;
        printf("Saisir la note de `%s`:\n", fmat[i].name);
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
}

void CalculateAve(FullMatter *fmat, int nMatters, Mod **mods, int nMods, float *ave)
{
    float totalSum = 0.0f;
    int totalCoeff = 0, modCounter = 0;
    for (int i = 0; i < nMatters;)
    {
        // count number of elements in this module
        int nElm = 0;
        for (int j = i; fmat[j].mod == fmat[i].mod; j++, nElm++);

        // Allocate memory for the elements
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
    *ave = totalSum / totalCoeff;
}

void printTable(Mod *mods[], int nMods, float moy)
{
    printf(" _________________________________________________________________\n");
    printf("|               |      |      |     |     |     |      |          |\n");
    printf("|    Élements   |Devoir|Examen| TP  |Note |Coeff|Module|  Mention |\n");
    printf("|_______________|______|______|_____|_____|_____|______|__________|\n");
    printf("|               |      |      |     |     |     |      |          |\n");

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
                printf("  %d  |", mods[i]->elms[j].coeff);
                if ((3 == mods[i]->nElm && j + 1 == 2) || 1 == mods[i]->nElm) 
                    printf(" %05.2f|%-*s|\n", mods[i]->note, MAX_CAPTION, mods[i]->caption? "  Validé  " : "Non Validé");
                else
                    printf("      |          |\n");


                if (j + 1 < mods[i]->nElm)
                {
                    printf("|---------------+------+------+-----+-----+-----|");
                    if (2 == mods[i]->nElm)
                        printf(" %05.2f|%-*s|\n", mods[i]->note, MAX_CAPTION, mods[i]->caption? "  Validé  " : "Non Validé");
                    else
                        printf("      |          |\n");
                }
                else if (i + 1 < nMods) 
                    printf("|---------------+------+------+-----+-----+-----+------+----------| \n");
                else
                    printf("|_______________|______|______|_____|_____|_____|______|__________|\n");
            }
            else
            {
                printf("|%-*s|            -            |  %d  | %05.2f|%-*s|\n", MAX_NAME, mods[i]->elms[j].name, mods[i]->elms[j].coeff, mods[i]->note, MAX_CAPTION, mods[i]->caption? "  Validé  " : "Non Validé");
                printf("|_______________|_________________________|_____|______|__________|\n");
            }
        }
    }

    printf("|               |                                                 |\n");
    printf("|    Moyenne    |                    %05.2f                        |\n", moy);
    printf("|_______________|_________________________________________________|\n");    
}

void printInfo(void)
{
    printf(" _________________________________________________________________\n");
    printf("|                                                                 |\n");
    printf("|         Ce programme a été écrit par Oussama Med Teyib.         |\n");
    printf("|_________________________________________________________________|\n");
    printf("|                                                                 |\n");
    printf("| Le code source de tous le projet est disponible sur:            |\n");
    printf("| https://github.com/OussamaTeyib/ISET                            |\n");
    printf("|_________________________________________________________________|\n");
}

unsigned char GEMS4[] = {0x08, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x56, 0x75, 0x6C, 0x67, 0x61, 0x72, 0x69, 0x73, 0x61, 0x74, 0x69, 0x6F, 0x6E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x56, 0x65, 0x6E, 0x74, 0x69, 0x6C, 0x61, 0x74, 0x69, 0x6F, 0x6E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x4D, 0x6F, 0x74, 0x65, 0x75, 0x72, 0x73, 0x00, 0x00, 0x6F, 0x6E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x45, 0x6C, 0x65, 0x63, 0x74, 0x72, 0x6F, 0x74, 0x65, 0x63, 0x68, 0x2E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x05, 0x00, 0x00, 0x00, 0x49, 0x6E, 0x73, 0x74, 0x72, 0x75, 0x6D, 0x65, 0x6E, 0x74, 0x61, 0x74, 0x69, 0x6F, 0x6E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x4D, 0x61, 0x74, 0x65, 0x72, 0x69, 0x61, 0x75, 0x78, 0x00, 0x00, 0x74, 0x69, 0x6F, 0x6E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x07, 0x00, 0x00, 0x00, 0x43, 0x6F, 0x6E, 0x73, 0x74, 0x2E, 0x20, 0x4D, 0x65, 0x63, 0x2E, 0x00, 0x00, 0x6F, 0x6E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x53, 0x74, 0x61, 0x67, 0x65, 0x00, 0x00, 0x4D, 0x65, 0x71, 0x2E, 0x00, 0x00, 0x6F, 0x6E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x07, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x05, 0x00, 0x00, 0x00};
size_t GEMS4_SIZE = 324;