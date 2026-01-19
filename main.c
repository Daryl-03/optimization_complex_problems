#include <stdio.h>
#include "lib.h"
#include "string.h"
#include "multivar.h"
#include "analyse.h"


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

    // Question 5 : Filtrage Offline avec 500 solutions aleatoires
//    question5();

    // Question 6 : Filtrage Online avec analyse des comparaisons et temps
//    question6();

    // Question 7 : Approche Scalaire avec analyse du pas
    question7();

    return 0;
}