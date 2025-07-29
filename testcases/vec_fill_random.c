#include<ss.h>
int main() {
    ss_vector *vec;
    vec = ss_vector_create_new(10, NULL);
    ss_vector_fill_random(vec, NULL);
    ss_vector_print(stdout, vec, NULL);
    return 0;
}