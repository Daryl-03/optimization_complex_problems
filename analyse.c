//
// Created by Naku on 19/01/2026.
//

#include "analyse.h"


/**
 * On génere P solutions aleatoires et on applique le filtrage offline.
 * On analyse le nombre de solutions non-dominees et le temps d'execution.
 * On verifie que les solutions restantes sont bien non-dominees entre elles.
 */
void question5() {
    printf("=============================================================\n");
    printf("         QUESTION 5 : FILTRAGE OFFLINE\n");
    printf("=============================================================\n\n");

    Instance *instance = getInstance();
    if (instance == NULL) {
        printf("Erreur: impossible de charger l'instance.\n");
        return;
    }

    printf("Instance chargee : %d jobs, %d machines\n\n", instance->nombreDeJobs, instance->nombreDeMachines);

    int P = 50;
    printf("Generation de %d solutions aleatoires...\n", P);

    Solution *solutions = malloc(P * sizeof(Solution));
    for (int i = 0; i < P; i++) {
        solutions[i].nombreDeJobs = instance->nombreDeJobs;
        solutions[i].jobOrder = generate_random_solution(instance->nombreDeJobs);
    }

    clock_t start = clock();
    int nombreSolutionsNonDominees = 0;

    Solution *result = filtrage_offline(instance, solutions, P, &nombreSolutionsNonDominees);

    clock_t stop = clock();
    double temps_ms = (double)(stop - start) * 1000.0 / CLOCKS_PER_SEC;

    printf("\n=============================================================\n");
    printf("                     RESULTATS\n");
    printf("=============================================================\n");
    printf("Nombre de solutions initiales    : %d\n", P);
    printf("Nombre de solutions non-dominees : %d\n", nombreSolutionsNonDominees);
    printf("Solutions eliminees (dominees)   : %d\n", P - nombreSolutionsNonDominees);
    printf("Temps d'execution                : %.3f ms\n", temps_ms);

    printf("\n=============================================================\n");
    printf("         SOLUTIONS NON-DOMINEES (Front de Pareto)\n");
    printf("=============================================================\n");
    printf("%-5s | %-15s | %-15s\n", "No", "Cmax", "CT");
    printf("-----------------------------------------------\n");

    for (int i = 0; i < nombreSolutionsNonDominees; i++) {
        printf("%-5d | %-15.2f | %-15.2f\n", i + 1, result[i].cout.cmax, result[i].cout.ct);
    }

    printf("\n=============================================================\n");
    printf("                     VERIFICATION\n");
    printf("=============================================================\n");
    printf("Verification que les solutions sont non-dominees entre elles:\n");

    // Verification : aucune solution ne doit dominer une autre dans le resultat
    int erreurs = 0;
    for (int i = 0; i < nombreSolutionsNonDominees; i++) {
        for (int j = 0; j < nombreSolutionsNonDominees; j++) {
            if (i != j) {
                // Verifier si i domine j
                if ((result[i].cout.cmax <= result[j].cout.cmax && result[i].cout.ct <= result[j].cout.ct) &&
                    (result[i].cout.cmax < result[j].cout.cmax || result[i].cout.ct < result[j].cout.ct)) {
                    printf("ERREUR: Solution %d domine solution %d\n", i + 1, j + 1);
                    printf("  Solution %d : Cmax=%.2f, CT=%.2f\n", i + 1, result[i].cout.cmax, result[i].cout.ct);
                    printf("  Solution %d : Cmax=%.2f, CT=%.2f\n", j + 1, result[j].cout.cmax, result[j].cout.ct);
                    erreurs++;
                }
            }
        }
    }

    if (erreurs > 0) {
        printf("ATTENTION: %d erreurs detectees dans le filtrage.\n", erreurs);
    }

    free_tab_solutions(solutions, P);
    free(result);
    free_instance(instance);
}

/**
 * Pour différentes valeurs de P, on génere P solutions aleatoires et on applique le filtrage online.
 * On analyse le nombre de solutions non-dominees, le nombre de comparaisons effectuees et le temps d'execution.
 */
void question6() {
    printf("=============================================================\n");
    printf("         QUESTION 6 : FILTRAGE ONLINE\n");
    printf("=============================================================\n\n");

    Instance *instance = getInstance();
    if (instance == NULL) {
        printf("Erreur: impossible de charger l'instance.\n");
        return;
    }

    printf("Instance chargee : %d jobs, %d machines\n\n", instance->nombreDeJobs, instance->nombreDeMachines);

    int valeurs_P[] = {10, 50, 100, 500, 1000, 5000};
    int nb_valeurs = sizeof(valeurs_P) / sizeof(valeurs_P[0]);

    printf("%-10s | %-15s | %-20s | %-15s\n", "P", "Temps (ms)", "Nb Comparaisons", "Solutions ND");
    printf("----------------------------------------------------------------------\n");

    for (int v = 0; v < nb_valeurs; v++) {
        int P = valeurs_P[v];

        // Generer P solutions aleatoires
        Solution *solutions = malloc(P * sizeof(Solution));
        for (int i = 0; i < P; i++) {
            solutions[i].nombreDeJobs = instance->nombreDeJobs;
            solutions[i].jobOrder = generate_random_solution(instance->nombreDeJobs);
        }

        // Mesurer le temps et les comparaisons
        clock_t start = clock();
        int nombreSolutionsNonDominees = 0;
        long nombreComparaisons = 0;

        Solution *result = filtrage_online(instance, solutions, P, &nombreSolutionsNonDominees, &nombreComparaisons);

        clock_t stop = clock();
        double temps_ms = (double)(stop - start) * 1000.0 / CLOCKS_PER_SEC;

        printf("%-10d | %-15.3f | %-20ld | %-15d\n", P, temps_ms, nombreComparaisons, nombreSolutionsNonDominees);

        // Liberer la memoire
        free_tab_solutions(solutions, P);
        free_tab_solutions(result, nombreSolutionsNonDominees);
    }
    free_instance(instance);
}

/**
 * Question 7 : Approche Scalaire pour le mFSP
 * Analyse le comportement de l'algorithme scalaire en fonction du pas (stride).
 * On teste differentes valeurs de pas et on mesure le temps d'execution,
 * le nombre de solutions generees et le nombre de solutions non-dominees.
 */
void question7() {
    printf("=============================================================\n");
    printf("         QUESTION 7 : APPROCHE SCALAIRE\n");
    printf("=============================================================\n\n");

    Instance *instance = getInstance();
    if (instance == NULL) {
        printf("Erreur: impossible de charger l'instance.\n");
        return;
    }

    printf("Instance chargee : %d jobs, %d machines\n\n", instance->nombreDeJobs, instance->nombreDeMachines);

    // Differentes valeurs de pas (stride) a tester
    double valeurs_stride[] = {0.5, 0.25, 0.1, 0.05, 0.02, 0.01};
    int nb_valeurs = sizeof(valeurs_stride) / sizeof(valeurs_stride[0]);

    printf("=============================================================\n");
    printf("         ANALYSE EN FONCTION DU PAS (STRIDE)\n");
    printf("=============================================================\n");
    printf("%-10s | %-12s | %-15s | %-15s | %-12s\n",
           "Stride", "Nb Etapes", "Solutions ND", "Temps (ms)", "Operation");
    printf("------------------------------------------------------------------------\n");

    // Test avec operation ECHANGE
    for (int v = 0; v < nb_valeurs; v++) {
        double stride = valeurs_stride[v];
        int nb_etapes = (int)(1.0 / stride) + 1;

        clock_t start = clock();
        int nombreSolutionsND = 0;

        Solution *result = approche_scalaire(instance,
            (ScalarizationParams){ECHANGE, stride}, &nombreSolutionsND);

        clock_t stop = clock();
        double temps_ms = (double)(stop - start) * 1000.0 / CLOCKS_PER_SEC;

        printf("%-10.3f | %-12d | %-15d | %-15.3f | %-12s\n",
               stride, nb_etapes, nombreSolutionsND, temps_ms, "ECHANGE");

        free_tab_solutions(result, nombreSolutionsND);
    }

    printf("\n");

    // Test avec operation INSERTION
    for (int v = 0; v < nb_valeurs; v++) {
        double stride = valeurs_stride[v];
        int nb_etapes = (int)(1.0 / stride) + 1;

        clock_t start = clock();
        int nombreSolutionsND = 0;

        Solution *result = approche_scalaire(instance,
            (ScalarizationParams){INSERTION, stride}, &nombreSolutionsND);

        clock_t stop = clock();
        double temps_ms = (double)(stop - start) * 1000.0 / CLOCKS_PER_SEC;

        printf("%-10.3f | %-12d | %-15d | %-15.3f | %-12s\n",
               stride, nb_etapes, nombreSolutionsND, temps_ms, "INSERTION");

        free_tab_solutions(result, nombreSolutionsND);
    }

    printf("\n=============================================================\n");
    printf("                     ANALYSE\n");
    printf("=============================================================\n");
    printf("Parametres de l'approche scalaire:\n");
    printf("  - Stride (pas): determine le nombre d'etapes = 1/stride + 1\n");
    printf("  - Operation: ECHANGE ou INSERTION pour le voisinage\n\n");
    printf("Observations:\n");
    printf("  - Plus le stride est petit, plus on a d'etapes\n");
    printf("  - Plus d'etapes = meilleure exploration mais temps plus long\n");
    printf("  - Le nombre de solutions ND peut ne pas augmenter lineairement\n");
    printf("    car beaucoup de solutions peuvent etre dominees\n");
    printf("=============================================================\n\n");

    // Afficher les solutions pour un stride de reference (0.1)
    printf("=============================================================\n");
    printf("    SOLUTIONS NON-DOMINEES (stride = 0.1, ECHANGE)\n");
    printf("=============================================================\n");

    int nombreSolutionsND = 0;
    Solution *result_final = approche_scalaire(instance,
        (ScalarizationParams){ECHANGE, 0.1}, &nombreSolutionsND);

    printf("\n%-5s | %-15s | %-15s\n", "No", "Cmax", "CT");
    printf("-----------------------------------------------\n");

    for (int i = 0; i < nombreSolutionsND; i++) {
        printf("%-5d | %-15.2f | %-15.2f\n", i + 1,
               result_final[i].cout.cmax, result_final[i].cout.ct);
    }

    free_tab_solutions(result_final, nombreSolutionsND);
    free_instance(instance);
}
