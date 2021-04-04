// SPDX-License-Identifier: MIT and GPL
#include <linux/module.h> // required by all modules
#include <linux/kernel.h> // required for sysinfo
#include <linux/init.h> // used by module_init, module_exit macros
#include <linux/jiffies.h> // where jiffies and its helpers reside
#include <linux/fs.h>
#include <linux/list.h>
#include <linux/errno.h>
#include <linux/slab.h>
#include <linux/cdev.h>
#include <linux/debugfs.h>
#include <linux/ioctl.h>
#include <linux/device.h>
#include <linux/kdev_t.h>
#include <linux/spinlock.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>

#include "lcd.h"
#include "cdev.h"

#define DEV_COMPATIBLE "lcd_dev"

MODULE_DESCRIPTION("Basic module demo: working with LCD");
MODULE_AUTHOR("AlexShlikhta oleksandr.shlikhta@globallogic.com");
MODULE_VERSION("0.1");
MODULE_LICENSE("Dual MIT/GPL"); // this affects the kernel behavior

struct i2c_client *lcd_i2c_client;
struct cdev_reg_t device;

///////////////////////////////////////////////////////////////////////////////////////////////////

int lcd_probe(struct i2c_client *drv_client, const struct i2c_device_id *id)
{
	int status = 0;

	lcd_i2c_client = drv_client;

	device.name = kstrdup("lcd", GFP_KERNEL);

	status = cdev_reg(&device);
	if (IS_ERR_VALUE(status))
		goto probe_err;

	lcd_init(NUM_OF_LINES);

	lcd_home();

	lcd_backlight(ON);

	lcd_puts("Zaebalo blyat");

	dev_info(&drv_client->dev, "connected\n");

probe_err:
	return status;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

int lcd_remove(struct i2c_client *drv_client)
{
	lcd_home();
	lcd_clear();
	lcd_backlight(OFF);
	lcd_i2c_client = NULL;

	dev_info(&drv_client->dev, "disconnected\n");

	return 0;
}

static const struct i2c_device_id lcd_id[] = { { DEV_COMPATIBLE, 0 }, {} };
MODULE_DEVICE_TABLE(i2c, lcd_id);

static const struct of_device_id lcd_of_match[] = { { .compatible =
							      DEV_COMPATIBLE },
						    {} };
MODULE_DEVICE_TABLE(of, lcd_of_match);

static struct i2c_driver lcd_driver = {
	.driver = {
			.name = DEV_COMPATIBLE,
			.of_match_table = lcd_of_match,
		},
	.probe = lcd_probe,
	.remove = lcd_remove,
	.id_table = lcd_id,
};

///////////////////////////////////////////////////////////////////////////////
//###########################INIT CALLBACK#####################################
///////////////////////////////////////////////////////////////////////////////

int __init mod_init(void)
{
	pr_info("'%s' module initialized\n", THIS_MODULE->name);

	return i2c_add_driver(&lcd_driver);
}

///////////////////////////////////////////////////////////////////////////////
//###########################EXIT CALLBACK#####################################
///////////////////////////////////////////////////////////////////////////////

void __exit mod_cleanup(void)
{
	i2c_del_driver(&lcd_driver);

	cdev_unreg(&device);

	pr_info("'%s' module released\n", THIS_MODULE->name);
}

module_init(mod_init);
module_exit(mod_cleanup);
