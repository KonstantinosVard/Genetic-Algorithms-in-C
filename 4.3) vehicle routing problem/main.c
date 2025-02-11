#include "vrp_functions.h"

int main() {
    LOCATIONS locations = bayg29;
    POPULATION old_population, new_population, temporary_population;
    float fitness_score[POPULATION_SIZE];
    float total_fitness[POPULATION_SIZE];
    int generation_cnt = 0;
    float mean, max = 0, min = MAX_FLOAT;

    srand(time(NULL));

    initiate_population(&old_population);
    population_copy(&new_population, old_population);
    // print_population(old_population);

    fitness(old_population, fitness_score, total_fitness, locations);
    // print_fitness(fitness_score);

    stats_fitness(fitness_score, &max, &mean, &min);
    printf("\n%d) max = %.3f, mean = %.3f, min = %.3f", generation_cnt, max, mean, min);
    printf("\n");
    while (generation_cnt < MAX_GENERATIONS) {
        generation_cnt++;
        tournament_selection(&new_population, old_population, fitness_score);
        shuffle_population(&new_population);
        population_copy(&temporary_population, new_population);

        uniform_ordered_crossover(&new_population, temporary_population);

        shuffle_mutation(&new_population);        
        population_copy(&old_population, new_population);
        fitness(new_population, fitness_score, total_fitness, locations);
        stats_fitness(fitness_score, &max, &mean, &min);
        if (isatty(fileno(stdin))) {
            printf("\n%d) max = %.3f, mean = %.3f, min = %.3f", generation_cnt, max, mean, min);
        }

    }

    if (!(isatty(fileno(stdin)))) {
        printf("\n%d) max = %.3f, mean = %.3f, min = %.3f", generation_cnt, max, mean, min);
    }

    printf("\n\n");

    save_best_solution_csv(new_population, bayg29, fitness_score, "best_solution.csv");
    printf("\n\n\n\t\t\t\tBest Order Solution \n");
    print_best_solution(new_population, locations, fitness_score);
    printf("\n");
    if (isatty(fileno(stdin))) {
        system("pause");
    }
}
