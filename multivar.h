//
// Created by Naku on 12/01/2026.
//

#ifndef TP1_MULTIVAR_H
#define TP1_MULTIVAR_H

#include "lib.h"
#include "utils.h"

typedef struct Node {
    Solution solution;
    struct Node *next;
    bool explored;
} Node;

typedef struct {
    Node *head;
    Node *tail;
    int size;
} Archive;

typedef struct {
    Operation operation;
    double stride;
} ScalarizationParams;

typedef struct {
    Operation operation;
    int max_steps;
} ParetoParams;

Archive *creer_archive();

void ajouter_solution(Archive *archive, Solution sol);

void supprimer_dominees(Archive *archive, Solution nouvelle, long *nombreComparaisons);

bool est_dominee_par_archive(Archive *archive, Solution sol, long *nombreComparaisons);

Solution *
filtrage_offline(Instance *instance, Solution *solutions, int nombreDeSolutions, int *nombreSolutionsNonDominees);

Solution *
filtrage_online(Instance *instance, Solution *solutions, int nombreDeSolutions, int *nombreSolutionsNonDominees, long *nombreComparaisons);

void filtrage_online_archive(Instance *instance, Solution *solutions, int nombreDeSolutions, Archive *archive);

Solution climber_best_ponder(Instance *instance, Operation op, double w1, double w2);

Solution *approche_scalaire(Instance *instance, ScalarizationParams params, int *size);
Solution *approche_pareto(Instance *instance, ParetoParams params, int *size);



#endif //TP1_MULTIVAR_H