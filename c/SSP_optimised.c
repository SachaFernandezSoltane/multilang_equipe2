#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include <dirent.h>
#include <sys/stat.h>
#include <ctype.h>

typedef struct {
    long long target;
    long long* original;
    int size;
    long long** solutions;
    int solution_count;
    int solution_capacity;
} SSP;

void bp(SSP* ssp, int i, long long partial, long long total, bool* x) {
    if (partial + total < ssp->target || partial > ssp->target) return;

    if (partial == ssp->target) {
        if (ssp->solution_count >= 10000000) return;
        if (ssp->solution_count >= ssp->solution_capacity) {
            ssp->solution_capacity *= 2;
            ssp->solutions = realloc(ssp->solutions, ssp->solution_capacity * sizeof(long long*));
            if (!ssp->solutions) {
                fprintf(stderr, "Erreur de réallocation\n");
                exit(EXIT_FAILURE);
            }
        }
        long long* sol = malloc(i * sizeof(long long));
        int sol_index = 0;
        for (int k = 0; k < i; ++k) {
            if (x[k]) sol[sol_index++] = ssp->original[k];
        }
        ssp->solutions[ssp->solution_count] = sol;
        ssp->solution_count++;
        return;
    }

    if (i == ssp->size) return;

    total -= ssp->original[i];

    x[i] = false;
    bp(ssp, i + 1, partial, total, x);

    x[i] = true;
    partial += ssp->original[i];
    bp(ssp, i + 1, partial, total, x);
    x[i] = false;
}

SSP* create_ssp(int n) {
    if (n <= 0) {
        fprintf(stderr, "SSP size cannot be nonpositive\n");
        exit(EXIT_FAILURE);
    }
    if (n <= 2) {
        fprintf(stderr, "SSP size is too small\n");
        exit(EXIT_FAILURE);
    }

    SSP* ssp = (SSP*)malloc(sizeof(SSP));
    ssp->target = 1;
    ssp->original = (long long*)malloc(n * sizeof(long long));
    ssp->size = n;
    ssp->original[0] = 1;
    ssp->solution_count = 0;
    ssp->solution_capacity = 10;
    ssp->solutions = calloc(ssp->solution_capacity, sizeof(long long*));

    srand((unsigned int)time(NULL));
    for (int i = 1; i < n; ++i) {
        ssp->original[i] = i + 1;
        if (rand() % 2) ssp->target += ssp->original[i];
    }

    for (int i = 0; i < n - 1; ++i) {
        for (int j = 0; j < n - i - 1; ++j) {
            if (ssp->original[j] < ssp->original[j + 1]) {
                long long temp = ssp->original[j];
                ssp->original[j] = ssp->original[j + 1];
                ssp->original[j + 1] = temp;
            }
        }
    }

    return ssp;
}

SSP* create_ssp_from_file(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    int size;
    if (fscanf(file, "%d", &size) != 1 || size <= 0) {
        fprintf(stderr, "Error while parsing input file\n");
        exit(EXIT_FAILURE);
    }

    SSP* ssp = (SSP*)malloc(sizeof(SSP));
    ssp->original = (long long*)malloc(size * sizeof(long long));
    ssp->size = size;
    ssp->solution_count = 0;
    ssp->solution_capacity = 10;
    ssp->solutions = calloc(ssp->solution_capacity, sizeof(long long*));

    if (fscanf(file, "%lld", &ssp->target) != 1) {
        fprintf(stderr, "Error while parsing input file\n");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < size; ++i) {
        if (fscanf(file, "%lld", &ssp->original[i]) != 1) {
            fprintf(stderr, "Error while parsing input file\n");
            exit(EXIT_FAILURE);
        }
    }

    for (int i = 0; i < size - 1; ++i) {
        for (int j = 0; j < size - i - 1; ++j) {
            if (ssp->original[j] < ssp->original[j + 1]) {
                long long temp = ssp->original[j];
                ssp->original[j] = ssp->original[j + 1];
                ssp->original[j + 1] = temp;
            }
        }
    }

    fclose(file);
    return ssp;
}

long long total_sum(const SSP* ssp) {
    long long sum = 0;
    for (int i = 0; i < ssp->size; ++i) sum += ssp->original[i];
    return sum;
}

void run_bp(SSP* ssp) {
    bool* x = (bool*)malloc(ssp->size * sizeof(bool));
    for (int i = 0; i < ssp->size; ++i) x[i] = false;
    long long total = total_sum(ssp);
    bp(ssp, 0, 0, total, x);
    free(x);
}

void print_ssp(const SSP* ssp) {
    printf("SSP(n = %d; target = %lld)\n", ssp->size, ssp->target);
}

void print_integers(const SSP* ssp) {
    printf("Original set = [");
    for (int i = 0; i < ssp->size; ++i) {
        printf("%lld", ssp->original[i]);
        if (i + 1 < ssp->size) printf(", ");
    }
    printf("]\n");
}

void print_solutions(const SSP* ssp) {
    if (ssp->solution_count < 10) {
        for (int i = 0; i < ssp->solution_count; ++i) {
            printf("[");
            for (int j = 0; j < ssp->size; ++j) {
                if (ssp->solutions[i][j] == 0) break;
                printf("%lld", ssp->solutions[i][j]);
                if (ssp->solutions[i][j + 1] != 0) printf(", ");
            }
            printf("]\n");
        }
    } else {
        printf("bp found %d solutions\n", ssp->solution_count);
    }
}

void free_ssp(SSP* ssp) {
    for (int i = 0; i < ssp->solution_count; ++i) {
        free(ssp->solutions[i]);
    }
    free(ssp->solutions);
    free(ssp->original);
    free(ssp);
}

void process_directory(const char* dir_path) {
    DIR* dir;
    struct dirent* entry;
    if ((dir = opendir(dir_path)) == NULL) {
        perror("Unable to open directory");
        return;
    }

    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_REG) {
            char* ext = strrchr(entry->d_name, '.');
            if (ext != NULL && strcmp(ext, ".txt") == 0) {
                char filepath[1024];
                snprintf(filepath, sizeof(filepath), "%s/%s", dir_path, entry->d_name);
                printf("Traitement du fichier : %s\n", entry->d_name);
                SSP* ssp = create_ssp_from_file(filepath);
                print_ssp(ssp);
                print_integers(ssp);
                printf("Target is %lld\n", ssp->target);
                printf("\n");

                printf("Running bp ... ");
                clock_t start = clock();
                run_bp(ssp);
                clock_t end = clock();
                printf("done!\n");
                print_solutions(ssp);
                printf("elapsed time %f\n", (double)(end - start) / CLOCKS_PER_SEC);
                printf("\n");

                free_ssp(ssp);
            }
        }
    }

    closedir(dir);
}

bool is_integer(const char* s) {
    for (int i = 0; s[i] != '\0'; ++i) {
        if (!isdigit(s[i])) return false;
    }
    return true;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printf("Usage: %s <integer|filename|directory>\n", argv[0]);
        return EXIT_FAILURE;
    }

    const char* arg = argv[1];
    struct stat path_stat;
    if (stat(arg, &path_stat) != 0) {
        if (is_integer(arg)) {
            int n = atoi(arg);
            SSP* ssp = create_ssp(n);
            print_ssp(ssp);
            print_integers(ssp);
            printf("Target is %lld\n", ssp->target);
            printf("\n");

            printf("Running bp ... ");
            clock_t start = clock();
            run_bp(ssp);
            clock_t end = clock();
            printf("done!\n");
            print_solutions(ssp);
            printf("elapsed time %f\n", (double)(end - start) / CLOCKS_PER_SEC);
            printf("\n");

            free_ssp(ssp);
        } else {
            fprintf(stderr, "Argument is not an integer or a valid file/directory: %s\n", arg);
            return EXIT_FAILURE;
        }
    } else {
        if (S_ISREG(path_stat.st_mode)) {
            SSP* ssp = create_ssp_from_file(arg);
            print_ssp(ssp);
            print_integers(ssp);
            printf("Target is %lld\n", ssp->target);
            printf("\n");

            printf("Running bp ... ");
            clock_t start = clock();
            run_bp(ssp);
            clock_t end = clock();
            printf("done!\n");
            print_solutions(ssp);
            printf("elapsed time %f\n", (double)(end - start) / CLOCKS_PER_SEC);
            printf("\n");

            free_ssp(ssp);
        } else if (S_ISDIR(path_stat.st_mode)) {
            process_directory(arg);
        } else {
            fprintf(stderr, "Argument is not a valid file or directory: %s\n", arg);
            return EXIT_FAILURE;
        }
    }

    return EXIT_SUCCESS;
}
