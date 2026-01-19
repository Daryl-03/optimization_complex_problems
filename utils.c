//
// Created by Naku on 18/01/2026.
//

#include "utils.h"

bool exporter_solutions_gnuplot(Solution *solutions, int nombreSolutions, const char *path) {
    FILE *f = fopen(path, "w");
    if (!f) {
        perror("Erreur ouverture fichier");
        return false;
    }

    fprintf(f, "# cmax ct\n");
    for (int i = 0; i < nombreSolutions; i++) {
        fprintf(f, "%f %f\n", solutions[i].cout.cmax, solutions[i].cout.ct);
    }

    fclose(f);
    return true;
}

bool exporter_solutions_gnuplot_flag(Solution *solutions, bool *dominated, int n, const char *filename) {
    FILE *f = fopen(filename, "w");
    if (!f) return false;
    for (int i = 0; i < n; ++i) {
        fprintf(f, "%f %f %d\n",
                solutions[i].cout.cmax,
                solutions[i].cout.ct,
                dominated[i] ? 1 : 0);
    }
    fclose(f);
    return true;
}


Instance *getInstance() {
    const char *basePath = "C:\\Users\\Naku\\Desktop\\Naku_desktop\\EILCO\\ING3\\problemes_complexes\\tp1\\instances\\";

    char *fullPath = malloc(strlen(basePath) + 30 + 1);
    strcpy(fullPath, basePath);

    Instance *instance = read_instance(strcat(fullPath, "20_10_01.txt"));
    free(fullPath);
    return instance;
}