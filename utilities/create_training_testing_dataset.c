#include<ss.h>
#include<stdlib.h>
#include<stdio.h>
#include<time.h>

int main(int cc, char *vv[]) {
    uint32_t *array1, *array2;
    char m;
    uint32_t upper_bound, random_index, g, i;
    char *input_csv, *testing_csv, *training_csv;
    uint32_t skip_lines;
    int training_percentage;
    uint32_t input_csv_rows;
    FILE *input_file,*training_file,*testing_file;
    uint32_t divider;
    if (cc != 6) {
        printf("Usage : create_training_testing_dataset.out input_dataset skip_lines training_dataset testing_dataset training_dataset_percentage\n");
        return 0;
    }
    input_csv = vv[1];
    skip_lines = atoi(vv[2]);
    training_csv = vv[3];
    testing_csv = vv[4];
    training_percentage = atoi(vv[5]);
    // count input rows
    input_file = fopen(input_csv, "r");
    if (input_file == NULL) {
        printf("Unable to read %s\n", input_csv);
        return 0;
    }
    input_csv_rows = 0;
    while (1) {
        m = fgetc(input_file);
        if (feof(input_file)) break;
        if(m == '\n') input_csv_rows++;
    }
    input_csv_rows -= skip_lines;
    rewind(input_file);
    array1 = (uint32_t *)malloc(sizeof(uint32_t)*input_csv_rows);
    if (array1 == NULL) {
        fclose(input_file);
        printf("Low memory, unable to create training and testing dataset\n");
        return 0;
    }
    array2 = (uint32_t *)malloc(sizeof(uint32_t)*input_csv_rows);
    if (array2 == NULL) {
        free(array1);
        fclose(input_file);
        printf("Low memory, unable to create training and testing dataset\n");
        return 0;
    }
    for (i = 0; i < input_csv_rows; ++i) array1[i] = i;
    srand(time(0));
    upper_bound = input_csv_rows - 1;
    while (upper_bound > 0) {
        random_index = rand() % upper_bound;
        g = array1[random_index];
        array1[random_index] = array1[upper_bound];
        array1[upper_bound] = g;
        upper_bound--;
    }
    for (i=0; i < input_csv_rows; ++i) array2[array1[i]] = i;
    while (skip_lines > 0) {
        m = fgetc(input_file);
        if (m == '\n') skip_lines--;
    }
    training_file = fopen(training_csv, "w");
    if (training_file == NULL) {
        fclose(input_file);
        free(array1);
        free(array2);
        printf("Unable to create %s\n", training_csv);
        return 0;
    }
    testing_file = fopen(testing_csv, "w");
    if (testing_file == NULL) {
        fclose(input_file);
        fclose(training_file);
        free(array1);
        free(array2);
        printf("Unable to create %s\n", testing_csv);
        return 0;
    }
    divider = input_csv_rows * training_percentage/100;
    printf("Divider : %u", divider);
    divider--;
    printf("Divider : %u", divider);
    printf("Input csv rows : %u", input_csv_rows);
    int index = 0;
    while (1) {
        m = fgetc(input_file);
        if (feof(input_file)) break;
        if (m == '\n') {
            if (array2[index] <= divider) fputc('\n', training_file);
            else fputc('\n', testing_file);
            index++;
            continue;
        }
        if (array2[index] <= divider) fputc(m, training_file);
        else fputc(m, testing_file);
    }
    fclose(input_file);
    fclose(training_file);
    fclose(testing_file);
    printf("%s and %s created\n", training_csv, testing_csv);


}