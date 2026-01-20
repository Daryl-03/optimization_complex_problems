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

    int P = 500;
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

    // Differentes valeurs de pas (stride) à tester
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
    free_instance(instance);
}

/**
 * Question 8 : Approche Pareto pour le mFSP
 * Analyse le comportement de l'algorithme Pareto sur 3 instances differentes.
 * Le nombre d'iterations est adapte en fonction de la taille de l'instance.
 */
void question8() {
    printf("=============================================================\n");
    printf("         QUESTION 8 : APPROCHE PARETO\n");
    printf("=============================================================\n\n");

    typedef struct {
        const char *nom;
        int iterations_insertion;
    } InstanceConfig;

    InstanceConfig configs[] = {
        {"7_5_01.txt",50},
        {"20_10_01.txt", 100},
        {"50_10_01.txt", 150}
    };
    int nb_instances = sizeof(configs) / sizeof(configs[0]);

    printf("=============================================================\n");
    printf("         ANALYSE SUR 3 INSTANCES\n");
    printf("=============================================================\n\n");

    for (int inst = 0; inst < nb_instances; inst++) {
        Instance *instance = getInstanceByName(configs[inst].nom);
        if (instance == NULL) {
            printf("Erreur: impossible de charger l'instance %s\n", configs[inst].nom);
            continue;
        }

        printf("-------------------------------------------------------------\n");
        printf("Instance: %s (%d jobs, %d machines)\n",
               configs[inst].nom, instance->nombreDeJobs, instance->nombreDeMachines);
        printf("-------------------------------------------------------------\n\n");

        printf("%-12s | %-12s | %-15s | %-15s\n",
               "Operation", "Iterations", "Solutions ND", "Temps (ms)");
        printf("-------------------------------------------------------------\n");

        {
            clock_t start = clock();
            int nombreSolutionsND = 0;

            Solution *result = approche_pareto(instance,
                (ParetoParams){INSERTION, configs[inst].iterations_insertion}, &nombreSolutionsND);

            clock_t stop = clock();
            double temps_ms = (double)(stop - start) * 1000.0 / CLOCKS_PER_SEC;

            printf("%-12s | %-12d | %-15d | %-15.3f\n",
                   "INSERTION", configs[inst].iterations_insertion, nombreSolutionsND, temps_ms);

            // Afficher quelques solutions non-dominees
            printf("\n  Solutions non-dominees :\n");
            printf("  %-5s | %-15s | %-15s\n", "No", "Cmax", "CT");
            printf("  -----------------------------------------------\n");
            int max_affichage = (nombreSolutionsND < 20) ? nombreSolutionsND : 20;
            for (int i = 0; i < max_affichage; i++) {
                printf("  %-5d | %-15.2f | %-15.2f\n", i + 1,
                       result[i].cout.cmax, result[i].cout.ct);
            }
            if (nombreSolutionsND > 20) {
                printf("  ... et %d autres solutions\n", nombreSolutionsND - 20);
            }
            printf("\n");

            free_tab_solutions(result, nombreSolutionsND);
        }

        free_instance(instance);
    }
}

/**
 * Calcul de l'hypervolume pour un ensemble de solutions par rapport a un point de reference.
 * Utilise une approche simplifiee pour 2 objectifs.
 */
double calcul_hypervolume(Solution *solutions, int n, Cout reference) {
    if (n == 0) return 0.0;

    // Trier les solutions par Cmax croissant
    for (int i = 0; i < n - 1; i++) {
        for (int j = i + 1; j < n; j++) {
            if (solutions[i].cout.cmax > solutions[j].cout.cmax) {
                Solution temp = solutions[i];
                solutions[i] = solutions[j];
                solutions[j] = temp;
            }
        }
    }

    double hypervolume = 0.0;
    double prev_ct = reference.ct;

    for (int i = 0; i < n; i++) {
        if (solutions[i].cout.cmax < reference.cmax && solutions[i].cout.ct < reference.ct) {
            double width = reference.cmax - solutions[i].cout.cmax;
            double height = prev_ct - solutions[i].cout.ct;
            if (height > 0) {
                hypervolume += width * height;
            }
            prev_ct = solutions[i].cout.ct;
        }
    }

    return hypervolume;
}

/**
 * Questions 10 & 11 : Analyse comparative des approches Scalaire et Pareto
 * - Execute chaque algorithme 10 fois sur une instance
 * - Calcule l'hypervolume moyen et l'ecart-type
 * - Affiche les 10 fronts de chaque algorithme avec gnuplot
 */
void question10_11() {
    printf("=============================================================\n");
    printf("    QUESTIONS 10 & 11 : ANALYSE COMPARATIVE\n");
    printf("=============================================================\n\n");

    Instance *instance = getInstanceByName("50_20_01.txt");
    if (instance == NULL) {
        printf("Erreur: impossible de charger l'instance.\n");
        return;
    }

    printf("Instance: %d jobs, %d machines\n\n", instance->nombreDeJobs, instance->nombreDeMachines);
    srand(0); // Pour reproductibilite
    // Point de reference pour l'hypervolume
    Solution sol_ref;
    sol_ref.jobOrder = generate_random_solution(instance->nombreDeJobs);
    sol_ref.cout = eval_mo(instance, sol_ref.jobOrder);

    Cout reference;
    reference.cmax = sol_ref.cout.cmax * 1.1;
    reference.ct = sol_ref.cout.ct * 1.1;

    printf("Point de reference: Cmax=%.2f, CT=%.2f\n\n", reference.cmax, reference.ct);

    int nb_executions = 10;
    double hypervolumes_scalaire[10];
    double hypervolumes_pareto[10];

    // Fichiers pour gnuplot
    FILE *file_scalaire = fopen("fronts_scalaire.dat", "w");
    FILE *file_pareto = fopen("fronts_pareto.dat", "w");

    printf("=============================================================\n");
    printf("         APPROCHE SCALAIRE (10 executions)\n");
    printf("=============================================================\n");
    printf("%-5s | %-15s | %-15s | %-15s\n", "Exec", "Solutions ND", "Hypervolume", "Temps (ms)");
    printf("-------------------------------------------------------------\n");

    for (int exec = 0; exec < nb_executions; exec++) {
        srand(exec);
        clock_t start = clock();
        int nombreSolutionsND = 0;

        Solution *result = approche_scalaire(instance,
            (ScalarizationParams){INSERTION, 0.05}, &nombreSolutionsND);

        clock_t stop = clock();
        double temps_ms = (double)(stop - start) * 1000.0 / CLOCKS_PER_SEC;

        hypervolumes_scalaire[exec] = calcul_hypervolume(result, nombreSolutionsND, reference);

        printf("%-5d | %-15d | %-15.2f | %-15.3f\n",
               exec + 1, nombreSolutionsND, hypervolumes_scalaire[exec], temps_ms);

        // Ecrire les solutions dans le fichier pour gnuplot
        for (int i = 0; i < nombreSolutionsND; i++) {
            fprintf(file_scalaire, "%f %f %d\n", result[i].cout.cmax, result[i].cout.ct, exec);
        }
        fprintf(file_scalaire, "\n\n"); // Separateur pour gnuplot

        free_tab_solutions(result, nombreSolutionsND);
    }

    // Calcul moyenne et ecart-type pour scalaire
    double somme_scalaire = 0.0;
    for (int i = 0; i < nb_executions; i++) {
        somme_scalaire += hypervolumes_scalaire[i];
    }
    double moyenne_scalaire = somme_scalaire / nb_executions;

    double variance_scalaire = 0.0;
    for (int i = 0; i < nb_executions; i++) {
        variance_scalaire += (hypervolumes_scalaire[i] - moyenne_scalaire) *
                             (hypervolumes_scalaire[i] - moyenne_scalaire);
    }
    double ecart_type_scalaire = sqrt(variance_scalaire / nb_executions);

    printf("-------------------------------------------------------------\n");
    printf("Hypervolume moyen : %.2f\n", moyenne_scalaire);
    printf("Ecart-type        : %.2f\n\n", ecart_type_scalaire);

    int max_iterations = 2000;
    printf("=============================================================\n");
    printf("         APPROCHE PARETO (10 executions) %d max iter\n", max_iterations);
    printf("=============================================================\n");
    printf("%-5s | %-15s | %-15s | %-15s\n", "Exec", "Solutions ND", "Hypervolume", "Temps (ms)");
    printf("-------------------------------------------------------------\n");

    for (int exec = 0; exec < nb_executions; exec++) {
        srand(exec);
        clock_t start = clock();
        int nombreSolutionsND = 0;

        Solution *result = approche_pareto(instance,
            (ParetoParams){INSERTION, max_iterations}, &nombreSolutionsND);

        clock_t stop = clock();
        double temps_ms = (double)(stop - start) * 1000.0 / CLOCKS_PER_SEC;

        hypervolumes_pareto[exec] = calcul_hypervolume(result, nombreSolutionsND, reference);

        printf("%-5d | %-15d | %-15.2f | %-15.3f\n",
               exec + 1, nombreSolutionsND, hypervolumes_pareto[exec], temps_ms);

        // Ecrire les solutions dans le fichier pour gnuplot
        for (int i = 0; i < nombreSolutionsND; i++) {
            fprintf(file_pareto, "%f %f %d\n", result[i].cout.cmax, result[i].cout.ct, exec);
        }
        fprintf(file_pareto, "\n\n"); // Separateur pour gnuplot

        free_tab_solutions(result, nombreSolutionsND);
    }

    // Calcul moyenne et ecart-type pour pareto
    double somme_pareto = 0.0;
    for (int i = 0; i < nb_executions; i++) {
        somme_pareto += hypervolumes_pareto[i];
    }
    double moyenne_pareto = somme_pareto / nb_executions;

    double variance_pareto = 0.0;
    for (int i = 0; i < nb_executions; i++) {
        variance_pareto += (hypervolumes_pareto[i] - moyenne_pareto) *
                           (hypervolumes_pareto[i] - moyenne_pareto);
    }
    double ecart_type_pareto = sqrt(variance_pareto / nb_executions);

    printf("-------------------------------------------------------------\n");
    printf("Hypervolume moyen : %.2f\n", moyenne_pareto);
    printf("Ecart-type        : %.2f\n\n", ecart_type_pareto);

    fclose(file_scalaire);
    fclose(file_pareto);

    // Resume comparatif
    printf("=============================================================\n");
    printf("                 RESUME COMPARATIF\n");
    printf("=============================================================\n");
    printf("%-15s | %-20s | %-15s\n", "Algorithme", "Hypervolume moyen", "Ecart-type");
    printf("-------------------------------------------------------------\n");
    printf("%-15s | %-20.2f | %-15.2f\n", "Scalaire", moyenne_scalaire, ecart_type_scalaire);
    printf("%-15s | %-20.2f | %-15.2f\n", "Pareto", moyenne_pareto, ecart_type_pareto);
    printf("=============================================================\n\n");

    // Affichage gnuplot pour les fronts scalaires
    const char *gnuplot_cmd_scalaire =
        "gnuplot -p -e \""
        "set grid; "
        "set xlabel 'Cmax'; "
        "set ylabel 'CT'; "
        "set title 'Fronts de Pareto - Approche Scalaire (10 executions)'; "
        "set autoscale; "
        "set offsets graph 0.05, 0.15, 0.05, 0.05; "
        "set key outside right; "
        "plot 'fronts_scalaire.dat' using ($3==0 ? $1 : 1/0):($3==0 ? $2 : 1/0) with points pt 7 ps 1 lc 1 title 'Exec 1', "
        "'fronts_scalaire.dat' using ($3==1 ? $1 : 1/0):($3==1 ? $2 : 1/0) with points pt 5 ps 1 lc 2 title 'Exec 2', "
        "'fronts_scalaire.dat' using ($3==2 ? $1 : 1/0):($3==2 ? $2 : 1/0) with points pt 9 ps 1 lc 3 title 'Exec 3', "
        "'fronts_scalaire.dat' using ($3==3 ? $1 : 1/0):($3==3 ? $2 : 1/0) with points pt 11 ps 1 lc 4 title 'Exec 4', "
        "'fronts_scalaire.dat' using ($3==4 ? $1 : 1/0):($3==4 ? $2 : 1/0) with points pt 13 ps 1 lc 5 title 'Exec 5', "
        "'fronts_scalaire.dat' using ($3==5 ? $1 : 1/0):($3==5 ? $2 : 1/0) with points pt 6 ps 1 lc 6 title 'Exec 6', "
        "'fronts_scalaire.dat' using ($3==6 ? $1 : 1/0):($3==6 ? $2 : 1/0) with points pt 4 ps 1 lc 7 title 'Exec 7', "
        "'fronts_scalaire.dat' using ($3==7 ? $1 : 1/0):($3==7 ? $2 : 1/0) with points pt 8 ps 1 lc 8 title 'Exec 8', "
        "'fronts_scalaire.dat' using ($3==8 ? $1 : 1/0):($3==8 ? $2 : 1/0) with points pt 10 ps 1 lc 9 title 'Exec 9', "
        "'fronts_scalaire.dat' using ($3==9 ? $1 : 1/0):($3==9 ? $2 : 1/0) with points pt 12 ps 1 lc 10 title 'Exec 10'\"";

    system(gnuplot_cmd_scalaire);

    // Affichage gnuplot pour les fronts pareto
    const char *gnuplot_cmd_pareto =
        "gnuplot -p -e \""
        "set grid; "
        "set xlabel 'Cmax'; "
        "set ylabel 'CT'; "
        "set title 'Fronts de Pareto - Approche Pareto (10 executions - 2000 iter max each)'; "
        "set autoscale; "
        "set offsets graph 0.05, 0.15, 0.05, 0.05; "
        "set key outside right; "
        "plot 'fronts_pareto.dat' using ($3==0 ? $1 : 1/0):($3==0 ? $2 : 1/0) with points pt 7 ps 1 lc 1 title 'Exec 1', "
        "'fronts_pareto.dat' using ($3==1 ? $1 : 1/0):($3==1 ? $2 : 1/0) with points pt 5 ps 1 lc 2 title 'Exec 2', "
        "'fronts_pareto.dat' using ($3==2 ? $1 : 1/0):($3==2 ? $2 : 1/0) with points pt 9 ps 1 lc 3 title 'Exec 3', "
        "'fronts_pareto.dat' using ($3==3 ? $1 : 1/0):($3==3 ? $2 : 1/0) with points pt 11 ps 1 lc 4 title 'Exec 4', "
        "'fronts_pareto.dat' using ($3==4 ? $1 : 1/0):($3==4 ? $2 : 1/0) with points pt 13 ps 1 lc 5 title 'Exec 5', "
        "'fronts_pareto.dat' using ($3==5 ? $1 : 1/0):($3==5 ? $2 : 1/0) with points pt 6 ps 1 lc 6 title 'Exec 6', "
        "'fronts_pareto.dat' using ($3==6 ? $1 : 1/0):($3==6 ? $2 : 1/0) with points pt 4 ps 1 lc 7 title 'Exec 7', "
        "'fronts_pareto.dat' using ($3==7 ? $1 : 1/0):($3==7 ? $2 : 1/0) with points pt 8 ps 1 lc 8 title 'Exec 8', "
        "'fronts_pareto.dat' using ($3==8 ? $1 : 1/0):($3==8 ? $2 : 1/0) with points pt 10 ps 1 lc 9 title 'Exec 9', "
        "'fronts_pareto.dat' using ($3==9 ? $1 : 1/0):($3==9 ? $2 : 1/0) with points pt 12 ps 1 lc 10 title 'Exec 10'\"";

    system(gnuplot_cmd_pareto);

    free(sol_ref.jobOrder);
    free_instance(instance);
}

