#include <iostream>
#include "server.hpp"
#include "DoubleLinkList.hpp"


int main()
{
    // 多线程处理
    pthread_t th1;
    pthread_create(&th1, NULL, server,0);
    

    pthread_t th2;
    pthread_create(&th2, NULL, list_start,0);

    pthread_detach(th1);

    pthread_join(th2, NULL);

    return 0;
}
