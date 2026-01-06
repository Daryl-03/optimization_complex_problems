//
// Created by Naku on 07/10/2025.
//

#ifndef TP1_LIB_H
#define TP1_LIB_H

#include "string.h"
#include "stdio.h"
#include "stdlib.h"
#include "time.h"
#include "math.h"
#include "stdbool.h"



typedef struct job {
    int id;
    int* durations;
} Job;

typedef struct instance {
    int nombreDeJobs;
    int nombreDeMachines;
    Job* jobs;
} Instance;

typedef struct solution {
    int* jobOrder;
    double cmax;
    int nombreDeJobs;
} Solution;

typedef enum operation{
    ECHANGE,
    INSERTION
} Operation;

Instance* read_instance(const char* path);

void free_instance(Instance* instance);

void afficher_instance(Instance instance);

int* generate_random_solution(int nombreDeJobs);

void swap_int (int *a, int *b);

void shuffle_array(int* array, int size);

double cout_CMax(Instance* instance, int* solution);

double temps_fin_tache(Instance* instance, int* solution, int jobIndex, int machineIndex, double** memorization);

void echange(int* solution, int a, int b);

void inserer(Solution* solution, int from, int to);

Solution marche_aleatoire(Instance* instance, double max_iterations);

Solution marche_aleatoire_op(Instance *instance, double max_iterations, Operation op);

Solution creer_solution(int nombreDeJobs);

Solution afficher_solution(Solution solution);

Solution climber_first(Instance *instance, Operation op);
Solution climber_best(Instance *instance, Operation op);
Solution algo_perso(Instance *instance, Operation op, double max_iter);


Solution * voisins_echange(int* solution, int nombreDeJobs, int* nombreDeVoisins);
Solution * voisins_insertion(int* solution, int nombreDeJobs, int* nombreDeVoisins);

void free_tab_solutions(Solution* solutions, int nombreDeSolutions);

// test functions

void test_inserer();
#endif //TP1_LIB_H
