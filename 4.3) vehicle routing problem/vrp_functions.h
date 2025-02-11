#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <unistd.h>

#define TRUE 1
#define FALSE 0
#define MAX_FLOAT 3.40282347e38f

#define ELITISM_SIZE 30
#define N_OF_LOCATIONS 29
#define POPULATION_SIZE 100
#define MAX_GENERATIONS 500
#define CROSSOVER_PROB 1
#define UNIFORM_CX_PROB 0.5
#define MUTANT_PROB 0.2
#define TOURNAMENT_SIZE 3

#define N_OF_VEHICLES 6
#define DEPOT_LOCATION 12
#define CHROMOSOME_SIZE (N_OF_LOCATIONS-1+N_OF_VEHICLES-1) 


struct chromosome {
    unsigned int chr[CHROMOSOME_SIZE];
};

struct pop {
    struct chromosome ind[POPULATION_SIZE];
};

struct location {
    int id;
    float x;
    float y;
};

struct locations {
    struct location location_list[N_OF_LOCATIONS];
};

typedef struct location LOCATION;
typedef struct chromosome INDIVIDUAL;
typedef struct pop POPULATION;
typedef struct locations LOCATIONS;

extern const LOCATIONS bayg29;
// extern const LOCATIONS burma14;

void print_location_list(LOCATIONS locations);
void fitness(POPULATION population, float *fitness_score, float *total_score, LOCATIONS locations);

void initiate_population(POPULATION *population);
void shuffle_population(POPULATION *population);
void print_population(POPULATION population);

void print_fitness(float *fitness_score);
void stats_fitness(float *fitness_score, float *max, float *mean, float *min);

void chromosome_copy(INDIVIDUAL *dest, INDIVIDUAL src);
void population_copy(POPULATION *new_population, POPULATION old_population);

void tournament_selection(POPULATION *new_population, POPULATION old_population, float *fitness_score);
void uniform_ordered_crossover(POPULATION *new_population, POPULATION temporary_population);
void shuffle_mutation(POPULATION *population);
void inverse_mutation(POPULATION *population);

void print_best_solution(POPULATION population, const LOCATIONS locations, float *fitness_score);
int best_solution(float *fitness_score);
void save_best_solution_csv(POPULATION population, const LOCATIONS locations, float *fitness_score, const char *filename);

#endif
