#include "ir_functions.h"

int main() {
    omp_set_num_threads(omp_get_num_procs());

    float std = INIT_STD;
    IMAGE target_img;
    fitness_dtype fitness[POPULATION_SIZE];
    POPULATION old_population, new_population;
    fitness_dtype mean, max, min;
    int generation_cnt = 0;
    char filename[50];

    srand(time(NULL));

    // Load the target image to get its dimensions
    read_image("Mona_Lisa_head.png", &target_img);

    // Initialize a random image with the same dimensions as the target image
    init_population_random(&old_population);
    init_population_random(&new_population);

    MSE_fitness(fitness, old_population, target_img);

    stats_fitness(fitness, &max, &mean, &min);
    printf("\n%d) max = %.3f, mean = %.3f, min = %.3f", generation_cnt, max, mean, min);
    printf("\n");

    while (generation_cnt < MAX_GENERATIONS) {
        if (generation_cnt%500 == 0){
            sprintf(filename, "generation_%d.png", generation_cnt); // Create filename with generation count
            save_image(filename, &old_population.ind[best_solution(fitness)]);
            std *= STD_REDUCTION;
        }

        generation_cnt++;
        tournament_selection(&new_population, old_population, fitness);
        shuffle_population(&new_population);
        uniform_ordered_crossover(&old_population, new_population);
        normal_mutation(&old_population, std);
        MSE_fitness(fitness, old_population, target_img);

        stats_fitness(fitness, &max, &mean, &min);
        printf("\n%d) max = %.3f, mean = %.3f, min = %.3f", generation_cnt, max, mean, min);
    }

    save_image("best_solution.png", &old_population.ind[best_solution(fitness)]);

    // Free allocated memory
    for (int i = 0; i<POPULATION_SIZE; i++)
    {
        free(old_population.ind[i].pixel);
        free(new_population.ind[i].pixel);
    }

    free(target_img.pixel);

    if (isatty(fileno(stdin))) {
        printf("\nPress any key to continue...\n");
        getchar();
    }
}