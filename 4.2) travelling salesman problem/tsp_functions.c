#include "tsp_functions.h"


const LOCATIONS bayg29 = {
    .location_list = {
        {0, 1150.0, 1760.0}, {1, 630.0, 1660.0}, {2, 40.0, 2090.0}, {3, 750.0, 1100.0}, {4, 750.0, 2030.0}, {5, 1030.0, 2070.0},
        {6, 1650.0, 650.0}, {7, 1490.0, 1630.0}, {8, 790.0, 2260.0}, {9, 710.0, 1310.0}, {10, 840.0, 550.0}, {11, 1170.0, 2300.0},
        {12, 970.0, 1340.0}, {13, 510.0, 700.0}, {14, 750.0, 900.0}, {15, 1280.0, 1200.0}, {16, 230.0, 590.0}, {17, 460.0, 860.0},
        {18, 1040.0, 950.0}, {19, 590.0, 1390.0}, {20, 830.0, 1770.0}, {21, 490.0, 500.0}, {22, 1840.0, 1240.0}, {23, 1260.0, 1500.0},
        {24, 1280.0, 790.0}, {25, 490.0, 2130.0}, {26, 1460.0, 1420.0}, {27, 1260.0, 1910.0}, {28, 360.0, 1980.0}
    }
};

const LOCATIONS burma14 = {
    .location_list = {
        {0, 16.47, 96.10}, {1, 16.47, 94.44}, {2, 20.09, 92.54}, {3, 22.39, 93.37}, {4, 25.23, 97.24}, {5, 22.00, 96.05}, {6, 20.47, 97.02}, 
        {7, 17.20, 96.29}, {8, 16.30, 97.38}, {9, 14.05, 98.12}, {10, 16.53, 97.38}, {11, 21.52, 95.59}, {12, 19.41, 97.13}, {13, 20.09, 94.55}
    }
};


void print_location_list(LOCATIONS locations){
    for (int i = 0; i < N_OF_LOCATIONS; i++) {
        printf("Location: %d, x_coordinate: %d, y_coordinate: %d\n",
               locations.location_list[i].id,
               locations.location_list[i].x,
               locations.location_list[i].y);
    }    
}


void fitness(POPULATION population, float *fitness_score, LOCATIONS locations) {

    float total_distance;
    float x_difference, y_difference;

    for (int i = 0; i < POPULATION_SIZE; i++) {
        total_distance = 0;
        
        for (int j = 0; j < N_OF_LOCATIONS - 1; j++) {
            x_difference = locations.location_list[population.ind[i].chr[j]].x - locations.location_list[population.ind[i].chr[j+1]].x;
            y_difference = locations.location_list[population.ind[i].chr[j]].y - locations.location_list[population.ind[i].chr[j+1]].y;
            total_distance += hypot(x_difference, y_difference);
        }

        x_difference = locations.location_list[population.ind[i].chr[N_OF_LOCATIONS - 1]].x - locations.location_list[population.ind[i].chr[0]].x;
        y_difference = locations.location_list[population.ind[i].chr[N_OF_LOCATIONS - 1]].y - locations.location_list[population.ind[i].chr[0]].y;
        total_distance += hypot(x_difference, y_difference);

        fitness_score[i] = total_distance;
    }
}


void initiate_population(POPULATION *population)
{
    int temp;
    int random_index;
    for (int i=0; i<POPULATION_SIZE; i++)
    {
        for (int j=0; j<N_OF_LOCATIONS; j++)
            population->ind[i].chr[j] = j;

        for (int j=N_OF_LOCATIONS-1; j>0; j--)
        {
            random_index = rand() % (j+1);
            temp = population->ind[i].chr[j];
            population->ind[i].chr[j] = population->ind[i].chr[random_index];
            population->ind[i].chr[random_index] = temp;
        }
    }
}

void print_population(POPULATION population)
{
    printf("\n");
    for (int i=0; i<POPULATION_SIZE; i++)
    {
        for (int j=0; j<N_OF_LOCATIONS; j++)
            printf("| %d", population.ind[i].chr[j]);
        printf("|\n");
    }
}

void print_fitness(float *fitness_score)
{
    int i;
    for (i=0; i<POPULATION_SIZE; i++)
    {
        printf("|%f ", fitness_score[i]);
        if ((i+1)%20==0)
            printf("|\n");
    }
    if ((i+1)%30==0)
        printf("|");
}

void stats_fitness(float *fitness_score, float *max, float *mean, float *min)
{
    *max = 0.0;
    *min = MAX_FLOAT;
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
    for (int j = 0; j < N_OF_LOCATIONS; j++) {
        dest->chr[j] = src.chr[j];
    }
}


void population_copy(POPULATION *new_population, POPULATION old_population) 
{
    for (int i = 0; i < POPULATION_SIZE; i++) {
        chromosome_copy(&new_population->ind[i], old_population.ind[i]);
    }
}


void tournament_selection(POPULATION *new_population, POPULATION old_population, float *fitness_score)
{
    int chosen_individual;
    float min_fitness, chosen_fitness;
    int winner;
    float elitists[ELITISM_SIZE];
    int elitists_index[ELITISM_SIZE];
    float score;
    int pos;
    
    for (int i = 0; i < ELITISM_SIZE; i++)
    {
        elitists[i] = MAX_FLOAT;
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
        min_fitness = MAX_FLOAT;
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


void ordered_crossover(POPULATION *new_population, POPULATION temporary_population)
{
    int skip;
    int point1, point2;
    int new_point;
    int old_point;

    for (int i=0; i<POPULATION_SIZE; i+=2)
        if (((i+1)<POPULATION_SIZE) && ((float) rand()/RAND_MAX < CROSSOVER_PROB))
        {
            point1 = rand()%(N_OF_LOCATIONS-1);
            point2 = (point1+1) + rand()%(N_OF_LOCATIONS - point1 - 1);

            for (int j=point1; j<=point2; j++)
            {
                new_population->ind[i].chr[j] = temporary_population.ind[i+1].chr[j];
                new_population->ind[i+1].chr[j] = temporary_population.ind[i].chr[j];
            }

            new_point = point2+1;
            if (new_point == N_OF_LOCATIONS)
                new_point = 0;

            for (int j=point2+1; j<(point2+1+N_OF_LOCATIONS); j++)
            {
                skip = FALSE;
                old_point = j;
                if (old_point>=N_OF_LOCATIONS)
                    old_point -= N_OF_LOCATIONS;
    
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
                    if (new_point == N_OF_LOCATIONS)
                        new_point = 0;
                }
            }

            new_point = point2+1;
            if (new_point == N_OF_LOCATIONS)
                new_point = 0;

            for (int j=point2+1; j<(point2+1+N_OF_LOCATIONS); j++)
            {
                skip = FALSE;
                old_point = j;
                if (old_point>=N_OF_LOCATIONS)
                    old_point -= N_OF_LOCATIONS;

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
                        if (new_point == N_OF_LOCATIONS)
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
            point1 = rand()%(N_OF_LOCATIONS-1);
            point2 = (point1+1) + rand()%(N_OF_LOCATIONS - point1 - 1);

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
            point1 = rand() % (N_OF_LOCATIONS - 1);
            point2 = (point1 + 1) + rand() % (N_OF_LOCATIONS - point1 - 1);

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


int best_solution(float *fitness_score){
    int best = -1;
    float min_fitness = MAX_FLOAT;
    for (int i=0; i<POPULATION_SIZE; i++)
        if (fitness_score[i]<min_fitness)
        {
            min_fitness = fitness_score[i];
            best = i;
        }
    return best;
}

void print_best_solution(POPULATION population, const LOCATIONS locations, float *fitness_score){

    int best = best_solution(fitness_score);
    printf("|");
    for (int i=0; i<N_OF_LOCATIONS; i++)
    {
        printf("%d |", locations.location_list[population.ind[best].chr[i]].id);
    }
}


void save_best_solution_csv(POPULATION population, const LOCATIONS locations, float *fitness_score, const char *filename) {
    int best = best_solution(fitness_score);
    FILE *file = fopen(filename, "w");
    if (!file) {
        perror("Error opening file");
        return;
    }
    
    fprintf(file, "Location_ID,X,Y\n");
    
    for (int i = 0; i < N_OF_LOCATIONS; i++) {
        int location_id = locations.location_list[population.ind[best].chr[i]].id;
        float x = locations.location_list[population.ind[best].chr[i]].x;
        float y = locations.location_list[population.ind[best].chr[i]].y;
        fprintf(file, "%d,%.6f,%.6f\n", location_id, x, y);
    }
    
    fclose(file);
    printf("Best solution saved to %s\n", filename);
}