#include "vrp_functions.h"


const LOCATIONS bayg29 = {
    .location_list = {
        {0, 1150.0, 1760.0}, {1, 630.0, 1660.0}, {2, 40.0, 2090.0}, {3, 750.0, 1100.0}, {4, 750.0, 2030.0}, {5, 1030.0, 2070.0},
        {6, 1650.0, 650.0}, {7, 1490.0, 1630.0}, {8, 790.0, 2260.0}, {9, 710.0, 1310.0}, {10, 840.0, 550.0}, {11, 1170.0, 2300.0},
        {12, 970.0, 1340.0}, {13, 510.0, 700.0}, {14, 750.0, 900.0}, {15, 1280.0, 1200.0}, {16, 230.0, 590.0}, {17, 460.0, 860.0},
        {18, 1040.0, 950.0}, {19, 590.0, 1390.0}, {20, 830.0, 1770.0}, {21, 490.0, 500.0}, {22, 1840.0, 1240.0}, {23, 1260.0, 1500.0},
        {24, 1280.0, 790.0}, {25, 490.0, 2130.0}, {26, 1460.0, 1420.0}, {27, 1260.0, 1910.0}, {28, 360.0, 1980.0}
    }
};

// const LOCATIONS burma14 = {
//     .location_list = {
//         {0, 16.47, 96.10}, {1, 16.47, 94.44}, {2, 20.09, 92.54}, {3, 22.39, 93.37}, {4, 25.23, 97.24}, {5, 22.00, 96.05}, {6, 20.47, 97.02}, 
//         {7, 17.20, 96.29}, {8, 16.30, 97.38}, {9, 14.05, 98.12}, {10, 16.53, 97.38}, {11, 21.52, 95.59}, {12, 19.41, 97.13}, {13, 20.09, 94.55}
//     }
// };


void print_location_list(LOCATIONS locations){
    for (int i = 0; i < N_OF_LOCATIONS; i++) {
        printf("Location: %d, x_coordinate: %d, y_coordinate: %d\n",
               locations.location_list[i].id,
               locations.location_list[i].x,
               locations.location_list[i].y);
    }    
}


void fitness(POPULATION population, float *fitness_score, float *total_score, LOCATIONS locations) {

    float vehicle_distance;
    float x_difference, y_difference;
    float max;
    float total = 0;

    for (int i = 0; i < POPULATION_SIZE; i++) {

        max = 0;

        for (int j = -1; j <= CHROMOSOME_SIZE ; j++) {
            if (j==-1 || population.ind[i].chr[j]>=N_OF_LOCATIONS-1)
            {
                x_difference = locations.location_list[population.ind[i].chr[DEPOT_LOCATION]].x - locations.location_list[population.ind[i].chr[j+1]].x;
                y_difference = locations.location_list[population.ind[i].chr[DEPOT_LOCATION]].y - locations.location_list[population.ind[i].chr[j+1]].y;
                vehicle_distance = hypot(x_difference, y_difference);
            }
            else if (j==CHROMOSOME_SIZE || population.ind[i].chr[j+1]>=N_OF_LOCATIONS-1)
            {
                x_difference = locations.location_list[population.ind[i].chr[j]].x - locations.location_list[population.ind[i].chr[DEPOT_LOCATION]].x;
                y_difference = locations.location_list[population.ind[i].chr[j]].y - locations.location_list[population.ind[i].chr[DEPOT_LOCATION]].y;
                vehicle_distance += hypot(x_difference, y_difference);

                total += vehicle_distance;

                if (vehicle_distance > max)
                    max = vehicle_distance;
            }

            else 
            {
                x_difference = locations.location_list[population.ind[i].chr[j]].x - locations.location_list[population.ind[i].chr[j+1]].x;
                y_difference = locations.location_list[population.ind[i].chr[j]].y - locations.location_list[population.ind[i].chr[j+1]].y;
                vehicle_distance += hypot(x_difference, y_difference);
            }
        }

        fitness_score[i] = max;
        total_score[i] = total;

    }
}


void initiate_population(POPULATION *population)
{
    int temp;
    int random_index;
    for (int i=0; i<POPULATION_SIZE; i++)
    {
        for (int j=0; j<DEPOT_LOCATION; j++)
            population->ind[i].chr[j] = j;

        for (int j=DEPOT_LOCATION; j<CHROMOSOME_SIZE; j++)
            population->ind[i].chr[j] = j+1;

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

void print_fitness(float *fitness_score)
{
    int i;
    for (i=0; i<POPULATION_SIZE; i++)
    {
        printf("|%f ", fitness_score[i]);
        if ((i+1)%10==0)
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
    int index;
    printf("| DEPOT |");
    for (int i=0; i<CHROMOSOME_SIZE; i++)
    {
        index = population.ind[best].chr[i];
        if (index <=N_OF_LOCATIONS - 1) 
            printf(" %d |", locations.location_list[population.ind[best].chr[i]].id);
        else
            printf(" DEPOT |");
    }
    printf(" DEPOT |");

}


void save_best_solution_csv(POPULATION population, const LOCATIONS locations, float *fitness_score, const char *filename) {
    int best = best_solution(fitness_score);
    int location_id;
    float x, y;

    FILE *file = fopen(filename, "w");
    if (!file) {
        perror("Error opening file");
        return;
    }
    
    fprintf(file, "Location_ID,X,Y\n");
    
    for (int i = -1; i <= CHROMOSOME_SIZE; i++) {
        if (i == -1 || i == CHROMOSOME_SIZE || population.ind[best].chr[i]>N_OF_LOCATIONS-1)
        {
            location_id = locations.location_list[DEPOT_LOCATION].id;
            x = locations.location_list[DEPOT_LOCATION].x;
            y = locations.location_list[DEPOT_LOCATION].y;
        }
        else 
        {
            location_id = locations.location_list[population.ind[best].chr[i]].id;
            x = locations.location_list[population.ind[best].chr[i]].x;
            y = locations.location_list[population.ind[best].chr[i]].y;
        }
        fprintf(file, "%d,%.6f,%.6f\n", location_id, x, y);
    }
    
    fclose(file);
    printf("Best solution saved to %s\n", filename);
}