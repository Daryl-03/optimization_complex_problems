//
// Created by Naku on 12/01/2026.
//

#include "multivar.h"

bool isDominated(Solution a, Solution b);

Solution *
filtrage_offline(Instance *instance, Solution *solutions, int nombreDeSolutions, int *nombreSolutionsNonDominees) {
    if (solutions == NULL || nombreDeSolutions == 0) {
        *nombreSolutionsNonDominees = 0;
        printf("Aucune solution a filtrer en offline.\n");
        return NULL;
    }

    *nombreSolutionsNonDominees = nombreDeSolutions;
    for (int i = 0; i < nombreDeSolutions; i++) {
        solutions[i].cout = eval_mo(instance, solutions[i].jobOrder);
    }

    bool dominated[nombreDeSolutions];
    for (int i = 0; i < nombreDeSolutions; i++) {
        dominated[i] = false;
    }

    for (int i = 0; i < nombreDeSolutions; ++i) {
        if (dominated[i]) continue;
        for (int j = 0; j < nombreDeSolutions; ++j) {
            if (i == j || dominated[j]) continue;

            if (isDominated(solutions[i], solutions[j])) {
                dominated[j] = true;
            } else if (solutions[i].cout.cmax == solutions[j].cout.cmax &&
                       solutions[i].cout.ct == solutions[j].cout.ct) {
                if (i < j) {
                    dominated[j] = true;
                }
            }
        }
    }

    for (int i = 0; i < nombreDeSolutions; ++i) {
        if (dominated[i]) {
            (*nombreSolutionsNonDominees)--;
        }
    }

    Solution *nonDominatedSolutions = malloc(((*nombreSolutionsNonDominees)) * sizeof(Solution));
    int index = 0;
    for (int i = 0; i < nombreDeSolutions; i++) {
        if (!dominated[i]) {
            nonDominatedSolutions[index] = solutions[i];

            nonDominatedSolutions[index].jobOrder = malloc(solutions[i].nombreDeJobs * sizeof(int));
            memcpy(nonDominatedSolutions[index].jobOrder, solutions[i].jobOrder, solutions[i].nombreDeJobs * sizeof(int));

            index++;
        }
    }

    exporter_solutions_gnuplot_flag(solutions, dominated, nombreDeSolutions, "solutions_offline.dat");
    char gnuplot_cmd[2048];
    sprintf(gnuplot_cmd,
            "gnuplot -p -e \""
            "set grid; "
            "set xlabel 'Cmax'; "
            "set ylabel 'CT'; "
            "set title 'Solutions - Filtrage Offline - %d solutions'; "
            "set autoscale; "
            "set offsets graph 0.05, 0.05, 0.05, 0.05; "
            "set key outside right; "
            "plot 'solutions_offline.dat' using ($3==1 ? $1 : 1/0):($3==1 ? $2 : 1/0) "
            "with points pt 7 ps 1.5 lc rgb 'blue' title 'Dominated', "
            "'solutions_offline.dat' using ($3==0 ? $1 : 1/0):($3==0 ? $2 : 1/0) "
            "with points pt 7 ps 1.5 lc rgb 'red' title 'Non-dominated'\"",
            nombreDeSolutions);



    system(gnuplot_cmd);
    return nonDominatedSolutions;
}

bool isDominated(Solution a, Solution b) {
    if ((a.cout.cmax <= b.cout.cmax &&
         a.cout.ct <= b.cout.ct) && (a.cout.cmax < b.cout.cmax || a.cout.ct < b.cout.ct))
        return true;
    return false;
}

Solution *
filtrage_online(Instance *instance, Solution *solutions, int nombreDeSolutions, int *nombreSolutionsNonDominees,
                long *nombreComparaisons) {
    Archive *archive = creer_archive();

    FILE *logFile = fopen("solutions_pareto.dat", "w");

    for (int i = 0; i < nombreDeSolutions; i++) {
        solutions[i].cout = eval_mo(instance, solutions[i].jobOrder);
        if (!est_dominee_par_archive(archive, solutions[i], nombreComparaisons)) {
            supprimer_dominees(archive, solutions[i], nombreComparaisons);
            ajouter_solution(archive, solutions[i]);
            fprintf(logFile, "%f %f %f\n", solutions[i].cout.cmax, solutions[i].cout.ct, 0.0);
        }
    }

    *nombreSolutionsNonDominees = archive->size;
    Solution *nonDominatedSolutions = malloc(archive->size * sizeof(Solution));
    Node *current = archive->head;

    int index = 0;
    while (current) {
        nonDominatedSolutions[index] = current->solution;
        nonDominatedSolutions[index].jobOrder = malloc(current->solution.nombreDeJobs * sizeof(int));
        memcpy(nonDominatedSolutions[index].jobOrder, current->solution.jobOrder,
               current->solution.nombreDeJobs * sizeof(int));
        fprintf(logFile, "%f %f %f\n", current->solution.cout.cmax, current->solution.cout.ct, 1.0);
        current = current->next;
        index++;
    }

    current = archive->head;
    while (current) {
        Node *temp = current;
        current = current->next;
        if(temp->solution.jobOrder)
            free(temp->solution.jobOrder);
        free(temp);
    }
    free(archive);

    fclose(logFile);

    char gnuplot_cmd[2048];

    sprintf(gnuplot_cmd,
            "gnuplot -p -e \""
            "set grid; "
            "set xlabel 'Cmax (Makespan)'; "
            "set ylabel 'CT (Total Completion Time)'; "
            "set title 'Analyse du Front de Pareto (%d solutions)'; "

            // Amélioration du Padding : 10% de marge sur chaque côté
            "set offsets graph 0.1, 0.1, 0.1, 0.1; "

            // Force Gnuplot à ne pas coller les axes aux données
            "set autoscale; "

            // Légende à l'extérieur pour ne pas cacher les points
            "set key outside right vertical; "

            // Le PLOT :
            // 1. On dessine le BLEU (Flag 1) en premier, petit (ps 0.6)
            "plot 'solutions_pareto.dat' using ($3==0 ? $1 : 1/0):($3==0 ? $2 : 1/0) "
            "with points pt 7 ps 0.6 lc rgb 'blue' title 'Solutions explorees', "

            // 2. On dessine le ROUGE (Flag 0) par-dessus, gros (ps 1.8)
            "'solutions_pareto.dat' using ($3==1 ? $1 : 1/0):($3==1 ? $2 : 1/0) "
            "with points pt 7 ps 0.8 lc rgb 'red' title 'Front de Pareto final'\"",
            nombreDeSolutions);
    system(gnuplot_cmd);

    return nonDominatedSolutions;
}

Archive *creer_archive() {
    Archive *archive = malloc(sizeof(Archive));
    archive->head = NULL;
    archive->tail = NULL;
    archive->size = 0;
    return archive;
}

void ajouter_solution(Archive *archive, Solution sol) {
    Node *nouveau = malloc(sizeof(Node));

    nouveau->solution.jobOrder = malloc(sol.nombreDeJobs * sizeof(int));
    memcpy(nouveau->solution.jobOrder, sol.jobOrder, sol.nombreDeJobs * sizeof(int));

    nouveau->solution.cout = sol.cout;
    nouveau->solution.nombreDeJobs = sol.nombreDeJobs;

    nouveau->explored = false;

    nouveau->next = NULL;
    nouveau->explored = false;
    if (archive->head == NULL) {

        archive->head = nouveau;
        archive->tail = nouveau;
    } else {

        archive->tail->next = nouveau;
        archive->tail = nouveau;
    }
    archive->size++;
}

void supprimer_dominees(Archive *archive, Solution nouvelle, long *nombreComparaisons) {
    Node **current = &archive->head;
    Node *prev = NULL;

    while (*current) {
        *nombreComparaisons += 1;
        if (isDominated(nouvelle, (*current)->solution)) {
            if ((*current) == archive->tail) {
                archive->tail = prev;
            }
            Node *temp = *current;
            *current = (*current)->next;

            free(temp->solution.jobOrder);
            free(temp);
            archive->size--;
        } else {
            prev = *current;
            current = &(*current)->next;
        }
    }
}

bool est_dominee_par_archive(Archive *archive, Solution sol, long *nombreComparaisons) {
    Node *current = archive->head;
    while (current) {
        *nombreComparaisons += 1;
        if (isDominated(current->solution, sol) || (current->solution.cout.cmax == sol.cout.cmax &&
                                                    current->solution.cout.ct == sol.cout.ct)) {
            return true;
        }
        current = current->next;
    }
    return false;
}

// approche scalaire
Solution *approche_scalaire(Instance *instance, ScalarizationParams params, int *size) {
    int max_steps = (int) (1.0 / params.stride) + 1;
    *size = max_steps;

    Solution *solutions = malloc(max_steps * sizeof(Solution));
    for (int step = 0; step < max_steps; step++) {
        double w1 = step * params.stride;
        double w2 = 1.0 - w1;
        solutions[step] = climber_best_ponder(instance, params.operation, w1, w2);
    }

    Solution *result = filtrage_offline(instance, solutions, max_steps, size);

    free_tab_solutions(solutions, max_steps);
    return result;
}

Solution *approche_pareto(Instance *instance, ParetoParams params, int *size) {
    Archive *archive = creer_archive();
    long nombreComparaisons = 0;

    Solution initial_solution;
    initial_solution.jobOrder = generate_random_solution(instance->nombreDeJobs);
    initial_solution.cout = eval_mo(instance, initial_solution.jobOrder);
    initial_solution.nombreDeJobs = instance->nombreDeJobs;

    FILE *logFile = fopen("solutions_pareto.dat", "w");


    ajouter_solution(archive, initial_solution);
    fprintf(logFile, "%f %f %f\n", initial_solution.cout.cmax, initial_solution.cout.ct, 0.0);

    free(initial_solution.jobOrder);

    for (int i = 0; i < params.max_steps; ++i) {
        printf("iteration %d/%d - Archive size: %d\n", i + 1, params.max_steps, archive->size);
        Solution active_solution;

        Node *current = archive->head;
        while (current) {
            if (!current->explored) {
                active_solution = current->solution;
                current->explored = true;
                break;
            }
            current = current->next;
        }

        if (current == NULL) {
            break;
        }

        Solution *voisins;
        int nombreDeVoisins;
        switch (params.operation) {
            case ECHANGE:
                voisins = voisins_echange(active_solution.jobOrder, instance->nombreDeJobs, &nombreDeVoisins);
                break;
            case INSERTION:
                voisins = voisins_insertion(active_solution.jobOrder, instance->nombreDeJobs, &nombreDeVoisins);
                break;
        }

        for (int j = 0; j < nombreDeVoisins; j++) {
            voisins[j].cout = eval_mo(instance, voisins[j].jobOrder);
            if (!est_dominee_par_archive(archive, voisins[j], &nombreComparaisons)) {
                supprimer_dominees(archive, voisins[j], &nombreComparaisons);
                ajouter_solution(archive, voisins[j]);
                fprintf(logFile, "%f %f %f\n", voisins[j].cout.cmax, voisins[j].cout.ct, 0.0);
            }
        }
        free_tab_solutions(voisins, nombreDeVoisins);
        printf("fin iteration %d/%d - Archive size: %d\n", i + 1, params.max_steps, archive->size);
    }

    // exportation des solutions non dominées
    *size = archive->size;
    Solution *nonDominatedSolutions = malloc(archive->size * sizeof(Solution));
    Node *current = archive->head;
    int index = 0;
    while (current) {
        nonDominatedSolutions[index] = current->solution;
        nonDominatedSolutions[index].jobOrder = malloc(current->solution.nombreDeJobs * sizeof(int));
        memcpy(nonDominatedSolutions[index].jobOrder, current->solution.jobOrder,
               current->solution.nombreDeJobs * sizeof(int));
        fprintf(logFile, "%f %f %f\n", current->solution.cout.cmax, current->solution.cout.ct, 1.0);
        current = current->next;
        index++;
    }

    // libération de l'archive
    current = archive->head;
    while (current) {
        Node *temp = current;
        current = current->next;
        if( temp->solution.jobOrder)
            free(temp->solution.jobOrder);
        free(temp);
    }
    free(archive);

    fclose(logFile);

    const char *gnuplot_cmd =
            "gnuplot -p -e \""
//            "set terminal wxt;"
            "set grid; "
            "set xlabel 'Cmax (Makespan)'; "
            "set ylabel 'CT (Total Completion Time)'; "
            "set title 'Analyse du Front de Pareto'; "

            // Amélioration du Padding : 10% de marge sur chaque côté
            "set offsets graph 0.1, 0.1, 0.1, 0.1; "

            // Force Gnuplot à ne pas coller les axes aux données
            "set autoscale; "

            // Légende à l'extérieur pour ne pas cacher les points
            "set key outside right vertical; "

            // Le PLOT :
            // 1. On dessine le BLEU (Flag 1) en premier, petit (ps 0.6)
            "plot 'solutions_pareto.dat' using ($3==0 ? $1 : 1/0):($3==0 ? $2 : 1/0) "
            "with points pt 7 ps 0.6 lc rgb 'blue' title 'Solutions explorees', "

            // 2. On dessine le ROUGE (Flag 0) par-dessus, gros (ps 1.8)
            "'solutions_pareto.dat' using ($3==1 ? $1 : 1/0):($3==1 ? $2 : 1/0) "
            "with points pt 7 ps 0.8 lc rgb 'red' title 'Front de Pareto final'\"";
    system(gnuplot_cmd);

    return nonDominatedSolutions;
}

Solution climber_best_ponder(Instance *instance, Operation op, double w1, double w2) {
    Solution solution_courante;
    solution_courante.jobOrder = generate_random_solution(instance->nombreDeJobs);
    solution_courante.cout = eval_mo(instance, solution_courante.jobOrder);
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
            Cout couts = eval_mo(instance, voisins[i].jobOrder);
            double cout = w1 * couts.cmax + w2 * couts.ct;
            if (cout < (w1 * solution_courante.cout.cmax + w2 * solution_courante.cout.ct)) {
                memcpy(solution_courante.jobOrder, voisins[i].jobOrder, instance->nombreDeJobs * sizeof(int));

                solution_courante.cout.cmax = couts.cmax;
                solution_courante.cout.ct = couts.ct;

                notStuck = true;
            }
        }
        free_tab_solutions(voisins, nombreDeVoisins);
    } while (notStuck);

    return solution_courante;
}

double calculer_hypervolume(Solution *front, int taille, Cout reference) {
    if (taille == 0) return 0.0;

    // 1. Trier le front par Cmax croissant
    for (int i = 0; i < taille - 1; i++) {
        for (int j = 0; j < taille - i - 1; j++) {
            if (front[j].cout.cmax > front[j+1].cout.cmax) {
                Solution temp = front[j];
                front[j] = front[j+1];
                front[j+1] = temp;
            }
        }
    }

    double hypervolume = 0.0;
    double dernier_cmax = front[0].cout.cmax;

    // 2. Calculer l'aire par "tranches" verticales
    // On calcule l'aire entre le point de référence et le front
    for (int i = 0; i < taille; i++) {
        double largeur = reference.cmax - front[i].cout.cmax;
        double hauteur;

        if (i == 0) {
            hauteur = reference.ct - front[i].cout.ct;
        } else {
            // Pour ne pas recompter ce qui est déjà sous le point précédent
            hauteur = front[i-1].cout.ct - front[i].cout.ct;
        }

        // Aire du rectangle courant
        hypervolume += (reference.cmax - front[i].cout.cmax) * hauteur;
    }

    return hypervolume;
}