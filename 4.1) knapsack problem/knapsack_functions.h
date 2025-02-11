#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define LIST_SIZE 22
#define POPULATION_SIZE 50
#define WEIGHT_LIMIT 400
#define MAX_GENERATIONS 50
#define CROSSOVER_PROB 0.9
#define MUTANT_PROB 0.1
#define TOURNAMENT_SIZE 3
#define FLIP_PROB (1.0f/LIST_SIZE)

struct chromosome {
    unsigned int chr[LIST_SIZE];
};

struct pop {
    struct chromosome ind[POPULATION_SIZE];
};

struct item {
    char name[50];
    int weight;
    int value;
};

struct items {
    struct item item_list[LIST_SIZE];
};

typedef struct item ITEM;
typedef struct chromosome INDIVIDUAL;
typedef struct pop POPULATION;
typedef struct items ITEMS;

extern const ITEMS default_items;

void print_item_list(ITEMS items);
void fitness_punish(POPULATION population, int *fitness, ITEMS items);
void fitness_skip(POPULATION population, int *fitness, ITEMS items);

void initiate_population(POPULATION *p);
void print_population(POPULATION p);

void print_fitness(int *fitness_score);
void stats_fitness(int *fitness, int *max, float *mean, int *min);

void chromosome_copy(INDIVIDUAL *dest, INDIVIDUAL src);
void population_copy(POPULATION *new_population, POPULATION old_population);

void tournament_selection(POPULATION *new_population, POPULATION old_population, int *fitness);
void two_point_crossover(POPULATION *new_population, POPULATION temporary_population);
void bit_flip_mutation(POPULATION *new_population);

void print_best_solution_skip(POPULATION population, const ITEMS items, int *fitness);
int best_solution(int *fitness);

#endif
