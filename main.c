#include <stdio.h>
#include "lib.h"
#include "string.h"

// Liste des fichiers d'instances
const char* instances[] = {
    "3_3_01.txt",
    "7_5_01.txt",
    "20_10_01.txt",
    "20_20_01.txt",
    "30_10_01.txt",
    "30_20_01.txt",
    "50_10_01.txt",
    "50_20_01.txt"
};
const int nb_instances = 8;

Instance *getInstanceByName(const char* instanceName) {
    const char* basePath = "C:\\Users\\Naku\\Desktop\\Naku_desktop\\EILCO\\ING3\\problemes_complexes\\tp1\\instances\\";

    char* fullPath = malloc(strlen(basePath) + strlen(instanceName) + 1);
    strcpy(fullPath, basePath);
    strcat(fullPath, instanceName);

    Instance *instance = read_instance(fullPath);
    free(fullPath);
    return instance;
}

Instance *getInstance() {
    return getInstanceByName("20_10_01.txt");
}


void test_algorithmes(Instance *instance, int k, double n) {
    printf("\n=== Test des algorithmes ===\n");
    printf("Instance: %d jobs, %d machines\n", instance->nombreDeJobs, instance->nombreDeMachines);
    printf("Nombre d'executions (k): %d\n", k);
    printf("Nombre d'iterations pour marche aleatoire (n): %.0f\n\n", n);

    // Tableau pour stocker les résultats
    // 4 algorithmes × 2 opérations = 8 variantes
    const char* algo_names[] = {"Marche Aleatoire", "Climber First", "Climber Best", "Algo Perso"};
    const char* op_names[] = {"INSERTION", "ECHANGE"};

    for (int algo = 0; algo < 4; algo++) {
        for (int op = 0; op < 2; op++) {
            Operation operation = (op == 0) ? INSERTION : ECHANGE;
            double total_cmax = 0.0;
            double total_evaluations = 0.0;

            for (int exec = 0; exec < k; exec++) {
                Solution sol = {0};
                int nb_evaluations = 0;

                switch (algo) {
                    case 0: // Marche aléatoire
                        sol = marche_aleatoire_op(instance, n, operation);
                        nb_evaluations = (int)n;
                        break;
                    case 1: // Climber first
                        sol = climber_first(instance, operation, &nb_evaluations);
                        break;
                    case 2: // Climber best
                        sol = climber_best(instance, operation, &nb_evaluations);
                        break;
                    case 3: // Algo perso
                        sol = algo_perso(instance, operation, n, &nb_evaluations);
                        break;
                    default:
                        break;
                }

                total_cmax += sol.cmax;
                total_evaluations += nb_evaluations;

                // Libérer la mémoire de la solution
                free(sol.jobOrder);
            }

            double moyenne_cmax = total_cmax / k;
            double moyenne_evaluations = total_evaluations / k;

            printf("%-20s | %-10s | Cout moyen: %10.2f | Evaluations moyennes: %10.0f\n",
                   algo_names[algo], op_names[op], moyenne_cmax, moyenne_evaluations);
        }
    }
    printf("\n");
}

// Fonction principale de test pour toutes les instances
void test_toutes_instances(int k, double n) {
    printf("╔════════════════════════════════════════════════════════════════════════════════╗\n");
    printf("║              TEST DES ALGORITHMES SUR TOUTES LES INSTANCES                     ║\n");
    printf("╚════════════════════════════════════════════════════════════════════════════════╝\n\n");

    for (int i = 0; i < nb_instances; i++) {
        printf("================================================================================\n");
        printf("                      INSTANCE: %s\n", instances[i]);
        printf("================================================================================\n");

        Instance *instance = getInstanceByName(instances[i]);
        if (instance == NULL) {
            printf("Erreur: impossible de charger l'instance %s\n\n", instances[i]);
            continue;
        }

        test_algorithmes(instance, k, n);

        free_instance(instance);
    }

    printf("================================================================================\n");
    printf("                           FIN DES TESTS\n");
    printf("================================================================================\n");
}

int launch_Optimization(){
    Instance *instance = getInstance();

    if(instance == NULL){
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
    int nb_evaluations = 0;
    Solution solution_climb_first_ins = climber_first(instance, INSERTION, &nb_evaluations);
    afficher_solution(solution_climb_first_ins);
    Solution solution_climb_first_ech = climber_first(instance, ECHANGE, &nb_evaluations);
    afficher_solution(solution_climb_first_ech);
    printf("Apres climb first insertion :\n");


    Solution solution_climb_best = climber_best(instance, INSERTION, &nb_evaluations);
    printf("Apres climb best :\n");
    afficher_solution(solution_climb_best);

    stop = clock();

    double time = (double)(stop - start) / CLOCKS_PER_SEC;
    printf("Le calcul a pris %.2f millisecondes\n", time*1000);

//    free(solution);
    free_instance(instance);
    return 1;
}

int main(void) {

    srand(time(NULL));

    int k = 10;        // 10 exécutions par méthode
    double n = 10000;  // 10000 itérations pour marche aléatoire

    test_toutes_instances(k, n);

    return 0;
}