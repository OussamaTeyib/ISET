#include <stdio.h>
#include <stdlib.h>

#define DNAME GABS6
#define DSIZE GABS6_SIZE
extern unsigned char GABS6[];
extern size_t GABS6_SIZE;

#define MAX 128
#define MAX_NAME 15
#define MAX_CAPTION 10

typedef struct
{
    int ID;
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

void printTable(Mod *mods[], int nMods, float grade);
void printInfo(void);

int main(void)
{
    system("chcp 65001");
    system("cls");

    // Turn the array into a file  
    FILE *dep = fopen("temp.bin", "wb+");
    if (!dep)
        die("Cannot Create a temprary file!");

    fwrite(DNAME, DSIZE, 1, dep);
    rewind(dep);

    // Get the number of modules
    int nMods;
    fread(&nMods, sizeof nMods, 1, dep);

    // Get modules from the file
    Mod *mods[nMods];
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

    // Get the notes from the user
    for (int i = 0; i < nMods; i++)
    {
        printf("Module #%d:\n", mods[i]->ID);

        for (int j = 0; j < mods[i]->nElms; j++)
        {
            if (mods[i]->elms[j].isSpecial)
            {
                printf("Saisir la note de '%s': ", mods[i]->elms[j].name);
                scanf("%f", &mods[i]->elms[j].note);
                continue;
            }

            printf("Saisir la note de '%s':\n", mods[i]->elms[j].name);
            printf("Devoir: ");
            scanf("%f", &mods[i]->elms[j].dev);
            printf("Examen: ");
            scanf("%f", &mods[i]->elms[j].ex);
            if (mods[i]->elms[j].isPractical)
            {
                printf("TP: ");
                scanf("%f", &mods[i]->elms[j].TP);
                mods[i]->elms[j].note = (mods[i]->elms[j].ex * 3 + mods[i]->elms[j].dev * 2 + mods[i]->elms[j].TP) / 6;
            }
            else
                mods[i]->elms[j].note = (mods[i]->elms[j].ex * 3 + mods[i]->elms[j].dev * 2) / 5;
        }

        printf("\n");
    }

    // Calculate notes of modules
    for (int i = 0; i < nMods; i++)
    {
        float tempSum = 0.0f;
        for (int j = 0; j < mods[i]->nElms; j++)       
            tempSum += mods[i]->elms[j].note * mods[i]->elms[j].coeff;

        mods[i]->note = tempSum / mods[i]->coeff;

        // is this module validated?
        int isValidated = 0;
        if (mods[i]->note >= 10)
        {
            isValidated = 1;
            for (int j = 0; j < mods[i]->nElms; j++)
            {
                if (mods[i]->elms[j].note < 7)
                {
                    isValidated = 0;
                    break;
                }
            }
        }
        mods[i]->isValidated = isValidated;
    }

    // Calculate the grade
    float grade, totalSum = 0.0f;
    int totalCoeff = 0;
    for (int i = 0; i < nMods; i++)
    {
        totalSum += mods[i]->note * mods[i]->coeff;
        totalCoeff += mods[i]->coeff;
    }
    grade = totalSum / totalCoeff;


    // Output the results
    printTable(mods, nMods, grade);
    printInfo();

    // Clean-up
    for (int i = 0; i < nMods; i++)
        free(mods[i]);

    fclose(dep); 
    remove("temp.bin");
    return EXIT_SUCCESS;    
}

void printTable(Mod *mods[], int nMods, float grade)
{
    printf(" _________________________________________________________________\n");
    printf("|               |      |      |     |     |     |      |          |\n");
    printf("|    Élements   |Devoir|Examen| TP  |Note |Coeff|Module|  Mention |\n");
    printf("|_______________|______|______|_____|_____|_____|______|__________|\n");
    if (!mods[0]->elms[0].isSpecial)
        printf("|               |      |      |     |     |     |      |          |\n");
    else
        printf("|               |                   |     |     |      |          |\n");

    for (int i = 0; i < nMods; i++)
    {
        for (int j = 0; j < mods[i]->nElms; j++)
        {
            printf("|%-*s|", MAX_NAME, mods[i]->elms[j].name);
            // if this is not a 'stage' or a 'projet'
            if (!mods[i]->elms[j].isSpecial)
            {
                printf(" %05.2f| %05.2f|", mods[i]->elms[j].dev, mods[i]->elms[j].ex);
                // Does this element have a 'TP'
                if (mods[i]->elms[j].isPractical)
                    printf("%05.2f|", mods[i]->elms[j].TP);
                else
                    printf("  -  |");
            }
            else
            {
                printf("         -         |");
            }

            printf("%05.2f|", mods[i]->elms[j].note);
            printf("%s %d  |", (mods[i]->elms[j].coeff >= 10)? "": " ", mods[i]->elms[j].coeff);

            // if there is one or three element(s) in the module
            if (1 == mods[i]->nElms || (3 == mods[i]->nElms && j + 1 == 2))
            {
                printf(" %05.2f|%-*s|\n", mods[i]->note, MAX_CAPTION, mods[i]->isValidated? "  Validé  " : "Non Validé");
            }
            else
            {
                printf("      |          |\n");
            }

            // if this is not the last element in the module
            if (j + 1 < mods[i]->nElms)
            {
                printf("|---------------+------+------+-----+-----+-----|");
                // if there is only two elements in the module
                if (2 == mods[i]->nElms)
                    printf(" %05.2f|%-*s|\n", mods[i]->note, MAX_CAPTION, mods[i]->isValidated? "  Validé  " : "Non Validé");
                else
                    printf("      |          |\n");
            }
            // if this is not the last element in the whole departement
            else if (i + 1 < nMods)
            { 
                printf("|---------------+------+------+-----+-----+-----+------+----------| \n");
            }
            else
            {
                if (!mods[i]->elms[j].isSpecial)
                    printf("|_______________|______|______|_____|_____|_____|______|__________|\n");
                else
                    printf("|_______________|___________________|_____|_____|______|__________|\n");
            }
        }
    }

    printf("|               |                                                 |\n");
    printf("|    Moyenne    |                    %05.2f                        |\n", grade);
    printf("|_______________|_________________________________________________|\n");    
}

void printInfo(void)
{
    printf(" _________________________________________________________________\n");
    printf("|                                                                 |\n");
    printf("| Le code source est disponible sur:                              |\n");
    printf("| https://github.com/OussamaTeyib/ISET                            |\n");
    printf("|_________________________________________________________________|\n");
}

unsigned char GABS6[] = {0x04, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0xB7, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x52, 0x65, 0x64, 0x61, 0x63, 0x74, 0x69, 0x6F, 0x6E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0xB7, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x41, 0x73, 0x73, 0x69, 0x6E, 0x69, 0x73, 0x73, 0x65, 0x6D, 0x65, 0x6E, 0x74, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x07, 0x00, 0x00, 0x00, 0xB7, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x4F, 0x75, 0x76, 0x72, 0x61, 0x67, 0x65, 0x73, 0x00, 0x00, 0x65, 0x6E, 0x74, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x42, 0x61, 0x72, 0x72, 0x61, 0x67, 0x65, 0x00, 0x00, 0x01, 0x00, 0x00, 0x50, 0x01, 0xFA, 0x80, 0xB7, 0x01, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x11, 0x00, 0x00, 0x00, 0xB7, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x50, 0x46, 0x45, 0x00, 0x00, 0x67, 0x65, 0x73, 0x00, 0x00, 0x65, 0x6E, 0x74, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x11, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
size_t GABS6_SIZE = 360;