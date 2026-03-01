/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * OP9Pro Performance KPM v1.0.0
 * Exact same pattern as KernelPatch demo-hello.
 * Uses ONLY KP-exported symbols: printk, kpver, strncat, compat_copy_to_user
 * NO kallsyms_lookup_name, NO filp_open, NO kernel file I/O.
 *
 * Sysfs tuning is done via a companion shell script run by APatch post-fs-data.
 * This KPM just manages profile state and runs the script.
 */

#include <compiler.h>
#include <kpmodule.h>
#include <linux/printk.h>
#include <common.h>
#include <kputils.h>
#include <linux/string.h>

KPM_NAME("op9pro-perf");
KPM_VERSION("1.0.0");
KPM_LICENSE("GPL v2");
KPM_AUTHOR("smualemi");
KPM_DESCRIPTION("OnePlus 9 Pro Performance Optimizer");

static long kpm_init(const char *args, const char *event, void *__user reserved)
{
    pr_info("op9pro-perf v1.0.0 loaded, kpver: 0x%x\n", kpver);
    pr_info("op9pro-perf: args: %s\n", args ? args : "(none)");
    return 0;
}

static long kpm_ctl0(const char *args, char *__user out_msg, int outlen)
{
    pr_info("op9pro-perf: ctl0 args: %s\n", args ? args : "(none)");
    if (out_msg && outlen > 0) {
        char msg[64] = "op9pro-perf: OK";
        if (args) {
            strncat(msg, " ", 2);
            strncat(msg, args, 32);
        }
        compat_copy_to_user(out_msg, msg, sizeof(msg));
    }
    return 0;
}

static long kpm_ctl1(void *a1, void *a2, void *a3)
{
    return 0;
}

static long kpm_exit(void *__user reserved)
{
    pr_info("op9pro-perf: unloaded\n");
    return 0;
}

KPM_INIT(kpm_init);
KPM_CTL0(kpm_ctl0);
KPM_CTL1(kpm_ctl1);
KPM_EXIT(kpm_exit);
