#include "knapsack_functions.h"


const ITEMS default_items = {
    .item_list = {
        {"map", 9, 150}, {"compass", 13, 35}, {"water", 153, 200}, {"sandwich", 50, 160},
        {"glucose", 15, 60}, {"tin", 68, 45}, {"banana", 27, 60}, {"apple", 39, 40},
        {"cheese", 23, 30}, {"beer", 52, 10}, {"suntan cream", 11, 70}, {"camera", 32, 30},
        {"T-shirt", 24, 15}, {"trousers", 48, 10}, {"umbrella", 73, 40}, 
        {"waterproof trousers", 42, 70}, {"waterproof overclothes", 43, 75}, 
        {"note-case", 22, 80}, {"sunglasses", 7, 20}, {"towel", 18, 12}, 
        {"socks", 4, 50}, {"book", 30, 10}   
    }
};

void print_item_list(ITEMS items){
    for (int i = 0; i < LIST_SIZE; i++) {
        printf("Item: %s, Weight: %d, Value: %d\n",
               items.item_list[i].name,
               items.item_list[i].weight,
               items.item_list[i].value);
    }    
}

void fitness_punish(POPULATION p, int *fitness_score, const ITEMS items){

    int value, weight;

    for (int i=0; i<POPULATION_SIZE; i++)
    {
        value = 0;
        weight = 0;
        for (int j=0; j<LIST_SIZE; j++)
        {
            value += p.ind[i].chr[j]*items.item_list[j].value;
            weight += p.ind[i].chr[j]*items.item_list[j].weight;
        }
        if (weight>WEIGHT_LIMIT)
            fitness_score[i] = 0;
        else 
            fitness_score[i] = value;
    }
}

void fitness_skip(POPULATION p, int *fitness_score, const ITEMS items){

    int value, weight;

    for (int i=0; i<POPULATION_SIZE; i++)
    {
        value = 0;
        weight = 0;
        for (int j=0; j<LIST_SIZE; j++)
            if (weight + items.item_list[j].weight < WEIGHT_LIMIT)
            {
                value += p.ind[i].chr[j]*items.item_list[j].value;
                weight += p.ind[i].chr[j]*items.item_list[j].weight;
            }
        fitness_score[i] = value;
    }
}

void initiate_population(POPULATION *p)
{
    for (int i=0; i<POPULATION_SIZE; i++)
        for (int j=0; j<LIST_SIZE; j++)
            p->ind[i].chr[j] = rand()%2;
}

void print_population(POPULATION p)
{
    for (int i=0; i<POPULATION_SIZE; i++)
    {
        for (int j=0; j<LIST_SIZE; j++)
            printf("| %d", p.ind[i].chr[j]);
        printf("\n");
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
    *min = LIST_SIZE;
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


void chromosome_copy(INDIVIDUAL *dest, INDIVIDUAL src) 
{
    for (int j = 0; j < LIST_SIZE; j++) {
        dest->chr[j] = src.chr[j];
    }
}


void population_copy(POPULATION *new_population, POPULATION old_population) 
{
    for (int i = 0; i < POPULATION_SIZE; i++) {
        chromosome_copy(&new_population->ind[i], old_population.ind[i]);
    }
}


void tournament_selection(POPULATION *new_population, POPULATION old_population, int *fitness)
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


void two_point_crossover(POPULATION *new_population, POPULATION temporary_population)
{

    int point1, point2;
    for (int i=0; i<POPULATION_SIZE; i+=2)
        if ((i+1)>POPULATION_SIZE)
            chromosome_copy(&new_population->ind[i], temporary_population.ind[i]);
        else if ((float) rand()/RAND_MAX < CROSSOVER_PROB)
        {
            point1 = rand()%(LIST_SIZE-1);
            point2 = (point1+1) + rand()%(LIST_SIZE - point1 - 1);
            for (int j=0; j<LIST_SIZE; j++)
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


void bit_flip_mutation(POPULATION *new_population)
{
    for (int i=0; i<POPULATION_SIZE; i++)
        if ((float) rand()/RAND_MAX < MUTANT_PROB)
            for (int j=0; j<LIST_SIZE; j++)
                if ((float) rand()/RAND_MAX < FLIP_PROB)
                    new_population->ind[i].chr[j] = 1 - new_population->ind[i].chr[j];
}

int best_solution(int *fitness){
    int best = -1;
    int max_fitness = -1;
    for (int i=0; i<POPULATION_SIZE; i++)
        if (fitness[i]>max_fitness)
        {
            max_fitness = fitness[i];
            best = i;
        }
    return best;
}

void print_best_solution_skip(POPULATION population, const ITEMS items, int *fitness){

    int best = best_solution(fitness);
    int weight = 0;
    int value = 0;

    for (int i=0; i<LIST_SIZE; i++)
    {
        if (weight + items.item_list[i].weight < WEIGHT_LIMIT) 
        {
            weight += population.ind[best].chr[i] * items.item_list[i].weight;
            value += population.ind[best].chr[i] * items.item_list[i].value;

            printf("\nItem: %-23s Weight: %-5d Value: %-5d Total Weight: %-5d Total Value: %-5d", 
                    items.item_list[i].name, 
                    items.item_list[i].weight, 
                    items.item_list[i].value, 
                    weight, 
                    value);
        }
    }
}