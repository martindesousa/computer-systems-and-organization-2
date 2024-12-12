#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "netsim.h"

unsigned char compute_checksum(const char *data, size_t len) {
    unsigned char checksum = 0;
    for (size_t i = 1; i < len; i++) { 
        checksum ^= data[i]; 
    }
    return checksum;
}


unsigned char last_ack_seq = 0;  
int timeout_id = -1;             

void request_resend(void *seq) {
    unsigned char seq_num = (unsigned char)(uintptr_t)seq;  

    char request[5];
    if (seq_num == 0) {
        request[1] = 'G';
        request[2] = 'E';
        request[3] = 'T';
        request[4] = last_ack_seq;  
    } else {
        request[1] = 'A';
        request[2] = 'C';
        request[3] = 'K';
        request[4] = seq_num;  
    }

    request[0] = compute_checksum(request, 5);

    send(5, request);

    timeout_id = setTimeout(request_resend, 1000, (void *)(uintptr_t)seq_num);
}

// Function called when a packet is received from the server
void recvd(size_t len, void* _data) {
    char *data = _data;                        
    unsigned char received_checksum = data[0]; // Get the checksum
    unsigned char computed_checksum = compute_checksum(data, len);  

    if (received_checksum != computed_checksum) { 
        return;
    }

    unsigned char seq_num = data[1];  
    unsigned char total_seq = data[2];  

    if (seq_num <= last_ack_seq) return;

    fwrite(data + 3, 1, len - 3, stdout);  
    fflush(stdout);  

    // Update the last acked seq_num to the current packet seq_num
    last_ack_seq = seq_num;

    // Prepare and send an ACK for the current packet
    char ack[5];
    ack[1] = 'A';
    ack[2] = 'C';
    ack[3] = 'K';
    ack[4] = seq_num;  
    ack[0] = compute_checksum(ack, 5);  
    send(5, ack);  

    if (timeout_id >= 0) {
        clearTimeout(timeout_id);
    }

    if (seq_num < total_seq) {
        timeout_id = setTimeout(request_resend, 1000, (void *)(uintptr_t)seq_num);
    }
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "USAGE: %s n\n    where n is a number between 0 and 3\n", argv[0]);
        return 1;  
    }
    
    last_ack_seq = 0;  

    char data[5];
    data[1] = 'G';
    data[2] = 'E';
    data[3] = 'T';
    data[4] = argv[1][0];  
    data[0] = compute_checksum(data, 5);  

    send(5, data);

    timeout_id = setTimeout(request_resend, 1000, (void *)(uintptr_t)0);

    waitForAllTimeoutsAndMessagesThenExit();
}
