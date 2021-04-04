// SPDX-License-Identifier: MIT and GPL
#include "cdev.h"

static int cdev_open(struct inode *inode, struct file *file);
static int cdev_release(struct inode *inode, struct file *file);
static ssize_t cdev_write(struct file *file, const char __user *buf,
			  size_t count, loff_t *loff);
static long cdev_ioctl(struct file *file, unsigned int cmd, unsigned long arg);

static uint8_t files_counter;
static const struct file_operations cdev_fops = {
	.open = &cdev_open,
	.release = &cdev_release,
	.write = &cdev_write,
	.unlocked_ioctl = &cdev_ioctl,
	// required to prevent module unloading while fops are in use
	.owner = THIS_MODULE,
};

int cdev_reg(struct cdev_reg_t *cdev)
{
	int status = 0;

	cdev->alloc.reg_cdev = 0;
	cdev->alloc.cl_create = 0;
	cdev->alloc.dev_create = 0;

	cdev->major_num = register_chrdev(0, cdev->name, &cdev_fops);
	if (IS_ERR_VALUE(cdev->major_num)) {
		pr_err("Can't register cdev:(\n");
		status = cdev->major_num;
		goto err;
	}
	cdev->alloc.reg_cdev = 1;

	cdev->cl = class_create(THIS_MODULE, cdev->name);
	if (IS_ERR(cdev->cl)) {
		pr_err("Can't create class:(\n");
		cdev_unreg(cdev);
		status = PTR_ERR(cdev->cl);
		goto err;
	}
	cdev->alloc.cl_create = 1;

	cdev->pdev = device_create(cdev->cl, NULL, MKDEV(cdev->major_num, 0),
				   NULL, cdev->name);
	if (IS_ERR(cdev->cl)) {
		pr_err("Can't create device:(\n");
		cdev_unreg(cdev);
		status = PTR_ERR(cdev->pdev);
		goto err;
	}
	cdev->alloc.dev_create = 1;

	pr_info("Registered device '%s' with %d:%d\n", cdev->name,
		cdev->major_num, 0);

err:
	return status;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void cdev_unreg(struct cdev_reg_t *cdev)
{
	// paranoid checking (afterwards to ensure all fops ended)
	if (files_counter != 0)
		pr_err("Some files still opened:("); // should never happen

	if (cdev->alloc.dev_create) {
		device_destroy(cdev->cl, MKDEV(cdev->major_num, 0));
		pr_info("Device destroyed\n");
	}

	if (cdev->alloc.cl_create) {
		class_destroy(cdev->cl);
		pr_info("class destroyed\n");
	}

	if (cdev->alloc.reg_cdev) {
		unregister_chrdev(cdev->major_num, cdev->name);
		pr_info("chdev unregistered\n");
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * cdev_open() - callback for open() file operation
 * @inode: information to manipulate the file (unused)
 * @file: VFS file opened by a process
 *
 *
 */
static int cdev_open(struct inode *inode, struct file *file)
{
	files_counter++;

	return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * cdev_release() - file close() callback
 * @inode: information to manipulate the file (unused)
 * @file: VFS file opened by a process
 */
static int cdev_release(struct inode *inode, struct file *file)
{
	files_counter--;

	return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * cdev_write() - callback for file write() operation
 * @file: VFS file opened by a process
 * @buf:
 * @count:
 * @loff:
 */
static ssize_t cdev_write(struct file *file, const char __user *buf,
			  size_t count, loff_t *loff)
{
	uint32_t n = 0;
	char *tmp = kzalloc(sizeof(*tmp) * count, GFP_KERNEL);
	char *line_buf;
	uint8_t i;

	//n = simple_write_to_buffer(tmp, count, loff, buf, count);

	n = copy_from_user(tmp, buf, count);

	lcd_clear();
	lcd_home();

	if (n <= NUM_OF_COLS) {
		lcd_puts(tmp);
	} else {
		//to be sure that last symbol is '\0'
		line_buf = kzalloc(NUM_OF_COLS + 1, GFP_KERNEL);

		for (i = 0; (i < NUM_OF_LINES) && (NUM_OF_COLS * i < count);
		     i++) {
			strncpy(line_buf, &tmp[NUM_OF_COLS * i], NUM_OF_COLS);
			lcd_gotoxy(0, i);
			lcd_puts(line_buf);
			memset(line_buf, 0, NUM_OF_COLS);
		}
	}

	kfree(tmp);
	kfree(line_buf);

	return n;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * my_ioctl - callback ioctl
 * @file:        file pointer
 * @cmd:
 * @arg:
 */
static long cdev_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	long status = 0;
	struct cursor_pos tmp_pos;
	uint8_t tmp_char[DOTS_PER_COL + 1];

	if (_IOC_TYPE(cmd) != IOC_MAGIC) {
		status = -ENOTTY;
		goto ioctl_err;
	}

	if (_IOC_NR(cmd) > IOC_MAXNR) {
		status = -ENOTTY;
		goto ioctl_err;
	}

	switch (cmd) {
	case LCD_CLEAR:
		lcd_clear();
		pr_info("LCD was cleared\n");
		break;

	case LCD_HOME:
		lcd_home();
		pr_info("lcd_home()\n");
		break;

	case LCD_GOTOXY:

		status = copy_from_user(&tmp_pos, (struct cursor_pos *)arg,
					sizeof(tmp_pos));
		if (status != 0) {
			status = -1;
			goto ioctl_err;
		}

		if (tmp_pos.x >= NUM_OF_COLS || tmp_pos.y >= NUM_OF_LINES) {
			status = -1;
			goto ioctl_err;
		}

		lcd_gotoxy(tmp_pos.x, tmp_pos.y);
		pr_info("lcd_gotoxy()\n");
		break;

	case LCD_CURSOR:
		lcd_cursor((bool)arg);
		pr_info("lcd_cursor(%d)\n", (bool)arg);
		break;

	case LCD_CURSOR_BLINK:
		lcd_cursor_blink((bool)arg);
		pr_info("lcd_cursor_blink(%d)\n", (bool)arg);
		break;

	case LCD_SHIFT_DISPLAY_LEFT:
		lcd_shift_display_left();
		pr_info("lcd_shift_display_left()\n");
		break;

	case LCD_SHIFT_DISPLAY_RIGHT:
		lcd_shift_display_right();
		pr_info("lcd_shift_display_right()\n");
		break;

	case LCD_CREATE_CHAR:

		status = copy_from_user(tmp_char, (uint8_t *)arg,
					DOTS_PER_COL + 1);
		if (status != 0) {
			status = -1;
			goto ioctl_err;
		}

		if (tmp_char[DOTS_PER_COL] <= NUM_OF_CUSTOM_CHAR) {
			lcd_create_char(tmp_char, tmp_char[DOTS_PER_COL]);
			pr_info("lcd_create_char()\n");
		} else {
			status = -1;
			goto ioctl_err;
		}

		break;

	case LCD_BACKLIGHT:
		lcd_backlight((bool)arg);
		pr_info("lcd_backlight(%d)\n", (bool)arg);
		break;

	case LCD_PUT_CHAR:
		lcd_put_char((char)arg);
		pr_info("lcd_put_char(%c)\n", (char)arg);
		break;

	default:
		pr_info("default...\n");
		status = -1;
		goto ioctl_err;
	}

ioctl_err:
	return status;
}
