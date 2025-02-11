#include "nqp_functions.h"

int main() {
    POPULATION old_population, new_population, temporary_population;
    int fitness_score[POPULATION_SIZE];
    int generation_cnt = 0, max = 0, min = MAX_INT;
    float mean;

    srand(time(NULL));

    initiate_population(&old_population);
    population_copy(&new_population, old_population);
    // print_population(old_population);

    fitness(old_population, fitness_score);
    // print_fitness(fitness_score);

    stats_fitness(fitness_score, &max, &mean, &min);
    printf("\n%d) max = %d, mean = %.3f, min = %d", generation_cnt, max, mean, min);
    printf("\n");
    while (generation_cnt < MAX_GENERATIONS) {
        generation_cnt++;
        tournament_selection(&new_population, old_population, fitness_score);
        shuffle_population(&new_population);
        population_copy(&temporary_population, new_population);

        uniform_ordered_crossover(&new_population, temporary_population);

        shuffle_mutation(&new_population);        
        population_copy(&old_population, new_population);
        fitness(new_population, fitness_score);
        stats_fitness(fitness_score, &max, &mean, &min);
        if (isatty(fileno(stdin))) {
            printf("\n%d) max = %d, mean = %.3f, min = %d", generation_cnt, max, mean, min);
        }

    }

    if (!(isatty(fileno(stdin)))) {
        printf("\n%d) max = %d, mean = %.3f, min = %d", generation_cnt, max, mean, min);
    }

    printf("\n\n");

    save_best_solutions_csv(new_population, fitness_score, "best_solution.csv");
    printf("\n\n");

    print_best_solutions(new_population, fitness_score);
    printf("\n");
    if (isatty(fileno(stdin))) {
        system("pause");
    }
}
