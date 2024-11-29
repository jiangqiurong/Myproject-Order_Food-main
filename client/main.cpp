#include <iostream>
#include "screen.hpp"
#include "client.hpp"
#include "DoubleLinkList.hpp"

int main()
{

	// 多线程处理
	// 1. 客户端线程
	pthread_t th1;
	pthread_create(&th1, NULL, client_start, 0);

	// 2. 链表线程
	pthread_t th2;
	pthread_create(&th2, NULL, list_start, 0);

	// 3. LCD线程
	pthread_t th3;
	pthread_create(&th3, NULL, start_lcd, 0);

	pthread_join(th1, NULL);
	pthread_join(th2, NULL);
	pthread_join(th3, NULL);

	return 0;
}