#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/printk.h>

static int __init oplus_kpm_init(void) {
    pr_info("OnePlus 9 Optimizer KPM Loaded Successfully!\n");
    return 0;
}

static void __exit oplus_kpm_exit(void) {
    pr_info("OnePlus 9 Optimizer KPM Unloaded!\n");
}

module_init(oplus_kpm_init);
module_exit(oplus_kpm_exit);

MODULE_LICENSE("GPL v2");
MODULE_AUTHOR("Dev");
MODULE_DESCRIPTION("OnePlus 9 Optimizer KPM");
