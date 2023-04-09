#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
    char name[21];
    int coeff;
    int isTP;
    int mod;
} Matter;

typedef struct {
    int ID;
    int nElm;
    int coeff;
    float note;
    int caption;
    FullMatter elms[];
} Mod;

int isVowel(char);

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s <departement-name>\n", argv[0]);
        return -1;
    } 

    char *dName = malloc(100);
    snprintf(dName, 100, "Departements/%s.bin", argv[1]);

    FILE *dep = fopen(dName, "rb");
    if (!dep)
    {
        fprintf(stderr, "Cannot open the file!\n");
        free(dName);
        return -1;
    }

    int nMatters;
    if(1 != fread(&nMatters, sizeof nMatters, 1, dep) || !nMatters)
    {
        fprintf(stderr, "The file is empty!\n");
        free(dName);
        fclose(dep);
        return -1;
    }

    Matter mat;
    FullMatter fmat[nMatters];
    for (int i = 0; i < nMatters; i++)
    {
        fread(&mat, sizeof mat, 1, dep);
        fmat[i].ID = mat.ID;
        strncpy(fmat[i].name, mat.name, 21);
        fmat[i].coeff = mat.coeff;
        fmat[i].isTP = mat.isTP;
        fmat[i].mod = mat.mod;
   /*
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
            fmat[i].elm = (fmat[i].ex * 3 + fmat[i].dev * 2) / 5; */
    }

    int nMods = fmat[nMatters - 1].mod; // the module of last matter is the number of modules in the departement

    Mod mods[nMods];
    float totalSum = 0.0;
    int totalCoeff = 0, modCounter = 0;
    for (int i = 0; i <nMatters;)
    {
        int nElm = 0; // count number of elements in this module
        for (int j = i; fmat[j].mod == fmat[i].mod; j++, nElm++);
        mods[modCounter].nElm = nElm;

        mods[modCounter].ID = modCounter + 1; // modCounter is an index statrs with 0
        mods[modCounter].elms = malloc(sizeof fmat * mods[modCounter].nElm);
        mods[modCounter].coeff = 0; // intialisation
        float tempSum = 0.0;
        for (int j = i; j < i + mods[modCounter].nElm; j++)
        {                    
            mods[modCounter].coeff += fmat[j].coeff;
            tempSum += fmat[j].elm * fmat[j].coeff; 
            mods[modCounter].elms[j - i] = fmat[j];   
        }
        mods[modCounter].note = tempSum / mods[modCounter].coeff;

        mods[modCounter].caption = 1; // must be modified

        totalSum += mods[modCounter].note * mods[modCounter].coeff;
        totalCoeff += mods[modCounter].coeff;

        i += mods[modCounter].nElm;
        modCounter++;
    } 

    float moy = totalSum / totalCoeff;
    
    // free(mods[i].elms); must be done;
    free(dName);
    fclose(dep);
    return 0;    
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