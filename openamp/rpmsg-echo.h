#ifndef RPMSG_ECHO_H
#define RPMSG_ECHO_H

#define RPMSG_SERVICE_NAME         "rpmsg-openamp-demo-channel"

#define SHUTDOWN_MSG	0xEF56A55A

#define LPRINTF(format, ...) xil_printf(format, ##__VA_ARGS__)
#define LPERROR(format, ...) LPRINTF("ERROR: " format, ##__VA_ARGS__)

int app(struct rpmsg_device *rdev, void *priv);

#endif /* RPMSG_ECHO_H */
