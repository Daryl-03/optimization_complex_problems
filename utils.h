//
// Created by Naku on 18/01/2026.
//

#ifndef TP1_UTILS_H
#define TP1_UTILS_H

#include "lib.h"
#include <stdio.h>

bool exporter_solutions_gnuplot(Solution *solutions, int nombreSolutions, const char *path);
bool exporter_solutions_gnuplot_flag(Solution *solutions, bool *dominated, int n, const char *filename);
Instance *getInstance();
#endif //TP1_UTILS_H
