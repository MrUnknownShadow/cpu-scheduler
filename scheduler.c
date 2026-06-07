#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#define MAX_PROCESSES 100

typedef struct {
    int pid;
    int arrival;
    int burst;
    int remaining;
    int start;
    int finish;
    int waiting;
    int turnaround;
} Process;

void read_input(const char *filename, Process procs[], int *n) {
    FILE *f = fopen(filename, "r");
    if (!f) { perror("fopen"); exit(1); }
    *n = 0;
    while (fscanf(f, "%d %d %d",
                  &procs[*n].pid,
                  &procs[*n].arrival,
                  &procs[*n].burst) == 3) {
        procs[*n].remaining = procs[*n].burst;
        (*n)++;
    }
    fclose(f);
}

void print_results(Process procs[], int n) {
    printf("\n%-6s %-10s %-10s %-12s %-10s\n",
           "PID", "Arrival", "Burst", "Turnaround", "Waiting");
    double avg_tat = 0, avg_wt = 0;
    for (int i = 0; i < n; i++) {
        printf("%-6d %-10d %-10d %-12d %-10d\n",
               procs[i].pid, procs[i].arrival, procs[i].burst,
               procs[i].turnaround, procs[i].waiting);
        avg_tat += procs[i].turnaround;
        avg_wt  += procs[i].waiting;
    }
    printf("\nAverage Turnaround Time: %.2f\n", avg_tat / n);
    printf("Average Waiting Time:    %.2f\n",  avg_wt  / n);
}

int cmp_arrival(const void *a, const void *b) {
    return ((Process *)a)->arrival - ((Process *)b)->arrival;
}

/* ── FCFS ── */
void fcfs(Process procs[], int n) {
    qsort(procs, n, sizeof(Process), cmp_arrival);
    int time = 0;
    printf("Gantt Chart: ");
    for (int i = 0; i < n; i++) {
        if (time < procs[i].arrival) time = procs[i].arrival;
        printf("| P%d ", procs[i].pid);
        procs[i].start      = time;
        procs[i].finish     = time + procs[i].burst;
        procs[i].turnaround = procs[i].finish - procs[i].arrival;
        procs[i].waiting    = procs[i].turnaround - procs[i].burst;
        time = procs[i].finish;
    }
    printf("\n");
    print_results(procs, n);
}

/* ── Round Robin ── */
void round_robin(Process procs[], int n, int quantum) {
    qsort(procs, n, sizeof(Process), cmp_arrival);
    int time = 0, done = 0;
    int queue[MAX_PROCESSES * 100], head = 0, tail = 0;
    int enqueued[MAX_PROCESSES] = {0};

    printf("Gantt Chart: ");
    queue[tail++] = 0;
    enqueued[0] = 1;

    while (done < n) {
        if (head == tail) {
            /* idle: advance to next arrival */
            int next = INT_MAX;
            for (int i = 0; i < n; i++)
                if (!enqueued[i] && procs[i].arrival < next)
                    next = procs[i].arrival;
            time = next;
            for (int i = 0; i < n; i++)
                if (!enqueued[i] && procs[i].arrival <= time) {
                    queue[tail++] = i;
                    enqueued[i] = 1;
                }
        }
        int idx = queue[head++];
        int slice = procs[idx].remaining < quantum ? procs[idx].remaining : quantum;
        printf("| P%d ", procs[idx].pid);
        procs[idx].remaining -= slice;
        time += slice;

        /* enqueue newly arrived */
        for (int i = 0; i < n; i++)
            if (!enqueued[i] && procs[i].arrival <= time) {
                queue[tail++] = i;
                enqueued[i] = 1;
            }

        if (procs[idx].remaining == 0) {
            procs[idx].finish     = time;
            procs[idx].turnaround = time - procs[idx].arrival;
            procs[idx].waiting    = procs[idx].turnaround - procs[idx].burst;
            done++;
        } else {
            queue[tail++] = idx;
        }
    }
    printf("\n");
    print_results(procs, n);
}

/* ── SRTF ── */
void srtf(Process procs[], int n) {
    int time = 0, done = 0;
    int prev = -1;
    printf("Gantt Chart: ");
    while (done < n) {
        int idx = -1, min_rem = INT_MAX;
        for (int i = 0; i < n; i++) {
            if (procs[i].arrival <= time && procs[i].remaining > 0
                && procs[i].remaining < min_rem) {
                min_rem = procs[i].remaining;
                idx = i;
            }
        }
        if (idx == -1) { time++; continue; }
        if (idx != prev) { printf("| P%d ", procs[idx].pid); prev = idx; }
        procs[idx].remaining--;
        time++;
        if (procs[idx].remaining == 0) {
            procs[idx].finish     = time;
            procs[idx].turnaround = time - procs[idx].arrival;
            procs[idx].waiting    = procs[idx].turnaround - procs[idx].burst;
            done++;
        }
    }
    printf("\n");
    print_results(procs, n);
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        fprintf(stderr, "Usage: %s <input.txt> <fcfs|rr|srtf> [quantum]\n", argv[0]);
        return 1;
    }
    Process procs[MAX_PROCESSES];
    int n;
    read_input(argv[1], procs, &n);

    if (strcmp(argv[2], "fcfs") == 0) {
        fcfs(procs, n);
    } else if (strcmp(argv[2], "rr") == 0) {
        int q = argc >= 4 ? atoi(argv[3]) : 4;
        round_robin(procs, n, q);
    } else if (strcmp(argv[2], "srtf") == 0) {
        srtf(procs, n);
    } else {
        fprintf(stderr, "Unknown algorithm: %s\n", argv[2]);
        return 1;
    }
    return 0;
}
