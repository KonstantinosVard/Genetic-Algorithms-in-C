#include "knapsack_functions.h"

int main() {
    ITEMS items = default_items;
    POPULATION old_population, new_population, temporary_population;
    int fitness_score[POPULATION_SIZE];
    int generation_cnt = 0, max = 0, min;
    float mean;

    srand(time(NULL));

    initiate_population(&old_population);
    population_copy(&new_population, old_population);

    fitness_skip(old_population, fitness_score, items);
    stats_fitness(fitness_score, &max, &mean, &min);
    printf("\n%d) max = %d, mean = %.3f, min = %d", generation_cnt, max, mean, min);

    while (generation_cnt < MAX_GENERATIONS) {
        generation_cnt++;
        tournament_selection(&new_population, old_population, fitness_score);
        population_copy(&temporary_population, new_population);
        two_point_crossover(&new_population, temporary_population);
        bit_flip_mutation(&new_population);
        population_copy(&old_population, new_population);
        fitness_skip(old_population, fitness_score, items);
        stats_fitness(fitness_score, &max, &mean, &min);
        printf("\n%d) max = %d, mean = %.3f, min = %d", generation_cnt, max, mean, min);
    }

    printf("\n\n\n\t\t\t\tBest Packing Solution \n");
    print_best_solution_skip(old_population, items, fitness_score);
    printf("\n");
    system("pause");
}
