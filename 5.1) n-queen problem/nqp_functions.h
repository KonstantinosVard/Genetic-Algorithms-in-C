#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <unistd.h>

#define TRUE 1
#define FALSE 0
#define MAX_INT 2147483646
#define MAX_SOLUTIONS 10

#define ELITISM_SIZE 200
#define POPULATION_SIZE 2000
#define MAX_GENERATIONS 500
#define CROSSOVER_PROB 1.
#define UNIFORM_CX_PROB 0.25
#define MUTANT_PROB 0.1
#define TOURNAMENT_SIZE 5
#define CHROMOSOME_SIZE 16


struct chromosome {
    unsigned int chr[CHROMOSOME_SIZE];
};

struct pop {
    struct chromosome ind[POPULATION_SIZE];
};


typedef struct chromosome INDIVIDUAL;
typedef struct pop POPULATION;

void fitness(POPULATION population, int *fitness_score);

void initiate_population(POPULATION *population);
void shuffle_population(POPULATION *population);
void print_population(POPULATION population);

void print_fitness(int *fitness_score);
void stats_fitness(int *fitness_score, int *max, float *mean, int *min);

void chromosome_copy(INDIVIDUAL *dest, INDIVIDUAL src);
void population_copy(POPULATION *new_population, POPULATION old_population);

void tournament_selection(POPULATION *new_population, POPULATION old_population, int *fitness_score);
void uniform_ordered_crossover(POPULATION *new_population, POPULATION temporary_population);
void ordered_crossover(POPULATION *new_population, POPULATION temporary_population);

void shuffle_mutation(POPULATION *population);
void inverse_mutation(POPULATION *population);

int best_solutions(int *fitness_score, int *best_indexes, POPULATION population);
void print_best_solutions(POPULATION population, int *fitness_score);
void save_best_solutions_csv(POPULATION population, int *fitness_score, const char *filename);

#endif
