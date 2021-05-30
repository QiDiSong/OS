select主要作用：
  将文件描述符fd收集过来，交给内核，让内核帮忙判断哪一个fd有数据
  当任意一个或多个fd有数据的时候，select函数就会返回，（没有数据的时候select处于阻塞状态）
  并且有数据的fd就会被置位，这个时候select返回，返回后遍历fd的集合，看哪个fd被置位了
  这时可以读取fd的数据并进行处理
 
提高效率最主要的一点是：
  select将fd的集合set交给内核去处理，让内核去帮忙判断哪个fd有数据到来，减少了由用户态切换至内核态的开销
  （不用select的话，for的遍历每一个fd都要有一次用户到内核态的切换）
  
select的缺点：
1. bitmap（也就是fd_set）默认大小1024,虽然可调整大小，但仍有上限
2. fd_set不可重用，每次while(1)回来之后，fd_set都要被重新置位，并赋值给rset
3. rset从用户态拷贝到内核态仍需要一定的开销
4. 当fd被置位后，仍需要O(n)的复杂度去遍历，才能找到是哪一个fd被置位


sockfd = socket(AF_INET, SOCK_STREAM, 0);
memset(&addr, 0, sizeof(addr));
addr.sin_family = AF_INET;
addr.sin_port = htons(2000);
adr.sin_addr.s_addr = INADDR_ANY;
bind(sockfd, (struct sockaddr*)&addr, sizeof(addr))
listen(sockfd, 5); 	// 监听5个文件描述符，最多可同时有5个fd收到数据

for (int i = 0; i < 5; i++)
{
	memset(&client, 0, sizeof(client));
	addrlen = sizeof(client);
	fd_set[i] = accept(sockfd, (struct sockaddr*)&client, &addrlen);
	if (fd_set[i] > fd_set_max)
	{
		fd_set_max = fd_set[i];
	}
}

// select处理
while (1)
{
	FD_ZERO(&rset);  // 全部置0
	for (int i = 0; i < 5; i++)
	{
		FD_SET(fd_set[i], &rset);	// 宏FD_SET设置文件描述符集fdset中对应于文件描述符fd的位(设置为1)
	}
	// 宏FD_SET设置文件描述符集fdset中对应于文件描述符fd的位(设置为1)
	// 宏FD_CLR清除文件描述符集fdset中对应于文件描述符fd的位（设置为0）
	// 宏FD_ZERO清除文件描述符集fdset中的所有位(即把所有位都设置为0)
	// 使用这3个宏在调用select前设置描述符屏蔽位。因为这3个描述符集参数是值-结果参数，在调用select后，结果指示哪些描述符已就绪。
	// 使用FD_ISSET来检测文件描述符集fdset中对应于文件描述符fd的位是否被设置。描述符集内任何与未就绪描述符对应的位返回时均清成0，
	// 为此，每次重新调用select函数时，必须再次把所有描述符集内所关心的位置1.

	puts("round again!");
	select(fd_set_max + 1, &rset, NULL, NULL, NULL);
	// 这个select函数在fd没有数据上报时，一直处于阻塞状态，直到有数据上传时，才被唤醒

	for (int i = 0; i < 5; i++)
	{
		if (FD_ISSET(fd_set[i], &rset))
		{
			memset(buffer, 0, MAXBUF);
			read(fd_set[i], buffer, MAXBUF);
			puts(buffer); // puts函数表示收到buffer数据后，你所对数据进行处理的操作
		}
	}
}
