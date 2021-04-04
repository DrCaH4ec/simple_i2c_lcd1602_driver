#ifndef _CDEV_H_
#define _CDEV_H_

#include <linux/module.h> // required by all modules
#include <linux/device.h>
#include <linux/fs.h>
#include <linux/ioctl.h>

#include "lcd.h"
#include "lcd_cdev_def.h"


struct alloc_t {
    unsigned reg_cdev : 1;
    unsigned cl_create : 1;
    unsigned dev_create : 1;
};

struct cdev_reg_t {
    struct alloc_t alloc;
    int32_t major_num;
    struct class *cl;
    struct device *pdev;
    char *name;
};

int cdev_reg(struct cdev_reg_t *cdev);

void cdev_unreg(struct cdev_reg_t *cdev);



/*_CDEV_H_*/
#endif
