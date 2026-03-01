#include "kpmodule.h"
#include <linux/printk.h>

KPM_NAME("oplus_optimize");
KPM_VERSION("1.0.0");
KPM_LICENSE("GPL v2");
KPM_AUTHOR("Dev");
KPM_DESCRIPTION("OnePlus 9 Optimizer KPM");

static long oplus_kpm_init(const char *args, const char *event, void *__user reserved) {
    pr_info("OnePlus 9 Optimizer KPM Loaded Successfully!\n");
    return 0;
}

static long oplus_kpm_exit(void *__user reserved) {
    pr_info("OnePlus 9 Optimizer KPM Unloaded!\n");
    return 0;
}

KPM_INIT(oplus_kpm_init);
KPM_EXIT(oplus_kpm_exit);
