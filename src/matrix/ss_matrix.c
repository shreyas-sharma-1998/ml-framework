#include<ss.h>
#include<stdlib.h>
#include<private_ss.h>

typedef struct __ss_matrix {
    double *data;
    uint32_t rows;
    uint32_t columns;
} ss_matrix;

ss_matrix * ss_matrix_create_new(uint32_t rows, uint32_t columns, ss_error *error) {
    if (error) ss_clear_error(error);
    ss_matrix *matrix;
    if (rows == 0 && columns == 0) {
        ss_set_error(error, "Invalid row count(0) and column count(0)", SS_INVALID_DIMENSIONS);
        return NULL;
    }
    if (rows == 0) {
        ss_set_error(error, "Invalid row count(0)", SS_INVALID_ROW_COUNT);
        return NULL;
    }
    if (columns == 0) {
        ss_set_error(error, "Invalid column count(0)", SS_INVALID_COLUMN_COUNT);
        return NULL;
    }
    matrix = (ss_matrix *)malloc(sizeof(ss_matrix));
    if (matrix == NULL) {
        ss_set_error(error, "Low Memory", SS_LOW_MEMORY);
        return NULL;
    }
    matrix -> data = (double *) malloc(sizeof(double)*rows*columns);
    if (matrix -> data == NULL) {
        ss_set_error(error, "Low Memory", SS_LOW_MEMORY);
        free(matrix);
        return NULL;
    }
    matrix -> rows = rows;
    matrix -> columns = columns;
    return matrix;
}

void __ss_matrix_destroy(ss_matrix **matrix, ss_error *error) {
    if (error) ss_clear_error(error);
    if (*matrix == NULL) {
        ss_set_error(error, "Null Pointer", SS_NULL_POINTER);
        return;
    }
    free((*matrix) -> data);
    free(*matrix);
    *matrix = NULL;
}

void ss_matrix_set(ss_matrix *matrix, uint32_t row, uint32_t column, double value, ss_error *error) {
    if (error) ss_clear_error(error);
   char error_message[4096];
   if (matrix == NULL) {
        ss_set_error(error, "Null Pointer", SS_NULL_POINTER);
        return;
   }
   if (row >= matrix -> rows && column >= matrix -> columns) {
        sprintf(error_message, "Invalid row index (%u) and column index (%u)", row, column);
        ss_set_error(error, error_message, SS_INVALID_ROW_AND_COLUMN_INDEX);
        return;
   }
   if (row >= matrix -> rows) {
        sprintf(error_message, "Invalid row index (%u)", row);
        ss_set_error(error, error_message, SS_INVALID_ROW_INDEX);
        return;
   }
   if (column >= matrix -> columns) {
        sprintf(error_message, "Invalid column index (%u)", column);
        ss_set_error(error, error_message, SS_INVALID_COLUMN_INDEX);
        return;
   }
   matrix -> data[row*matrix -> columns + column] = value;
}

double ss_matrix_get(const ss_matrix *matrix, uint32_t row, uint32_t column, ss_error *error) {
    if (error) ss_clear_error(error);
   char error_message[4096];
   if (matrix == NULL) {
        ss_set_error(error, "Null Pointer", SS_NULL_POINTER);
        return 0.0;
   }
   if (row >= matrix -> rows && column >= matrix -> columns) {
        sprintf(error_message, "Invalid row index (%u) and column index (%u)", row, column);
        ss_set_error(error, error_message, SS_INVALID_ROW_AND_COLUMN_INDEX);
        return 0.0;
   }
   if (row >= matrix -> rows) {
        sprintf(error_message, "Invalid row index (%u)", row);
        ss_set_error(error, error_message, SS_INVALID_ROW_INDEX);
        return 0.0;
   }
   if (column >= matrix -> columns) {
        sprintf(error_message, "Invalid column index (%u)", column);
        ss_set_error(error, error_message, SS_INVALID_COLUMN_INDEX);
        return 0.0;
   }
   return matrix -> data[row*matrix -> columns + column];
}

void ss_matrix_get_dimensions(const ss_matrix *matrix, uint32_t *rows, uint32_t *columns, ss_error *error) {
    if (error) ss_clear_error(error);
   if (matrix == NULL) {
        ss_set_error(error, "Null Pointer", SS_NULL_POINTER);
        return;
   }
   if (rows) *rows = matrix -> rows;
   if (columns) *columns = matrix -> columns;
}

uint32_t ss_matrix_get_row_count(const ss_matrix *matrix, ss_error *error) {
    if (error) ss_clear_error(error);
   if (matrix == NULL) {
        ss_set_error(error, "Null Pointer", SS_NULL_POINTER);
        return 0;
   }
   return matrix -> rows;
}

uint32_t ss_matrix_get_column_count(const ss_matrix *matrix, ss_error *error) {
    if (error) ss_clear_error(error);
   if (matrix == NULL) {
        ss_set_error(error, "Null Pointer", SS_NULL_POINTER);
        return 0;
   }
   return matrix -> columns;
}

void ss_matrix_write(const ss_matrix *matrix, const char * filename, ss_error *error) {
    if (error) ss_clear_error(error);
    FILE *f;
    char error_message[4096];
    if (matrix == NULL) {
        ss_set_error(error, "Null Pointer argument(1)", SS_NULL_POINTER);
        return;
    }
    if (filename == NULL) {
        ss_set_error(error, "Null Pointer argument(2)", SS_NULL_POINTER);
        return;
    }
    f = fopen(filename, "wb");
    if (f == NULL) {
        sprintf(error_message, "Unable to create file (%s)", filename);
        ss_set_error(error, error_message, SS_FILE_WRITE_ERROR);
        return;
    }
    fwrite(&(matrix-> rows), sizeof(uint32_t), 1, f);
    fwrite(&(matrix-> columns), sizeof(uint32_t), 1, f);
    fwrite(matrix-> data, sizeof(double), matrix -> rows * matrix -> columns, f);
    fclose(f);
}

ss_matrix * ss_matrix_read(const char * filename, ss_error *error) {
    if (error) ss_clear_error(error);
    ss_matrix *matrix;
    uint32_t rows, columns;
    FILE *f;
    char error_message[4096];
    if (filename == NULL) {
        ss_set_error(error, "Null Pointer argument(1)", SS_NULL_POINTER);
        return NULL;
    }
    f = fopen(filename, "rb");
    if (f == NULL) {
        sprintf(error_message, "Unable to read file (%s)", filename);
        ss_set_error(error, error_message, SS_FILE_READ_ERROR);
        return NULL;
    }
    fread(&rows, sizeof(uint32_t), 1, f);
    fread(&columns, sizeof(uint32_t), 1, f);
    matrix = ss_matrix_create_new(rows, columns, error);
    if (matrix == NULL) {
        fclose(f);
        return NULL;
    }
    fread(matrix -> data, sizeof(double), rows*columns, f);
    fclose(f);
    return matrix;

}

ss_matrix * ss_matrix_read_csv(const char * filename, uint8_t skip_lines, ss_error *error) {
    FILE *f;
    char error_message[4096];
    char m, last_character;
    char str[51];
    double value;
    ss_matrix *matrix;
    uint32_t rows, columns;
    long data_starts_at;
    uint32_t previous_row_commas, current_row_commas, r, c;
    int i;
    if (error) ss_clear_error(error);
    if (filename == NULL) {
        ss_set_error(error, "Null Pointer argument(1)", SS_NULL_POINTER);
        return NULL;
    }
    f = fopen(filename, "r");
    if (f == NULL) {
        sprintf(error_message, "Unable to read file (%s)", filename);
        ss_set_error(error, error_message, SS_FILE_READ_ERROR);
        return NULL;
    }
    while (skip_lines > 0) {
        m = fgetc(f);
        if (feof(f)) break;
        if (m == '\n') skip_lines--;
    }
    if (skip_lines > 0) {
        sprintf(error_message, "Invalid csv format of file %s", filename);
        ss_set_error(error, error_message, SS_CSV_FORMAT_ERROR);
        fclose(f);
        return NULL;
    }
    data_starts_at = ftell(f);
    // logic to count rows and columns
    previous_row_commas = 0;
    current_row_commas = 0;
    rows = 0;
    while (1) {
        m = fgetc(f);
        if (feof(f)) break;
        if (m == ',') current_row_commas++;
        if (m == '\n') {
            if (previous_row_commas !=0 && previous_row_commas != current_row_commas) {
              sprintf(error_message, "Invalid csv format of file %s", filename);
                ss_set_error(error, error_message, SS_CSV_FORMAT_ERROR);
                fclose(f);
                return NULL;
             }
             previous_row_commas = current_row_commas;
             current_row_commas = 0;
             rows++;
        }
        last_character = m;
    }
    columns = previous_row_commas + 1;
    if (columns == 0) {
        sprintf(error_message, "Empty csv file : %s", filename);
        ss_set_error(error, error_message, SS_CSV_EMPTY_ERROR);
        fclose(f);
        return NULL;
    }
    if (last_character != '\n') rows++;
    matrix = ss_matrix_create_new(rows, columns, error);
    if (matrix == NULL) return NULL;
    fseek(f, data_starts_at, 0);
    i = 0;
    r = 0;
    c = 0;
    while (1) {
        m = fgetc(f);
        if (feof(f)) break;
        if (m == ',' || m == '\n') {
            str[i] = '\0';
            i = 0;
            value = atof(str);
            ss_matrix_set(matrix, r, c, value, error);
            c++;
            if (c == columns) {
                r++;
                c = 0;
            }
        } else {
            str[i++] = m;
        }
    }
    fclose(f);
    return matrix;
}

void ss_matrix_transpose(ss_matrix *matrix, ss_error *error) {
    uint32_t a,b;
    double value;
    double *new_data;
    uint32_t new_rows, new_columns;
    if (error) ss_clear_error(error);
    if (matrix == NULL) {
        ss_set_error(error, "Null pointer", SS_NULL_POINTER);
        return;
    }
    if (matrix -> rows == 1 && matrix -> columns == 1) return;
    if (matrix -> rows == 1) {
        matrix -> rows = matrix -> columns;
        matrix -> columns = 1;
        return;
    }
    if (matrix -> columns == 1) {
        matrix -> columns = matrix -> rows;
        matrix -> rows = 1;
        return;
    }
    if (matrix -> rows == matrix -> columns) { // it is a square matrix
        for (a = 0; a < matrix -> rows; ++a) {
            for (b = a + 1; b < matrix -> columns; ++b) {
                value = matrix -> data[a*matrix->columns + b];
                matrix -> data[a*matrix->columns + b] = matrix -> data[b*matrix->columns + a];
                matrix -> data[b*matrix->columns + a] = value;
            }
        }
        return;
    }
    new_rows = matrix -> columns;
    new_columns = matrix -> rows;
    new_data = (double *)malloc(sizeof(double)*new_rows*new_columns);
    if (new_data == NULL) {
        ss_set_error(error, "Low Memory", SS_LOW_MEMORY);
        return;
    }
    for (a = 0; a < matrix->rows; ++a) {
        for (b = 0; b < matrix -> columns; ++b) {
            new_data[b*new_columns + a] = matrix -> data[a * matrix -> columns + b];
        }
    }
    free(matrix -> data);
    matrix -> data = new_data;
    matrix -> rows = new_rows;
    matrix -> columns = new_columns;
}

ss_matrix * ss_matrix_transpose_create_new(const ss_matrix *matrix, ss_error *error) {
    uint32_t a,b;
    ss_matrix *new_matrix;
    if (error) ss_clear_error(error);
    if (matrix == NULL) {
        ss_set_error(error, "Null pointer", SS_NULL_POINTER);
        return NULL;
    }
    new_matrix = ss_matrix_create_new(matrix -> columns, matrix -> rows, error);
    if (new_matrix == NULL) return NULL;
    for (a = 0; a < matrix->rows; ++a) {
        for (b = 0; b < matrix -> columns; ++b) {
            new_matrix -> data[b*new_matrix -> columns + a] = matrix -> data[a * matrix -> columns + b];
        }
    }
    return new_matrix;
}

void ss_matrix_print(FILE *f, const ss_matrix *matrix, ss_error *error) {
    uint32_t r,c;
    if (error) ss_clear_error(error);
    if (matrix == NULL) {
        ss_set_error(error, "Null pointer", SS_NULL_POINTER);
        return;
    }
    for (r = 0; r < matrix->rows; ++r) {
        for (c = 0; c < matrix -> columns; ++c) {
            fprintf(f, "%12.6lf ", ss_matrix_get(matrix, r, c, NULL));
        }
        fprintf(f, "\n");
    }

}
