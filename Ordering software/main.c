#include "charge.h"


int main()
{
    TCP_INIT();
    ts_fd = open("/dev/input/event0",O_RDWR);
    if(ts_fd == -1)
    {
        perror("open ts error\n");
        return -1;

    }
    login_interface();
    printf("Welcome\n");
    close(ts_fd);
    close(tcp_socket);
    return 0;
}