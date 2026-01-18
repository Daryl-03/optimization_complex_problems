#include <stdio.h>
#include "lib.h"
#include "string.h"
#include "multivar.h"

Instance *getInstance() {
    const char *basePath = "C:\\Users\\Naku\\Desktop\\Naku_desktop\\EILCO\\ING3\\problemes_complexes\\tp1\\instances\\";

    char *fullPath = malloc(strlen(basePath) + 30 + 1);
    strcpy(fullPath, basePath);

    Instance *instance = read_instance(strcat(fullPath, "7_5_01.txt"));
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
    printf("%f insertion vs %f echange\n", solution_marche_insertion.cout.cmax, solution_marche_echange.cout.cmax);

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
            20,43,47,35,34,12,41,8,5,19,11,42,31,17,33,14,27,10,37,6,44,1,40,46,32,29,3,24,0,36,16,4,39,38,48,13,7,28,9,2,21,45,30,26,49,23,25,18,22,15

    };
    int solutionA[50]={
            17,28,31,41,13,30,24,42,43,35,21,9,47,48,8,25,19,46,40,37,12,33,39,32,6,0,16,2,4,36,27,11,14,7,10,20,18,15,49,22,1,26,45,3,34,5,38,29,44,23
    };
    solution.jobOrder = a;
    solution.nombreDeJobs = 20;

    int nombreDeSolutions = 5;
    Solution solutions[nombreDeSolutions];
    for(int i =0; i<5; i++){
        solutions[i].nombreDeJobs = instance->nombreDeJobs;
    }
    solutions[0].jobOrder = (int[]){ 5, 6, 4, 1, 3, 2, 0 };
    solutions[1].jobOrder = (int[]){ 5, 2, 3, 0, 6, 4, 1 };
    solutions[2].jobOrder = (int[]){ 2, 5, 4, 3, 0, 6, 1 };
    solutions[3].jobOrder = (int[]){ 2, 4, 5, 3, 0, 1, 6 };
    solutions[4].jobOrder = (int[]){ 6, 4, 1, 5, 2, 3, 0 };


    //    Cout c = eval_mo(instance, solutionA);
    //    printf("les valeurs de cout : %f et %f", c.cmax, c.ct);

//    printf("filtrage offline des solutions\n");
//    int nombreSolutionsNonDominees = 0;
//    Solution* result = filtrage_offline( instance, solutions, nombreDeSolutions, &nombreSolutionsNonDominees);
//
//    //  Les solutions apres filtrage offline :
//    printf("Les solutions apres filtrage offline :\n");
//    printf("Nombre de solutions : %d\n", nombreSolutionsNonDominees);
//
//    for(int i =0; i< nombreSolutionsNonDominees; i++){
//        afficher_solution(result[i]);
//    }

//    Solution* result = approche_scalaire(instance, (ScalarizationParams){INSERTION, 0.01}, &nombreDeSolutions);
    Solution* result = approche_pareto(instance, (ParetoParams){INSERTION, 10000}, &nombreDeSolutions);
    //  Les solutions apres filtrage offline :
    printf("Les solutions apres filtrage offline :\n");
    printf("Nombre de solutions : %d\n", nombreDeSolutions);

    for(int i =0; i< nombreDeSolutions; i++){
        afficher_solution(result[i]);
    }
    return 0;
}