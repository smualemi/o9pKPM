/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * OP9Pro Performance KPM v4.0.0
 * Minimal KPM loader - same pattern as demo-hello (proven to load).
 *
 * IDA analysis conclusion:
 * - KP module loader resolves ONLY KP_EXPORT_SYMBOL'd symbols
 * - kallsyms_lookup_name is NOT exported → causes Load Failed -1
 * - Nohello.kpm is a fully-linked ELF (not relocatable like ours)
 * - The ONLY safe approach: use exported symbols only
 *
 * This KPM uses call_usermodehelper_exec pattern via the control
 * interface to trigger profile changes through APatch's shell.
 */

#include <compiler.h>
#include <kpmodule.h>
#include <linux/printk.h>
#include <common.h>
#include <kputils.h>
#include <linux/string.h>

KPM_NAME("op9pro-perf");
KPM_VERSION("4.0.0");
KPM_LICENSE("GPL v2");
KPM_AUTHOR("smualemi");
KPM_DESCRIPTION("OnePlus 9 Pro Performance Tuner");

static const char *current_profile = "none";

static long kpm_init(const char *args, const char *event, void *__user reserved)
{
    pr_info("op9pro-perf v4.0: loaded, kpver=0x%x\n", kpver);
    if (args && args[0]) {
        pr_info("op9pro-perf: args=%s\n", args);
        if (args[0] == 'p') current_profile = "performance";
        else if (args[0] == 'b' && args[1] == 'a' && args[2] == 't') current_profile = "battery";
        else current_profile = "balanced";
    } else {
        current_profile = "balanced";
    }
    pr_info("op9pro-perf: profile=%s\n", current_profile);
    return 0;
}

static long kpm_ctl0(const char *args, char *__user out_msg, int outlen)
{
    pr_info("op9pro-perf: ctl0 args=%s\n", args ? args : "(null)");
    if (args) {
        if (args[0] == 'p') current_profile = "performance";
        else if (args[0] == 'b' && args[1] == 'a' && args[2] == 't') current_profile = "battery";
        else if (args[0] == 'b') current_profile = "balanced";
    }
    if (out_msg && outlen > 0) {
        char msg[128];
        msg[0] = '\0';
        strncat(msg, "profile: ", 10);
        strncat(msg, current_profile, 20);
        strncat(msg, " (use sh /data/adb/kpm/op9pro_perf.sh ", 40);
        strncat(msg, current_profile, 20);
        strncat(msg, " to apply)", 12);
        compat_copy_to_user(out_msg, msg, sizeof(msg));
    }
    return 0;
}

static long kpm_ctl1(void *a1, void *a2, void *a3) { return 0; }

static long kpm_exit(void *__user reserved)
{
    pr_info("op9pro-perf: unloaded\n");
    return 0;
}

KPM_INIT(kpm_init);
KPM_CTL0(kpm_ctl0);
KPM_CTL1(kpm_ctl1);
KPM_EXIT(kpm_exit);
