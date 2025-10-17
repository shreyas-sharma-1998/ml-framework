#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main() {
    FILE *f = fopen("train_data.csv", "w");
    if (!f) return 1;
    srand(time(0));
    for (int x = -350; x <= 350; x++) {
        double noise = (rand() % 1000 - 500) / 500.0; // between -1 and +1
        //double noise = 0.0;
        double y = 2 * x + 5 + noise;
        fprintf(f, "%d,%.4f\n", x, y);
    }
    fclose(f);
    printf("Data written to train_data.csv\n");
    f = fopen("test_data.csv", "w");
    if (!f) return 1;
    srand(time(0));
    for (int x = 351; x <= 700; x++) {
        double noise = (rand() % 1000 - 500) / 500.0; // between -1 and +1
        //double noise = 0.0;
        double y = 2 * x + 5 + noise;
        fprintf(f, "%d,%.4f\n", x, y);
    }
    fclose(f);
    printf("Data written to test_data.csv\n");
    return 0;
}