#include "webserver.h"
#include "motorcontrol.h"
#include "servocontrol.h"
#include "roommapping.h"
#include "netutils/httpd.h"
#include "netutils/httpd_cgi.h"
#ifdef CONFIG_EXAMPLES_WEBSERVER_DHCPC
#include "arpa/inet.h"
#endif
#ifdef CONFIG_EXAMPLES_WEBSERVER_DHCPC
#  include "netutils/dhcpc.h"
#endif
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <sys/time.h>

#define HTTPD_BLOCK 0
#define HTTPD_FAIL -1

static bool auto_mode = false;
static time_t last_user_ping = 0;

#define USER_TIMEOUT_SEC 10

static const struct httpd_cgi_call cmd_cgis[] = {
  { "/fwd",  cmd_handler },
  { "/rev",  cmd_handler },
  { "/left", cmd_handler },
  { "/right", cmd_handler },
  { "/stop", cmd_handler },
  { "/auto", cmd_handler },
  { "/scan", cmd_handler },
  { NULL, NULL }
};


void webserver_start(void)
{
  httpd_init();
  httpd_register_cgi(cmd_cgis);
}

void update_user_ping(void) {
  time(&last_user_ping);
}

bool webserver_is_auto_mode(void)
{
  time_t now;
  time(&now);
  if (auto_mode && difftime(now, last_user_ping) > USER_TIMEOUT_SEC) {
    auto_mode = false;
    motor_stop();
    printf("Auto mode deactivated: user timeout.\n");
  }
  return auto_mode;
}

/* Handle commands from user given to webserver */
static int cmd_handler(const char *uri, char *params, char *post_data, int content_len, int sock)
{
  update_user_ping();

  //for now too many if-else commands
  //TODO: change it into something more computer friendly
  if (strcmp(uri, "/fwd") == 0) {
    motor_forward();
    return httpd_cgi_success(sock);
  }
  else if (strcmp(uri, "/rev") == 0) {
    motor_backward();
    return httpd_cgi_success(sock);
  }
  else if (strcmp(uri, "/left") == 0) {
    motor_left();
    return httpd_cgi_success(sock);
  }
  else if (strcmp(uri, "/right") == 0) {
    motor_right();
    return httpd_cgi_success(sock);
  }
  else if (strcmp(uri, "/stop") == 0) {
    motor_stop();
    return httpd_cgi_success(sock);
  }
  else if (strncmp(uri, "/auto", 5) == 0) {
    auto_mode = (strstr(params, "on") != NULL);
    printf("Auto mode %s\n", auto_mode ? "enabled" : "disabled");
    return httpd_cgi_success(sock);
  }
  else if (strcmp(uri, "/scan") == 0) {
    const int steps = 19;
    int angles[steps];
    float distances[steps];
    for (int i = 0; i < steps; i++) {
      angles[i] = i * (180 / (steps - 1));
    }
    mapping_scan(angles, steps, distances);

    char buf[1024];
    int offset = snprintf(buf, sizeof(buf), "[");
    for (int i = 0; i < steps; i++) {
      offset += snprintf(buf + offset, sizeof(buf) - offset,
        "{\"angle\":%d,\"dist\":%.1f}%s", angles[i], distances[i], (i+1<steps?",":""));
    }
    offset += snprintf(buf + offset, sizeof(buf) - offset, "]");

    dprintf(sock, "HTTP/1.0 200 OK\r\n");
    dprintf(sock, "Content-Type: application/json\r\n");
    dprintf(sock, "Content-Length: %d\r\n\r\n", (int)strlen(buf));
    dprintf(sock, "%s", buf);
    return HTTPD_BLOCK;
  }

  return HTTPD_FAIL;
}
