#include "lib.h"

double **create_memorization(const Instance *instance);

Instance *read_instance(const char *path) {
    FILE *fptr;

    fptr = fopen(path, "r");
    if (fptr == NULL) {
        printf("Not able to open the file.");
        return NULL;
    }

    Instance *instance = malloc(sizeof(Instance));

    fscanf(fptr, "%d", &instance->nombreDeJobs);

    fscanf(fptr, "%d", &instance->nombreDeMachines);
    long dump;
    fscanf(fptr, "%ld", &dump);

    instance->jobs = malloc(instance->nombreDeJobs * sizeof(Job));

    for (int i = 0; i < instance->nombreDeJobs; i++) {
        fscanf(fptr, "%d", &instance->jobs[i].id);
        instance->jobs[i].durations = malloc(instance->nombreDeMachines * sizeof(int));
        fscanf(fptr, "%ld", &dump);
        for (int j = 0; j < instance->nombreDeMachines; j++) {
            fscanf(fptr, "%d", &instance->jobs[i].durations[j]);
        }
    }

    fclose(fptr);
    return instance;

}

void free_instance(Instance *instance) {
    for (int i = 0; i < instance->nombreDeJobs; i++) {
        free(instance->jobs[i].durations);
    }
    free(instance->jobs);
    free(instance);
}

void afficher_instance(Instance instance) {
    printf("Nombre de jobs: %d\n", instance.nombreDeJobs);
    printf("Nombre de machines: %d\n", instance.nombreDeMachines);
    for (int i = 0; i < instance.nombreDeJobs; i++) {
        printf("Job %d: ", instance.jobs[i].id);
        for (int j = 0; j < instance.nombreDeMachines; j++) {
            printf("%d ", instance.jobs[i].durations[j]);
        }
        printf("\n");
    }
}

int *generate_random_solution(int nombreDeJobs) {
    int *solution = malloc(sizeof(int) * nombreDeJobs);

    for (int i = 0; i < nombreDeJobs; i++) {
        solution[i] = i;
    }
    shuffle_array(solution, nombreDeJobs);
    return solution;
}

void shuffle_array(int *array, int size) {
    for (int i = size - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        swap_int(&array[i], &array[j]);
    }
}

void swap_int(int *a, int *b) {
    int temp = *a;
    *a = *b;
    *b = temp;
}

void free_memoization(double **memorization, int nombreDeMachines) {
    for (int i = 0; i < nombreDeMachines; i++) {
        free(memorization[i]);
    }
    free(memorization);
}

void print_memoization(double **memorization, int nombreDeJobs, int nombreDeMachines) {
    printf("\nMemoization table :\n");
    for (int i = 0; i < nombreDeMachines; i++) {
        for (int j = 0; j < nombreDeJobs; j++) {
            printf("%.2f ", memorization[i][j]);
        }
        printf("\n");
    }
}

double cout_Cmax_iter(Instance *instance, int *solution) {
    if (solution == NULL) {
        exit(0);
    }
    double **memorization = create_memorization(instance);

    for (int i = 0; i < instance->nombreDeMachines; i++) {
        for (int j = 0; j < instance->nombreDeJobs; j++) {
            int jobId = solution[j];
            if (i == 0 && j == 0) {
                memorization[i][j] = instance->jobs[jobId].durations[i];
            } else if (i == 0) {
                memorization[i][j] = memorization[i][j - 1] + instance->jobs[jobId].durations[i];
            } else if (j == 0) {
                memorization[i][j] = memorization[i - 1][j] + instance->jobs[jobId].durations[i];
            } else {
                memorization[i][j] =
                        instance->jobs[jobId].durations[i] + fmax(memorization[i - 1][j], memorization[i][j - 1]);
            }

        }
    }

    double cout = memorization[instance->nombreDeMachines - 1][instance->nombreDeJobs - 1];
    free_memoization(memorization, instance->nombreDeMachines);
    return cout;
}

double cout_CMax(Instance *instance, int *solution) {
    if (solution == NULL) {
        exit(0);
    }
    double **memorization = create_memorization(instance);
    double cout = temps_fin_tache(instance, solution, instance->nombreDeJobs - 1, instance->nombreDeMachines - 1,
                                  memorization);
    free_memoization(memorization, instance->nombreDeMachines);
    return cout;
}

double **create_memorization(const Instance *instance) {
    double **memorization = malloc(instance->nombreDeMachines * sizeof(double *));

    for (int i = 0; i < instance->nombreDeMachines; i++) {
        memorization[i] = malloc(instance->nombreDeJobs * sizeof(double));
        for (int j = 0; j < instance->nombreDeJobs; ++j) {
            memorization[i][j] = -1;
        }
    }
    return memorization;
}

double
temps_fin_tache(Instance *instance, int *solution, int jobIndexInSolution, int machineIndex, double **memorization) {

//    print_memoization(memorization, instance->nombreDeJobs, instance->nombreDeMachines);

    if (memorization[machineIndex][jobIndexInSolution] != -1) {
        return memorization[machineIndex][jobIndexInSolution];
    }

    if (jobIndexInSolution == 0 && machineIndex == 0) {
        int jobId = solution[jobIndexInSolution];
        memorization[machineIndex][jobIndexInSolution] = instance->jobs[jobId].durations[machineIndex];
        return memorization[machineIndex][jobIndexInSolution];
    }

    double temps_fin_job_precedent = 0;
    if (jobIndexInSolution > 0) {
        temps_fin_job_precedent = temps_fin_tache(instance, solution, jobIndexInSolution - 1, machineIndex,
                                                  memorization);
    }

    double temps_fin_machine_precedente = 0;
    if (machineIndex > 0) {
        temps_fin_machine_precedente = temps_fin_tache(instance, solution, jobIndexInSolution, machineIndex - 1,
                                                       memorization);
    }

    int jobId = solution[jobIndexInSolution];

    memorization[machineIndex][jobIndexInSolution] = instance->jobs[jobId].durations[machineIndex]
                                                     + fmax(temps_fin_job_precedent, temps_fin_machine_precedente);

    return memorization[machineIndex][jobIndexInSolution];
}

void echange(int *solution, int a, int b) {
    solution[a - 1] = solution[a - 1] ^ solution[b - 1];
    solution[b - 1] = solution[a - 1] ^ solution[b - 1];
    solution[a - 1] = solution[b - 1] ^ solution[a - 1];
}

void inserer(Solution *solution, int from, int to) {
    if (from < to) {
        int temp = solution->jobOrder[from - 1];

        for (int i = from - 1; i < to - 1; i++) {
            solution->jobOrder[i] = solution->jobOrder[i + 1];
        }
        solution->jobOrder[to - 1] = temp;
    } else {
        int temp = solution->jobOrder[from - 1];

        for (int i = from - 1; i > to - 1; i--) {
            solution->jobOrder[i] = solution->jobOrder[i - 1];
        }
        solution->jobOrder[to - 1] = temp;
    }
}

Solution afficher_solution(Solution solution) {
    printf("Ordre des jobs : ");
    for (int i = 0; i < solution.nombreDeJobs; ++i) {
        printf("%d ", solution.jobOrder[i]);
    }
    printf("\nCmax : %.2f\n", solution.cmax);
    return solution;
}

Solution marche_aleatoire(Instance *instance, double max_iterations) {
    Solution best_solution;
    best_solution.jobOrder = generate_random_solution(instance->nombreDeJobs);
    best_solution.cmax = cout_CMax(instance, best_solution.jobOrder);
    best_solution.nombreDeJobs = instance->nombreDeJobs;

    Solution solution_courante;
    solution_courante.jobOrder = malloc(instance->nombreDeJobs * sizeof(int));
    solution_courante.cmax = best_solution.cmax;
    solution_courante.nombreDeJobs = instance->nombreDeJobs;
    memcpy(solution_courante.jobOrder, best_solution.jobOrder, instance->nombreDeJobs * sizeof(int));


    while (max_iterations-- > 0) {
        Operation op = rand() % 2;
        int a = (rand() % instance->nombreDeJobs) + 1;
        int b;
        do {
            b = (rand() % instance->nombreDeJobs) + 1;
        } while (b == a);
        switch (op) {
            case ECHANGE:
                echange(solution_courante.jobOrder, a, b);
                break;
            case INSERTION:
                inserer(&solution_courante, a, b);
                break;
        }
        double cout = cout_Cmax_iter(instance, solution_courante.jobOrder);
        if (cout < best_solution.cmax) {
            memcpy(best_solution.jobOrder, solution_courante.jobOrder, instance->nombreDeJobs * sizeof(int));

            best_solution.cmax = cout;
        }
    }

    return best_solution;
}

Solution marche_aleatoire_op(Instance *instance, double max_iterations, Operation op) {
    Solution best_solution;
    best_solution.jobOrder = generate_random_solution(instance->nombreDeJobs);
    best_solution.cmax = cout_CMax(instance, best_solution.jobOrder);
    best_solution.nombreDeJobs = instance->nombreDeJobs;

    while (max_iterations-- > 0) {
        int a = (rand() % instance->nombreDeJobs) + 1;
        int b;
        do {
            b = (rand() % instance->nombreDeJobs) + 1;
        } while (b == a);

        switch (op) {
            case ECHANGE:
                echange(best_solution.jobOrder, a, b);
                break;
            case INSERTION:
                inserer(&best_solution, a, b);
                break;
        }

        double cout = cout_Cmax_iter(instance, best_solution.jobOrder);
        best_solution.cmax = cout;
    }
    return best_solution;
}

Solution climber_first(Instance *instance, Operation op) {
    Solution solution_courante;
    solution_courante.jobOrder = generate_random_solution(instance->nombreDeJobs);
    solution_courante.cmax = cout_CMax(instance, solution_courante.jobOrder);
    solution_courante.nombreDeJobs = instance->nombreDeJobs;
    bool notStuck = true;

    do {
        Solution *voisins;
        int nombreDeVoisins;
        switch (op) {
            case ECHANGE:
                voisins = voisins_echange(solution_courante.jobOrder, instance->nombreDeJobs, &nombreDeVoisins);
                break;
            case INSERTION:
                voisins = voisins_insertion(solution_courante.jobOrder, instance->nombreDeJobs, &nombreDeVoisins);
                break;
        }

        for (int i = 0; i < nombreDeVoisins; i++) {
            double cout = cout_Cmax_iter(instance, voisins[i].jobOrder);
            if (cout < solution_courante.cmax) {
                memcpy(solution_courante.jobOrder, solution_courante.jobOrder, instance->nombreDeJobs * sizeof(int));

                solution_courante.cmax = cout;
                break;
            }
            if (i == nombreDeVoisins - 1) {
                notStuck = false;
            }
        }
        free_tab_solutions(voisins, nombreDeVoisins);
    } while (notStuck);

    return solution_courante;
}

Solution climber_best(Instance *instance, Operation op) {
    Solution solution_courante;
    solution_courante.jobOrder = generate_random_solution(instance->nombreDeJobs);
    solution_courante.cmax = cout_CMax(instance, solution_courante.jobOrder);
    solution_courante.nombreDeJobs = instance->nombreDeJobs;
    bool notStuck = true;

    do {
        notStuck = false;
        Solution *voisins;
        int nombreDeVoisins;
        switch (op) {
            case ECHANGE:
                voisins = voisins_echange(solution_courante.jobOrder, instance->nombreDeJobs, &nombreDeVoisins);
                break;
            case INSERTION:
                voisins = voisins_insertion(solution_courante.jobOrder, instance->nombreDeJobs, &nombreDeVoisins);
                break;
        }

        for (int i = 0; i < nombreDeVoisins; i++) {
            double cout = cout_Cmax_iter(instance, voisins[i].jobOrder);
            if (cout < solution_courante.cmax) {
                memcpy(solution_courante.jobOrder, solution_courante.jobOrder, instance->nombreDeJobs * sizeof(int));

                solution_courante.cmax = cout;
                notStuck = true;
            }
        }
        free_tab_solutions(voisins, nombreDeVoisins);
    } while (notStuck);

    return solution_courante;
}

Solution algo_perso(Instance *instance, Operation op, double max_iter) {
    Solution solution_courante;
    solution_courante.jobOrder = generate_random_solution(instance->nombreDeJobs);
    solution_courante.cmax = cout_CMax(instance, solution_courante.jobOrder);
    solution_courante.nombreDeJobs = instance->nombreDeJobs;
    bool notStuck = true;

    do {
        notStuck = false;
        Solution *voisins;
        int nombreDeVoisins;
        switch (op) {
            case ECHANGE:
                voisins = voisins_echange(solution_courante.jobOrder, instance->nombreDeJobs, &nombreDeVoisins);
                break;
            case INSERTION:
                voisins = voisins_insertion(solution_courante.jobOrder, instance->nombreDeJobs, &nombreDeVoisins);
                break;
        }

        for (int i = 0; i < nombreDeVoisins; i++) {
            double cout = cout_Cmax_iter(instance, voisins[i].jobOrder);
            if (cout < solution_courante.cmax) {
                memcpy(solution_courante.jobOrder, solution_courante.jobOrder, instance->nombreDeJobs * sizeof(int));

                solution_courante.cmax = cout;
                notStuck = true;
            }
        }
        free_tab_solutions(voisins, nombreDeVoisins);
    } while (notStuck);

    return solution_courante;
}

Solution *voisins_echange(int *solution, int nombreDeJobs, int *nombreDeVoisins) {
    *nombreDeVoisins = (nombreDeJobs * (nombreDeJobs - 1)) / 2;
    Solution *voisins = malloc((*nombreDeVoisins) * sizeof(Solution));
    int index = 0;

    for (int i = 0; i < nombreDeJobs; i++) {
        for (int j = i + 1; j < nombreDeJobs; j++) {
            voisins[index].jobOrder = malloc(nombreDeJobs * sizeof(int));
            memcpy(voisins[index].jobOrder, solution, nombreDeJobs * sizeof(int));
            echange(voisins[index].jobOrder, i + 1, j + 1);
            voisins[index].nombreDeJobs = nombreDeJobs;
            index++;
        }
    }

    return voisins;
}

Solution *voisins_insertion(int *solution, int nombreDeJobs, int *nombreDeVoisins) {
    *nombreDeVoisins = nombreDeJobs * (nombreDeJobs - 1) - (nombreDeJobs - 1);
    Solution *voisins = malloc((*nombreDeVoisins) * sizeof(Solution));
    int index = 0;

    for (int i = 0; i < nombreDeJobs; i++) {
        for (int j = 0; j < nombreDeJobs; j++) {
            if (i != j && i - j != -1) {
                voisins[index].jobOrder = malloc(nombreDeJobs * sizeof(int));
                memcpy(voisins[index].jobOrder, solution, nombreDeJobs * sizeof(int));
                inserer(&voisins[index], i + 1, j + 1);
                voisins[index].nombreDeJobs = nombreDeJobs;
                index++;
            }
        }
    }

    return voisins;
}

void free_tab_solutions(Solution *solutions, int nombreDeSolutions) {
    for (int i = 0; i < nombreDeSolutions; i++) {
        free(solutions[i].jobOrder);
    }
    free(solutions);
}