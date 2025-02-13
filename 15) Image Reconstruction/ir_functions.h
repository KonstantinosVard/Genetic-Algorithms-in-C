#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <unistd.h>
#include <omp.h>

#define POPULATION_SIZE 200
#define TOURNAMENT_SIZE 4
#define ELITISM_SIZE 20
#define MAX_GENERATIONS 100000
#define CROSSOVER_PROB 1
#define UNIFORM_CX_PROB 0.5
#define MUTANT_PROB 0.2
#define NORMAL_PROB 0.1
#define INIT_STD 0.1
#define STD_REDUCTION 0.99

typedef float fitness_dtype;

struct image {
    float *pixel;
};

struct pop {
    struct image ind[POPULATION_SIZE];
};

typedef struct image IMAGE;
typedef struct pop POPULATION;

void read_image(const char *filename, IMAGE *img);
void save_image(const char *filename, IMAGE *img);

void init_image_random(IMAGE *image);
void init_population_random(POPULATION *population);

void chromosome_copy(IMAGE *dest, IMAGE src);
void population_copy(POPULATION *new_population, POPULATION old_population);
void shuffle_population(POPULATION *population);

void tournament_selection(POPULATION *new_population, POPULATION old_population, fitness_dtype *fitness_score);
void uniform_ordered_crossover(POPULATION *new_population, POPULATION old_population);
void normal_mutation(POPULATION *population, float std);
float rand_normal(float std);

int best_solution(fitness_dtype *fitness_score);
void MSE_fitness(fitness_dtype *fitness_score, POPULATION population, IMAGE target_img);
void polygon_image_init(IMAGE *image);
void print_image(IMAGE image);

void print_fitness(fitness_dtype *fitness_score);
void stats_fitness(fitness_dtype *fitness_score, fitness_dtype *max, fitness_dtype *mean, fitness_dtype *min);

extern int N_ROWS;
extern int N_COLUMNS;
extern int N_COLOR_CHANNELS;
extern int IMAGE_LENGTH;
