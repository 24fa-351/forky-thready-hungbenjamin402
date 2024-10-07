#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>
#include <string.h>

FILE *output_file;

void write_to_file(const char *format, ...) {
    va_list args;
    va_start(args, format);
    vfprintf(output_file, format, args);
    va_end(args);
    fflush(output_file);
}

void create_processes(int n, int pattern, int parent_id) {
    if (n <= 0) return;

    pid_t pid = fork();

    if (pid < 0) {
        perror("Fork failed");
        exit(1);
    } else if (pid == 0) {
        // Child process
        int my_id = (pattern == 1) ? n : parent_id + 1;
        write_to_file("Process %d (PID: %d) beginning\n", my_id, getpid());

        int sleep_time = (rand() % 8) + 1;
        sleep(sleep_time);

        if (pattern == 1) {
            for (int i = n - 1; i > 0; i--) {
                write_to_file("Process %d (PID: %d) creating Process %d\n", my_id, getpid(), i);
                create_processes(i, pattern, my_id);
            }
        } else if (pattern == 2 && n > 1) {
            write_to_file("Process %d (PID: %d) creating Process %d\n", my_id, getpid(), my_id + 1);
            create_processes(n - 1, pattern, my_id);
        } else if (pattern == 3) {
            int left_child = my_id * 2;
            int right_child = my_id * 2 + 1;
            if (left_child <= n) {
                write_to_file("Process %d (PID: %d) creating Process %d\n", my_id, getpid(), left_child);
                create_processes(n - left_child + 1, pattern, my_id);
            }
            if (right_child <= n) {
                write_to_file("Process %d (PID: %d) creating Process %d\n", my_id, getpid(), right_child);
                create_processes(n - right_child + 1, pattern, my_id);
            }
        }

        write_to_file("Process %d (PID: %d) exiting\n", my_id, getpid());
        exit(0);
    } else {
        // Parent process
        wait(NULL);
    }
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <number_of_things> <pattern_number>\n", argv[0]);
        exit(1);
    }

    int n = atoi(argv[1]);
    int pattern = atoi(argv[2]);

    if (n < 1 || n > 256 || pattern < 1 || pattern > 3) {
        fprintf(stderr, "Invalid arguments. 1 <= number_of_things <= 256, pattern_number must be 1, 2, or 3\n");
        exit(1);
    }
    else {
        printf("Program is running. Output is being written in results.txt file.\n");
    }

    srand(time(NULL));

    output_file = fopen("results.txt", "a");  // Open file in append mode
    if (output_file == NULL) {
        perror("Failed to open results.txt");
        exit(1);
    }

    // Write the command used to run the program
    write_to_file("%s %s %s\n", argv[0], argv[1], argv[2]);

    write_to_file("Main process (PID: %d) beginning\n", getpid());
    create_processes(n, pattern, 0);
    write_to_file("Main process (PID: %d) exiting\n", getpid());

    fclose(output_file);

    printf("Program ends.");

    return 0;
}