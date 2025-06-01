/* mapping.c */
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define PI 3.14159265358979323846
#define MAX_SCAN 36
#define DEG2RAD(deg) ((deg) * PI / 180.0)

static int scan_angles[MAX_SCAN];
static int scan_dists[MAX_SCAN];
static int scan_count = 0;

void mapping_init(void)
{
    scan_count = 0;
    memset(scan_angles, 0, sizeof(scan_angles));
    memset(scan_dists, 0, sizeof(scan_dists));
}

/* Used for storing mapped area */
void mapping_store(int angle, int dist)
{
    if (scan_count < MAX_SCAN) {
        scan_angles[scan_count] = angle;
        scan_dists[scan_count] = dist;
        scan_count++;
    }
}

/*  Gets distance from scanned area */
int mapping_get_distance(int angle)
{
    for (int i = 0; i < scan_count; i++) {
        if (scan_angles[i] == angle)
            return scan_dists[i];
    }
    return -1;
}

/* Mainly debug function*/
void mapping_dump(void)
{
    for (int i = 0; i < scan_count; i++) {
        printf("angle=%d dist=%d\n", scan_angles[i], scan_dists[i]);
    }
}

/* After mapping convert to cartesian */
void mapping_get_cartesian(int index, float *x, float *y)
{
    if (index >= scan_count || index < 0) {
        *x = 0;
        *y = 0;
        return;
    }

    float angle_rad = DEG2RAD(scan_angles[index]);
    *x = scan_dists[index] * cosf(angle_rad);
    *y = scan_dists[index] * sinf(angle_rad);
}

/* Send body to webserver */
void http_send(int connfd, const char *body) {
    char header[256];
    int body_len = strlen(body);

    snprintf(header, sizeof(header),
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: application/json\r\n"
        "Content-Length: %d\r\n"
        "Connection: close\r\n"
        "\r\n",
        body_len);

    // Send header
    write(connfd, header, strlen(header));

    // Send body
    write(connfd, body, body_len);
}

/* Handles scan and sends it with http_send*/
void handle_scan(int connfd)
{
    char buf[1024];
    int n = 0;

    n += snprintf(buf + n, sizeof(buf) - n, "[");

    for (int i = 0; i < scan_count; i++) {
        float x, y;
        mapping_get_cartesian(i, &x, &y);

        n += snprintf(buf + n, sizeof(buf) - n,
                      "%s{\"angle\":%d,\"dist\":%d,\"x\":%.2f,\"y\":%.2f}",
                      i ? "," : "", scan_angles[i], scan_dists[i], x, y);
    }

    snprintf(buf + n, sizeof(buf) - n, "]");

    http_send(connfd, buf);
}
