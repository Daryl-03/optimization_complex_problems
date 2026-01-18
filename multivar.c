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
            if (i != j) {
                dominated[j] = isDominated(solutions[i], solutions[j]);
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
            nonDominatedSolutions[index++] = solutions[i];
        }
    }

    exporter_solutions_gnuplot_flag(solutions, dominated, nombreDeSolutions, "solutions_offline.dat");
    const char *gnuplot_cmd =
            "gnuplot -p -e \""
            "set grid; "
            "set xlabel 'Cmax'; "
            "set ylabel 'CT'; "
            "set title 'Solutions - Filtrage Offline'; "
            "set autoscale; "
            "set offsets graph 0.05, 0.05, 0.05, 0.05; "
            "set key outside right; "
            "plot 'solutions_offline.dat' using ($3==0 ? $1 : 1/0):($3==0 ? $2 : 1/0) "
            "with points pt 7 ps 1.5 lc rgb 'red' title 'Non-dominated', "
            "'solutions_offline.dat' using ($3==1 ? $1 : 1/0):($3==1 ? $2 : 1/0) "
            "with points pt 7 ps 1.5 lc rgb 'blue' title 'Dominated'\"";


    system(gnuplot_cmd);

    printf("Nombre de solutions apres filtrage offline : %d\n", *nombreSolutionsNonDominees);
    return nonDominatedSolutions;
}

bool isDominated(Solution a, Solution b) {
    if ((a.cout.cmax <= b.cout.cmax &&
         a.cout.ct <= b.cout.ct) && (a.cout.cmax < b.cout.cmax || a.cout.ct < b.cout.ct))
        return true;
    if (a.cout.cmax == b.cout.cmax &&
        a.cout.ct == b.cout.ct)
        return true;
    return false;
}

Solution *
filtrage_online(Instance *instance, Solution *solutions, int nombreDeSolutions, int *nombreSolutionsNonDominees) {
    Archive *archive = creer_archive();

    for (int i = 0; i < nombreDeSolutions; i++) {
        solutions[i].cout = eval_mo(instance, solutions[i].jobOrder);
        if (!est_dominee_par_archive(archive, solutions[i])) {
            supprimer_dominees(archive, solutions[i]);
            ajouter_solution(archive, solutions[i]);
        }
    }

    *nombreSolutionsNonDominees = archive->size;
    Solution *nonDominatedSolutions = malloc(archive->size * sizeof(Solution));
    Node *current = archive->head;

    int index = 0;
    while (current) {
        nonDominatedSolutions[index++] = current->solution;
        current = current->next;
    }

    current = archive->head;
    while (current) {
        Node *temp = current;
        current = current->next;
        free(temp);
    }
    free(archive);

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

void supprimer_dominees(Archive *archive, Solution nouvelle) {
    Node **current = &archive->head;
    Node *prev = NULL;

    while (*current) {
        if (isDominated(nouvelle, (*current)->solution)) {
            if((*current) == archive->tail) {
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

bool est_dominee_par_archive(Archive *archive, Solution sol) {
    Node *current = archive->head;
    while (current) {
        if ((current->solution.cout.cmax <= sol.cout.cmax &&
             current->solution.cout.ct <= sol.cout.ct) &&
            (current->solution.cout.cmax < sol.cout.cmax || current->solution.cout.ct < sol.cout.ct)) {
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

    printf("Approche scalaire avec %d etapes (stride = %.2f)\n", max_steps, params.stride);
    Solution *solutions = malloc(max_steps * sizeof(Solution));
    for (int step = 0; step < max_steps; step++) {
        double w1 = step * params.stride;
        double w2 = 1.0 - w1;
        solutions[step] = climber_best_ponder(instance, params.operation, w1, w2);
    }
    return filtrage_offline(instance, solutions, max_steps, size);
}

Solution *approche_pareto(Instance *instance, ParetoParams params, int *size) {
    Archive *archive = creer_archive();

    Solution initial_solution;
    initial_solution.jobOrder = generate_random_solution(instance->nombreDeJobs);
    initial_solution.cout = eval_mo(instance, initial_solution.jobOrder);
    initial_solution.nombreDeJobs = instance->nombreDeJobs;

    ajouter_solution(archive, initial_solution);

    for (int i = 0; i < params.max_steps; ++i) {
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


        for (int i = 0; i < nombreDeVoisins; i++) {
            voisins[i].cout = eval_mo(instance, voisins[i].jobOrder);
            if (!est_dominee_par_archive(archive, voisins[i])) {
                supprimer_dominees(archive, voisins[i]);
                ajouter_solution(archive, voisins[i]);
            }
        }
        free_tab_solutions(voisins, nombreDeVoisins);
    }


    *size = archive->size;
    Solution *nonDominatedSolutions = malloc(archive->size * sizeof(Solution));
    Node *current = archive->head;
    int index = 0;
    while (current) {
        nonDominatedSolutions[index++] = current->solution;
        current = current->next;

    }

    current = archive->head;
    while (current) {
        Node *temp = current;
        current = current->next;
        free(temp);
    }
    free(archive);

    return nonDominatedSolutions;
}

Solution climber_best_ponder(Instance *instance, Operation op, double w1, double w2) {
    Solution solution_courante;
    solution_courante.jobOrder = generate_random_solution(instance->nombreDeJobs);
    solution_courante.cout.cmax = cout_CMax(instance, solution_courante.jobOrder);
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
            if (cout < solution_courante.cout.cmax) {
                memcpy(solution_courante.jobOrder, voisins[i].jobOrder, instance->nombreDeJobs * sizeof(int));

                solution_courante.cout.cmax = cout;
                notStuck = true;
            }
        }
        free_tab_solutions(voisins, nombreDeVoisins);
    } while (notStuck);

    return solution_courante;
}