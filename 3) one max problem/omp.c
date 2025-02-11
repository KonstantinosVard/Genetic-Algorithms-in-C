#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define CHROMOSOME_SIZE 1000
#define POPULATION_SIZE 50
#define MAX_GENERATIONS 100
#define TOURNAMENT_SIZE 4
#define MUTANT_PROB 0.1
#define FLIP_PROB (1.0f / CHROMOSOME_SIZE)
#define CROSSOVER_PROB 0.9

struct chromosome {
    unsigned int chr[CHROMOSOME_SIZE];
};

struct pop {
    struct chromosome ind[POPULATION_SIZE];
};

typedef struct chromosome individual;
typedef struct pop population;

void initiate_population(population *p);
void print_population(population p);

void fitness_function(population p, int *fitness_score);
void print_fitness(int *fitness_score);
void stats_fitness(int *fitness, int *max, float *mean, int *min);

void chromosome_copy(individual *dest, individual src);
void population_copy(population *new_population, population old_population);

void tournament_selection(population *new_population, population old_population, int *fitness);
void two_point_crossover(population *new_population, population temporary_population);
void bit_flip_mutation(population *new_population);

int main()
{
    population old_population;
    population new_population;
    population temporary_population;
    int fitness_score[POPULATION_SIZE];
    int generation_cnt;
    int max=0, min;
    float mean;

    srand(time(NULL));

    initiate_population(&old_population);
    population_copy(&new_population, old_population);

    //print_population(old_population);
    fitness_function(old_population, fitness_score);
    //print_fitness(fitness_score);
    stats_fitness(fitness_score, &max, &mean, &min);
    generation_cnt = 0;

    printf("\n%d) max = %d, mean = %.3f, min = %d", generation_cnt, max, mean, min);
    while (generation_cnt < MAX_GENERATIONS && max<CHROMOSOME_SIZE)
    {
        generation_cnt+=1;
        tournament_selection(&new_population, old_population, fitness_score);
        population_copy(&temporary_population, new_population);

        two_point_crossover(&new_population, temporary_population);
        bit_flip_mutation(&new_population);
        population_copy(&old_population, new_population);

        fitness_function(old_population, fitness_score);
        stats_fitness(fitness_score, &max, &mean, &min);
        printf("\n%d) max = %d, mean = %.3f, min = %d", generation_cnt, max, mean, min);
    }
}

void initiate_population(population *p)
{
    for (int i=0; i<POPULATION_SIZE; i++)
        for (int j=0; j<CHROMOSOME_SIZE; j++)
            p->ind[i].chr[j] = rand()%2;
}

void print_population(population p)
{
    for (int i=0; i<POPULATION_SIZE; i++)
    {
        for (int j=0; j<CHROMOSOME_SIZE; j++)
            printf("| %d", p.ind[i].chr[j]);
        printf("\n");
    }
}

void fitness_function(population p, int *fitness_score)
{
    int sum;
    for (int i=0; i<POPULATION_SIZE; i++)
    {
        sum=0;
        for (int j=0; j<CHROMOSOME_SIZE; j++)
            sum += p.ind[i].chr[j];
        fitness_score[i] = sum;
    }
}

void print_fitness(int *fitness_score)
{
    int i;
    for (i=0; i<POPULATION_SIZE; i++)
    {
        printf("|%d ", fitness_score[i]);
        if ((i+1)%20==0)
            printf("|\n");
    }
    if ((i+1)%30==0)
        printf("|");
}

void stats_fitness(int *fitness, int *max, float *mean, int *min)
{
    *max = 0;
    *min = CHROMOSOME_SIZE;
    *mean = 0;

    for (int i = 0; i < POPULATION_SIZE; i++)
    {
        *mean += fitness[i];

        if (fitness[i] > *max) 
        {
            *max = fitness[i];
        }

        if (fitness[i] < *min) 
        {
            *min = fitness[i];
        }
    }

    *mean /= POPULATION_SIZE;
}


void chromosome_copy(individual *dest, individual src) 
{
    for (int j = 0; j < CHROMOSOME_SIZE; j++) {
        dest->chr[j] = src.chr[j];
    }
}


void population_copy(population *new_population, population old_population) 
{
    for (int i = 0; i < POPULATION_SIZE; i++) {
        chromosome_copy(&new_population->ind[i], old_population.ind[i]);
    }
}


void tournament_selection(population *new_population, population old_population, int *fitness)
{
    int chosen_individual;
    int max_fitness, chosen_fitness;
    int winner;

    for (int i=0; i<POPULATION_SIZE; i++)
    {
        max_fitness = -1;
        for (int j=0; j<TOURNAMENT_SIZE; j++)
        {
            chosen_individual = rand()%POPULATION_SIZE;
            chosen_fitness = fitness[chosen_individual];
            if (chosen_fitness > max_fitness)
            {
                max_fitness = chosen_fitness;
                winner = chosen_individual;
            }
        }
        chromosome_copy(&new_population->ind[i], old_population.ind[winner]);
    }
}


void two_point_crossover(population *new_population, population temporary_population)
{

    int point1, point2;
    for (int i=0; i<POPULATION_SIZE; i+=2)
        if ((i+1)>POPULATION_SIZE)
            chromosome_copy(&new_population->ind[i], temporary_population.ind[i]);
        else if ((float) rand()/RAND_MAX < CROSSOVER_PROB)
        {
            point1 = rand()%(CHROMOSOME_SIZE-1);
            point2 = (point1+1) + rand()%(CHROMOSOME_SIZE - point1 - 1);
            for (int j=0; j<CHROMOSOME_SIZE; j++)
            {
                if (j<point1 || j>point2)
                {
                    new_population->ind[i].chr[j] = temporary_population.ind[i].chr[j];
                    new_population->ind[i+1].chr[j] = temporary_population.ind[i+1].chr[j];
                }
                else
                {
                    new_population->ind[i].chr[j] = temporary_population.ind[i+1].chr[j];
                    new_population->ind[i+1].chr[j] = temporary_population.ind[i].chr[j];
                }
            }
        }
}


void bit_flip_mutation(population *new_population)
{
    for (int i=0; i<POPULATION_SIZE; i++)
        if ((float) rand()/RAND_MAX < MUTANT_PROB)
            for (int j=0; j<CHROMOSOME_SIZE; j++)
                if ((float) rand()/RAND_MAX < FLIP_PROB)
                    new_population->ind[i].chr[j] = 1 - new_population->ind[i].chr[j];
}