#include "nqp_functions.h"


void fitness(POPULATION population, int *fitness_score) {

    int score;

    for (int i = 0; i < POPULATION_SIZE; i++) {

        score = 0;

        for (int j = 0; j < CHROMOSOME_SIZE-1; j++)
            for (int k = j+1; k < CHROMOSOME_SIZE; k++)
                if (abs(j-k) == abs(population.ind[i].chr[j] - population.ind[i].chr[k]))
                    score++;

        fitness_score[i] = score;
    }
}


void initiate_population(POPULATION *population)
{
    int temp;
    int random_index;
    for (int i=0; i<POPULATION_SIZE; i++)
    {
        for (int j=0; j<CHROMOSOME_SIZE; j++)
            population->ind[i].chr[j] = j;

        for (int j=CHROMOSOME_SIZE - 1; j>0; j--)
        {
            random_index = rand() % (j+1);
            temp = population->ind[i].chr[j];
            population->ind[i].chr[j] = population->ind[i].chr[random_index];
            population->ind[i].chr[random_index] = temp;
        }
    }
}

void shuffle_population(POPULATION *population){
    int random_index;
    INDIVIDUAL temp;
    for (int i = POPULATION_SIZE-1; i > 0; i--)
    {
        random_index = rand() % (i+1);
        temp = population->ind[i];
        population->ind[i] = population->ind[random_index];
        population->ind[random_index] = temp;
    }
}

void print_population(POPULATION population)
{
    printf("\n");
    for (int i=0; i<POPULATION_SIZE; i++)
    {
        for (int j=0; j<CHROMOSOME_SIZE; j++)
            printf("| %d", population.ind[i].chr[j]);
        printf("|\n");
    }
}

void print_fitness(int *fitness_score)
{
    int i;
    for (i=0; i<POPULATION_SIZE; i++)
    {
        printf("|%d ", fitness_score[i]);
        if ((i+1)%10==0)
            printf("|\n");
    }
    if ((i+1)%30==0)
        printf("|");
}

void stats_fitness(int *fitness_score, int *max, float *mean, int *min)
{
    *max = 0;
    *min = MAX_INT;
    *mean = 0.0;

    for (int i = 0; i < POPULATION_SIZE; i++)
    {
        *mean += fitness_score[i];

        if (fitness_score[i] > *max) 
        {
            *max = fitness_score[i];
        }

        if (fitness_score[i] < *min) 
        {
            *min = fitness_score[i];
        }
    }

    *mean /= POPULATION_SIZE;
}


void chromosome_copy(INDIVIDUAL *dest, INDIVIDUAL src) 
{
    for (int j = 0; j < CHROMOSOME_SIZE; j++) {
        dest->chr[j] = src.chr[j];
    }
}


void population_copy(POPULATION *new_population, POPULATION old_population) 
{
    for (int i = 0; i < POPULATION_SIZE; i++) {
        chromosome_copy(&new_population->ind[i], old_population.ind[i]);
    }
}


void tournament_selection(POPULATION *new_population, POPULATION old_population, int *fitness_score)
{
    int chosen_individual;
    int min_fitness, chosen_fitness;
    int winner;
    int elitists[ELITISM_SIZE];
    int elitists_index[ELITISM_SIZE];
    int score;
    int pos;
    
    for (int i = 0; i < ELITISM_SIZE; i++)
    {
        elitists[i] = MAX_INT;
        elitists_index[i] = -1;
    }

    for (int i = 0; i < POPULATION_SIZE; i++) {
        score = fitness_score[i];

        if (score >= elitists[ELITISM_SIZE - 1])
            continue;

        pos = 0;
        while (pos < ELITISM_SIZE && elitists[pos] < score)
            pos++;

        for (int j = ELITISM_SIZE - 1; j > pos; j--)
        {
            elitists[j] = elitists[j - 1];
            elitists_index[j] = elitists_index[j - 1];
        }

        elitists[pos] = score;
        elitists_index[pos] = i;
    }
    

    for (int i=0; i<POPULATION_SIZE-ELITISM_SIZE; i++)
    {
        min_fitness = MAX_INT;
        for (int j=0; j<TOURNAMENT_SIZE; j++)
        {
            chosen_individual = rand()%POPULATION_SIZE;
            chosen_fitness = fitness_score[chosen_individual];
            if (chosen_fitness < min_fitness)
            {
                min_fitness = chosen_fitness;
                winner = chosen_individual;
            }
        }
        chromosome_copy(&new_population->ind[i], old_population.ind[winner]);
    }

    for (int i = 0; i < ELITISM_SIZE; i++)
    {
        chromosome_copy(&new_population->ind[POPULATION_SIZE - ELITISM_SIZE + i], old_population.ind[elitists_index[i]]);
    }
}


void uniform_ordered_crossover(POPULATION *new_population, POPULATION temporary_population)
{
    int skip;
    int val_to_input, inputted_value;
    int new_point;
    int indices[CHROMOSOME_SIZE];
    int pointer;

    for (int i=0; i<POPULATION_SIZE; i+=2)
        if (((i+1)<POPULATION_SIZE) && ((float) rand()/RAND_MAX < CROSSOVER_PROB))
        {
            pointer = -1;
            for (int j=0; j < CHROMOSOME_SIZE; j++)
                if ((float) rand()/RAND_MAX < UNIFORM_CX_PROB)
                {
                    new_population->ind[i].chr[j] = temporary_population.ind[i+1].chr[j];
                    new_population->ind[i+1].chr[j] = temporary_population.ind[i].chr[j];
                    pointer++;
                    indices[pointer] = j;
                }

            if (pointer!=-1)
            {
                for (int n = 0; n <= 1; n++)
                {
                    new_point = 0;
                    for (int j = 0; j < CHROMOSOME_SIZE; j++)
                    {
                        skip = FALSE;

                        for (int k = 0; k < pointer; k++)
                            if (new_point == indices[k])
                                new_point++;

                        val_to_input = temporary_population.ind[i+n].chr[j];

                        for (int k = 0; k < pointer; k++)
                        {
                            inputted_value = new_population->ind[i+n].chr[indices[k]];
                            if (val_to_input == inputted_value)
                            {
                                skip=TRUE;
                                break;
                            }
                        }

                        if (!(skip))
                        {
                            new_population->ind[i+n].chr[new_point] = val_to_input;
                            new_point++;
                        }
                    }  
                }     
            }
        }
}


void ordered_crossover(POPULATION *new_population, POPULATION temporary_population)
{
    int skip;
    int point1, point2;
    int new_point;
    int old_point;

    for (int i=0; i<POPULATION_SIZE; i+=2)
        if (((i+1)<POPULATION_SIZE) && ((float) rand()/RAND_MAX < CROSSOVER_PROB))
        {
            point1 = rand()%(CHROMOSOME_SIZE-1);
            point2 = (point1+1) + rand()%(CHROMOSOME_SIZE - point1 - 1);

            for (int j=point1; j<=point2; j++)
            {
                new_population->ind[i].chr[j] = temporary_population.ind[i+1].chr[j];
                new_population->ind[i+1].chr[j] = temporary_population.ind[i].chr[j];
            }

            new_point = point2+1;
            if (new_point == CHROMOSOME_SIZE)
                new_point = 0;

            for (int j=point2+1; j<(point2+1+CHROMOSOME_SIZE); j++)
            {
                skip = FALSE;
                old_point = j;
                if (old_point>=CHROMOSOME_SIZE)
                    old_point -= CHROMOSOME_SIZE;
    
                for (int k=point1; k<=point2; k++)
                    if (new_population->ind[i].chr[k] == temporary_population.ind[i].chr[old_point])
                    {
                        skip=TRUE;
                        break;
                    }
                
                if (!(skip))
                {
                    new_population->ind[i].chr[new_point] = temporary_population.ind[i].chr[old_point];

                    new_point++;
                    if (new_point == CHROMOSOME_SIZE)
                        new_point = 0;
                }
            }

            new_point = point2+1;
            if (new_point == CHROMOSOME_SIZE)
                new_point = 0;

            for (int j=point2+1; j<(point2+1+CHROMOSOME_SIZE); j++)
            {
                skip = FALSE;
                old_point = j;
                if (old_point>=CHROMOSOME_SIZE)
                    old_point -= CHROMOSOME_SIZE;

                for (int k=point1; k<=point2; k++)
                    if (new_population->ind[i+1].chr[k] == temporary_population.ind[i+1].chr[old_point])
                    {
                        skip=TRUE;
                        break;
                    }

                if (!(skip))
                    {
                        new_population->ind[i+1].chr[new_point] = temporary_population.ind[i+1].chr[old_point];

                        new_point++;
                        if (new_point == CHROMOSOME_SIZE)
                            new_point = 0;
                    }
            }
        }
    
}



void inverse_mutation(POPULATION *population)
{
    int point1, point2, temp;
    for (int i=0; i<POPULATION_SIZE; i++)
        if ((float) rand()/RAND_MAX < MUTANT_PROB)
        {
            point1 = rand()%(CHROMOSOME_SIZE-1);
            point2 = (point1+1) + rand()%(CHROMOSOME_SIZE - point1 - 1);

            for (int j=0; j<((point2 - point1)/2); j++)
            {
                temp = population->ind[i].chr[point1+j];
                population->ind[i].chr[point1+j] = population->ind[i].chr[point2-j];
                population->ind[i].chr[point2-j] = temp;
            }
        }
}

void shuffle_mutation(POPULATION *population)
{
    int point1, point2, temp, random_index;
    
    for (int i = 0; i < POPULATION_SIZE; i++)
    {
        if ((float) rand() / RAND_MAX < MUTANT_PROB)
        {
            point1 = rand() % (CHROMOSOME_SIZE - 1);
            point2 = (point1 + 1) + rand() % (CHROMOSOME_SIZE - point1 - 1);

            for (int j = point2; j > point1; j--)
            {
                random_index = point1 + rand() % (j - point1 + 1);
                temp = population->ind[i].chr[j];
                population->ind[i].chr[j] = population->ind[i].chr[random_index];
                population->ind[i].chr[random_index] = temp;
            }
        }
    }
}


// Find unique individuals with fitness == 0
int best_solutions(int *fitness_score, int *best_indexes, POPULATION population) {
    int count = 0;
    int duplicate;

    for (int i = 0; i < POPULATION_SIZE; i++) {
        if (fitness_score[i] == 0) {
            // If no solutions found yet, it's not a duplicate
            if (count == 0)
                duplicate = FALSE;
            else 
                duplicate = TRUE;

            // Compare chromosomes to ensure uniqueness
            for (int j = 0; j < count; j++) {
                duplicate = TRUE;  // Assume it's duplicate, unless proven otherwise
                for (int k = 0; k < CHROMOSOME_SIZE; k++) {
                    if (population.ind[i].chr[k] != population.ind[best_indexes[j]].chr[k]) {
                        duplicate = FALSE;  // Found a difference, so it's unique
                        break;
                    }
                }
                if (duplicate) break;  // If fully identical, no need to check further
            }

            // If it's a new unique solution, store it
            if (!duplicate) {
                best_indexes[count] = i;
                count++;
                if (count >= MAX_SOLUTIONS) break;  // Prevent overflow
            }
        }
    }
    return count;  // Return number of unique best solutions found
}

// Print all unique best solutions
void print_best_solutions(POPULATION population, int *fitness_score) {
    int best_indexes[MAX_SOLUTIONS];
    int count = best_solutions(fitness_score, best_indexes, population);

    printf("Best Solutions:\n");
    for (int k = 0; k < count; k++) {
        int best = best_indexes[k];
        printf("Solution %d: ", k + 1);
        for (int i = 0; i < CHROMOSOME_SIZE; i++) {
            printf("| %d ", population.ind[best].chr[i]);
        }
        printf("|\n");
    }
}

// Save all unique best solutions in CSV format, with each solution as a column
void save_best_solutions_csv(POPULATION population, int *fitness_score, const char *filename) {
    int best_indexes[MAX_SOLUTIONS];
    int count = best_solutions(fitness_score, best_indexes, population);

    FILE *file = fopen(filename, "w");
    if (!file) {
        perror("Error opening file");
        return;
    }

    // Print CSV header (Solution 1, Solution 2, ...)
    for (int k = 0; k < count; k++) {
        fprintf(file, "Solution %d%s", k + 1, (k == count - 1) ? "\n" : ",");
    }

    // Print each chromosome value as a row
    for (int i = 0; i < CHROMOSOME_SIZE; i++) {
        for (int k = 0; k < count; k++) {
            fprintf(file, "%d%s", population.ind[best_indexes[k]].chr[i], (k == count - 1) ? "\n" : ",");
        }
    }

    fclose(file);
    printf("Best solutions saved to %s\n", filename);
}
