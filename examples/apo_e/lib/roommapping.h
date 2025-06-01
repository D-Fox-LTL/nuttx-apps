#ifndef ROOMMAPPING_H
#define ROOMMAPPING_H

// Init variables
void mapping_init(void);

// Store mapped distance
void mapping_store(int angle, int dist);

// Get distance mapped inside dist variable
int mapping_get_distance(int angle);

void mapping_dump(void);

void mapping_get_cartesian(int index, float *x, float *y);

void http_send(int connfd, const char *body);

// HTTP handler for /scan
void handle_scan(int connfd);

#endif // ROOMMAPPING_H
