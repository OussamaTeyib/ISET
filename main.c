#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        fprintf(stderr, "Invalid number of arguments!\n");        
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
    free(dName);

    int nMatters = 0;
    if(1 != fread(&nMatters, sizeof nMatters, 1, dep) || !nMatters)
    {
        fprintf(stderr, "The file is empty!\n");
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
    fclose(dep);

    int nMods = fmat[nMatters - 1].mod; // the module of last matter is the number of modules in the departement

    Mod *mods[nMods];
    float totalSum = 0.0f;
    int totalCoeff = 0, modCounter = 0;
    for (int i = 0; i <nMatters;)
    {
        int nElm = 0; // count number of elements in this module
        for (int j = i; fmat[j].mod == fmat[i].mod; j++, nElm++);
        mods[modCounter] = malloc(sizeof(Mod) + sizeof(FullMatter[nElm]));

        mods[modCounter]->ID = modCounter + 1; // modCounter is an index statrs with 0
        mods[modCounter]->nElm = nElm;
        mods[modCounter]->coeff = 0; // intialisation
        float tempSum = 0.0f;
        for (int j = i; j < i + mods[modCounter]->nElm; j++)
        {                    
            mods[modCounter]->coeff += fmat[j].coeff;
            tempSum += fmat[j].elm * fmat[j].coeff; 
            mods[modCounter]->elms[j - i] = fmat[j];   
        }
        mods[modCounter]->note = tempSum / mods[modCounter]->coeff;

        mods[modCounter]->caption = 1; // must be modified

        totalSum += mods[modCounter]->note * mods[modCounter]->coeff;
        totalCoeff += mods[modCounter]->coeff;

        i += mods[modCounter]->nElm;
        modCounter++;
    } 
    float moy = totalSum / totalCoeff;

    FILE *result = fopen("result.txt", "w");
    if (!result)
    {
        fprintf(stderr, "Cannot create the output file!\n");
        return -1;
    }

    for (int i = 0; i < nMods; i++)
    {
        fprintf(result, "Module #%d\n", mods[i]->ID);
        fprintf(result, "	Coeff: %d\n", mods[i]->coeff);
        fprintf(result, "	Note: %5.2f\n", mods[i]->note);
        fprintf(result, "	Mention: %s\n", mods[i]->caption? "Valide" : "Non Valide");
        fprintf(result, "	Nombres des elements: %d\n", mods[i]->nElm);
        fprintf(result, "	Elements:\n");
        for (int j = 0; j < mods[i]->nElm; j++)
        {
            fprintf(result, "\n	Element #%d\n", mods[i]->elms[j].ID);
            fprintf(result, "		Nom: %s\n", mods[i]->elms[j].name);
            fprintf(result, "		Coeff: %d\n", mods[i]->elms[j].coeff);
            fprintf(result, "		TP: %s\n", mods[i]->elms[j].isTP? "Oui" : "Non");
            fprintf(result, "		Module: %d\n", mods[i]->elms[j].mod);
            fprintf(result, "		Devoir: %5.2f\n", mods[i]->elms[j].dev);
            fprintf(result, "		Examen: %5.2f\n", mods[i]->elms[j].ex);
            if (mods[i]->elms[j].isTP) 
                fprintf(result, "		TP: %5.2f\n", mods[i]->elms[j].TP);
            fprintf(result, "		Note d'element: %5.2f\n", mods[i]->elms[j].elm);
        }
        fprintf(result, "--------------------------------------------\n");
    }

    fclose(result);
    /*
       
    fwrite(&nMods, sizeof nMods, 1, result); 
    for (int i = 0; i < nMods; i++)
        fwrite(mods[i], sizeof(Mod) + sizeof(FullMatter[mods[i]->nElm]), 1, result);
    
    fclose(result); 

    print(); */
  
    // free(mods[i].elms); must be done;
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
/*
void print()
{
    FILE *result = fopen("result.bin", "rb");
    if (!result)
    {
        fprintf(stderr, "Cannot open the output file!\n");
        exit(EXIT_FAILURE);
    }

    int nMods;
    fread(&nMods, sizeof nMods, 1, result);
    Mod mod;
    Ful
    for (int i = 0; i <nMods; i++)
    {
        fread(&mod, sizeof mod, 1, result);
        int nElm = mod.nElm;
        fseek(result, - (long) sizeof mod



    }
} */