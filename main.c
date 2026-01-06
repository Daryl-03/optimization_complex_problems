#include <stdio.h>
#include "lib.h"
#include "string.h"

Instance *getInstance() {
    const char *basePath = "C:\\Users\\Naku\\Desktop\\Naku_desktop\\EILCO\\ING3\\problemes_complexes\\tp1\\instances\\";

    char *fullPath = malloc(strlen(basePath) + 30 + 1);
    strcpy(fullPath, basePath);

    Instance *instance = read_instance(strcat(fullPath, "50_10_01.txt"));
    free(fullPath);
    return instance;
}

int launch_Optimization() {
    Instance *instance = getInstance();

    if (instance == NULL) {
        printf("no instance");
        return 0;
    }

    afficher_instance(*instance);


    clock_t start, stop;
    start = clock();

    double nombre_iterations = 100000;
    printf("Marche aleatoire de %.f iterations :\n", nombre_iterations);
    Solution solution_marche_insertion = marche_aleatoire_op(instance, nombre_iterations, INSERTION);
    Solution solution_marche_echange = marche_aleatoire_op(instance, nombre_iterations, ECHANGE);

    printf("\n");
    printf("%f insertion vs %f echange\n", solution_marche_insertion.cmax, solution_marche_echange.cmax);

    printf("Climber first \n");
    Solution solution_climb_first_ins = climber_first(instance, INSERTION);
    Solution solution_climb_first_ech = climber_first(instance, ECHANGE);
    printf("Apres climb first insertion :\n");


    Solution solution_climb_best = climber_best(instance, INSERTION);
    printf("Apres climb best :\n");
    afficher_solution(solution_climb_best);

    stop = clock();

    double time = (double) (stop - start) / CLOCKS_PER_SEC;
    printf("Le calcul a pris %.2f millisecondes\n", time * 1000);

//    free(solution);
    free_instance(instance);
}

int main(void) {

    srand(time(NULL));
    Instance *instance = getInstance();
    afficher_instance(*instance);
//    launch_Optimization();
    Solution solution;
    int a[50] = {
            20,47,12,39,37,16,30,42,48,13,3,36,41,15,11,38,23,5,2,34,49,27,35,45,26,28,44,33,18,9,7,21,46,29,10,32,17,25,24,22,4,14,40,6,0,1,8,31,43,19
    };
    int solutionA[50]={
            17,28,31,41,13,30,24,42,43,35,21,9,47,48,8,25,19,46,40,37,12,33,39,32,6,0,16,2,4,36,27,11,14,7,10,20,18,15,49,22,1,26,45,3,34,5,38,29,44,23
    };
    solution.jobOrder = a;
    solution.nombreDeJobs = 20;

    Cout c = eval_mo(instance, a);
    printf("les valeurs de cout : %f et %f", c.cmax, c.ct);
    return 0;
}