#ifndef WEBSERVER_H
#define WEBSERVER_H

#include <netinet/in.h>
#include <stdbool.h>

// Start the HTTP server (should be run in a separate thread/task).
void webserver_start(void);

// A simple dispatcher: the CGI name in URI is the command.
// For "scan", we read distances and reply JSON; for others, issue motor commands.
static int cmd_handler(const char *uri, char *params, char *post_data, int content_len, int sock);

void update_user_ping(void);

bool webserver_is_auto_mode(void);

#endif // WEBSERVER_H
