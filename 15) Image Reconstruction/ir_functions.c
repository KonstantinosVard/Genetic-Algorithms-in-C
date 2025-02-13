#include "ir_functions.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

int N_ROWS = 0;
int N_COLUMNS = 0;
int N_COLOR_CHANNELS = 0;
int IMAGE_LENGTH = 0;


void init_image_random(IMAGE *image) {
    image->pixel = (float *)malloc(IMAGE_LENGTH * sizeof(float));
    if (!image->pixel) {
        printf("Memory allocation failed\n");
        exit(1);
    }

    for (int i = 0; i < IMAGE_LENGTH; i++) {
        image->pixel[i] = (float)rand() / RAND_MAX;
    }
}

void init_population_random(POPULATION *population) {
    for (int i = 0; i < POPULATION_SIZE; i++) {
        init_image_random(&population->ind[i]);
    }
}

void print_image(IMAGE image) {
    for (int row = 0; row < N_ROWS; row++) {
        printf("\n");
        for (int column = 0; column < N_COLUMNS; column++) {
            printf("| ");
            for (int color_channel = 0; color_channel < N_COLOR_CHANNELS; color_channel++) {
                // index = row * N_COLUMNS * N_COLOR_CHANNELS + column * N_COLOR_CHANNELS + color_channel
                int index = (row * N_COLUMNS + column) * N_COLOR_CHANNELS + color_channel;
                printf("%.2f ", image.pixel[index]);
            }
        }
        printf("|");
    }
    printf("\n");
}

void MSE_fitness(fitness_dtype *fitness_score, POPULATION population, IMAGE target_img)
{
    #pragma omp parallel for
    for (int i = 0; i < POPULATION_SIZE; i++)
    {
        fitness_dtype mse_score = 0; // Initialize for each thread

        for (int j = 0; j < IMAGE_LENGTH; j++)
        {
            mse_score += pow(population.ind[i].pixel[j] - target_img.pixel[j], 2);
        }

        mse_score /= IMAGE_LENGTH;
        fitness_score[i] = mse_score; // Store the result for each individual
    }
}



void print_fitness(fitness_dtype *fitness_score)
{
    int i;
    for (i=0; i<POPULATION_SIZE; i++)
    {
        printf("|%f ", fitness_score[i]);
        if ((i+1)%8==0)
            printf("|\n");
    }
    printf("|");
}

void stats_fitness(fitness_dtype *fitness_score, fitness_dtype *max, fitness_dtype *mean, fitness_dtype *min)
{
    *max = 0.0;
    *min = IMAGE_LENGTH;
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


void chromosome_copy(IMAGE *dest, IMAGE src) 
{
    for (int j = 0; j < IMAGE_LENGTH; j++) {
        dest->pixel[j] = src.pixel[j];
    }
}


void population_copy(POPULATION *new_population, POPULATION old_population) 
{
    for (int i = 0; i < POPULATION_SIZE; i++) {
        chromosome_copy(&new_population->ind[i], old_population.ind[i]);
    }
}

void shuffle_population(POPULATION *population){
    int random_index;
    IMAGE temp;
    for (int i = POPULATION_SIZE-1; i > 0; i--)
    {
        random_index = rand() % (i+1);
        temp = population->ind[i];
        population->ind[i] = population->ind[random_index];
        population->ind[random_index] = temp;
    }
}

void tournament_selection(POPULATION *new_population, POPULATION old_population, fitness_dtype *fitness_score)
{
    int elitists_index[ELITISM_SIZE];
    int pos;

    fitness_dtype elitists[ELITISM_SIZE];
    fitness_dtype score;

    
    for (int i = 0; i < ELITISM_SIZE; i++)
    {
        elitists[i] = (fitness_dtype) IMAGE_LENGTH;
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
    
    #pragma omp parallel for
    for (int i=0; i<POPULATION_SIZE-ELITISM_SIZE; i++)
    {
        int winner;
        fitness_dtype min_fitness = IMAGE_LENGTH;
        for (int j=0; j<TOURNAMENT_SIZE; j++)

        {
            int chosen_individual = rand()%POPULATION_SIZE;
            fitness_dtype chosen_fitness = fitness_score[chosen_individual];
            if (chosen_fitness < min_fitness)
            {
                min_fitness = chosen_fitness;
                winner = chosen_individual;
            }
        }
        chromosome_copy(&new_population->ind[i], old_population.ind[winner]);
    }

    #pragma omp parallel for
    for (int i = 0; i < ELITISM_SIZE; i++)
    {
        chromosome_copy(&new_population->ind[POPULATION_SIZE - ELITISM_SIZE + i], old_population.ind[elitists_index[i]]);
    }
}


void uniform_ordered_crossover(POPULATION *new_population, POPULATION old_population)
{
    #pragma omp parallel for
    for (int i=0; i<POPULATION_SIZE; i+=2)
        if (((i+1)<POPULATION_SIZE) && ((float) rand()/RAND_MAX < CROSSOVER_PROB))
        {
            for (int j=0; j < IMAGE_LENGTH; j++)
                if ((float) rand()/RAND_MAX < UNIFORM_CX_PROB)
                {
                    new_population->ind[i].pixel[j] = old_population.ind[i+1].pixel[j];
                    new_population->ind[i+1].pixel[j] = old_population.ind[i].pixel[j];
                }
        }
}

float rand_normal(float std) {
    float u1 = ((float)rand() + 1) / ((float)RAND_MAX + 1);  // Uniform(0,1]
    float u2 = (float)rand() / RAND_MAX;  
    float z = sqrt(-2.0 * log(u1)) * cos(2.0 * M_PI * u2);  // Box-Muller transform
    return z * std;
}


void normal_mutation(POPULATION *population, float std){
    // #pragma omp parallel
    for (int i = 0; i < POPULATION_SIZE; i++)
    {
        if ((float) rand()/RAND_MAX < MUTANT_PROB)
            for (int j = 0; j < IMAGE_LENGTH; j++)
                if ((float) rand()/RAND_MAX < NORMAL_PROB)
                    {
                        float z = rand_normal(std);
                        population->ind[i].pixel[j] += z;
                        if (population->ind[i].pixel[j]>=1)
                            population->ind[i].pixel[j]=0.9999;
                        else if (population->ind[i].pixel[j]<=0)
                            population->ind[i].pixel[j]=0.0001;
                    }
    }
}

int best_solution(fitness_dtype *fitness_score){
    int best = -1;
    fitness_dtype min_fitness = IMAGE_LENGTH;
    for (int i=0; i<POPULATION_SIZE; i++)
        if (fitness_score[i]<min_fitness)
        {
            min_fitness = fitness_score[i];
            best = i;
        }
    return best;
}


// Function to read an image and store it in IMAGE struct
void read_image(const char *filename, IMAGE *img) {
    int width, height, channels;

    // Load image using stb_image
    unsigned char *data = stbi_load(filename, &width, &height, &channels, 0);
    
    if (!data) {
        printf("Error loading image: %s\n", filename);
        exit(1);
    }

    // Set global dimensions based on input image
    N_ROWS = height;
    N_COLUMNS = width;
    N_COLOR_CHANNELS = channels;
    IMAGE_LENGTH = N_ROWS * N_COLUMNS * N_COLOR_CHANNELS;

    printf("Loaded image: %d x %d, Channels: %d\n", N_COLUMNS, N_ROWS, N_COLOR_CHANNELS);

    // Allocate memory for the image struct
    img->pixel = (float *)malloc(IMAGE_LENGTH * sizeof(float));
    if (!img->pixel) {
        printf("Memory allocation failed\n");
        stbi_image_free(data);
        exit(1);
    }

    // Convert pixel values to float (0.0 - 1.0) and store in IMAGE struct
    for (int i = 0; i < IMAGE_LENGTH; i++) {
        img->pixel[i] = data[i] / 255.0f;  // Normalize to [0,1]
    }

    // Free the original image buffer
    stbi_image_free(data);
}


void save_image(const char *filename, IMAGE *img) {
    if (!img->pixel) {
        printf("Error: No image data to save.\n");
        return;
    }

    int IMAGE_LENGTH = N_ROWS * N_COLUMNS * N_COLOR_CHANNELS;
    unsigned char *output_data = (unsigned char *)malloc(IMAGE_LENGTH * sizeof(unsigned char));
    if (!output_data) {
        printf("Memory allocation failed for saving image\n");
        return;
    }

    for (int i = 0; i < IMAGE_LENGTH; i++) {
        output_data[i] = (unsigned char)(img->pixel[i] * 255.0f);
    }

    if (stbi_write_png(filename, N_COLUMNS, N_ROWS, N_COLOR_CHANNELS, output_data, N_COLUMNS * N_COLOR_CHANNELS)) {
        printf("Image saved successfully: %s\n", filename);
    } else {
        printf("Error saving image: %s\n", filename);
    }

    free(output_data);
}
