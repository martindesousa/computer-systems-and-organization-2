//Source: life-serial.c by Luther Tychonievich and Charles Reiss

#include <pthread.h>
#include "life.h"
#include <string.h>

// Struct to hold thread data
typedef struct {
    LifeBoard *state;
    LifeBoard *next_state;
    int start_row;
    int end_row;
    int steps;
    pthread_barrier_t *barrier;
} ThreadData;

// Thread function to process one portion of the board
void* process_thread(void* data_arg) {
    ThreadData *data = (ThreadData*)data_arg;

    for (int step = 0; step < data->steps; step++) {
        LifeBoard *state = (step % 2 == 0) ? data->state : data->next_state;
        LifeBoard *next_state = (step % 2 == 0) ? data->next_state : data->state;

        /* We use the range [1, width - 1) here instead of
        * [0, width) because we fix the edges to be all 0s.
        */
        for (int y = data->start_row; y < data->end_row; y++) {
            for (int x = 1; x < state->width - 1; x++) {

                /* For each cell, examine a 3x3 "window" of cells around it,
                * and count the number of live (true) cells in the window. */

                int live_in_window = 0;
                for (int y_offset = -1; y_offset <= 1; y_offset++)
                    for (int x_offset = -1; x_offset <= 1; x_offset++)
                        if (LB_get(state, x + x_offset, y + y_offset))
                            live_in_window++;

                /* Cells with 3 live neighbors remain or become live.
                    Live cells with 2 live neighbors remain live. */
                LB_set(next_state, x, y,
                    live_in_window == 3 || (live_in_window == 4 && LB_get(state, x, y))
                );
            }
        }
        pthread_barrier_wait(data->barrier);
    }
    return NULL;
}

void simulate_life_parallel(int threads, LifeBoard *state, int steps) {
    LifeBoard *next_state = LB_new(state->width, state->height);
    pthread_t thread_ids[threads];
    ThreadData thread_data[threads];
    pthread_barrier_t barrier;  
    pthread_barrier_init(&barrier, NULL, threads);    
    int rows_per_thread = (state->height - 2) / threads;

    for (int i = 0; i < threads; i++) {
        thread_data[i].state = state;
        thread_data[i].next_state = next_state;
        thread_data[i].steps  = steps;
        thread_data[i].barrier = &barrier;
        thread_data[i].start_row = i * rows_per_thread + 1;
        if (i == threads - 1) {
            thread_data[i].end_row = state->height - 1;
        } 
        else {
            thread_data[i].end_row = thread_data[i].start_row + rows_per_thread;
        }

        pthread_create(&thread_ids[i], NULL, process_thread, &thread_data[i]);
    }

    for (int i = 0; i < threads; i++) {
        pthread_join(thread_ids[i], NULL);
    }

    if (!(steps % 2 == 0)) {
        LB_swap(state, next_state);
    }

    LB_del(next_state);
    pthread_barrier_destroy(&barrier);
}
