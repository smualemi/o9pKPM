/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * OP9Pro Performance KPM v2.1.0
 * All tuning built into the KPM - no shell script needed.
 *
 * Uses ksyms.h (NOT syscall.h) to get kallsyms_lookup_name
 * without pulling in unresolvable extern symbols.
 */

#include <compiler.h>
#include <kpmodule.h>
#include <linux/printk.h>
#include <common.h>
#include <kputils.h>
#include <ksyms.h>
#include <linux/string.h>

KPM_NAME("op9pro-perf");
KPM_VERSION("2.1.0");
KPM_LICENSE("GPL v2");
KPM_AUTHOR("smualemi");
KPM_DESCRIPTION("OnePlus 9 Pro Performance Optimizer");

/* Profiles */
#define PROF_PERF  0
#define PROF_BAL   1
#define PROF_BAT   2
static int cur_profile = PROF_BAL;

/* Kernel file ops - plain globals like demo-syscallhook */
void *(*k_filp_open)(const char *, int, unsigned short) = 0;
long (*k_kernel_write)(void *, const void *, unsigned long, long long *) = 0;
int (*k_filp_close)(void *, void *) = 0;

static void sysfs_write(const char *path, const char *val)
{
    void *f;
    long long pos = 0;
    unsigned long len = 0;
    const char *p = val;
    while (*p++) len++;

    f = k_filp_open(path, 0x0001 | 0x0200, 0);
    if (!f || (long)f < 0) return;
    k_kernel_write(f, val, len, &pos);
    k_filp_close(f, 0);
}

/* === CPU === */
static void cpu_perf(void)
{
    sysfs_write("/sys/devices/system/cpu/cpufreq/policy0/scaling_governor", "schedutil");
    sysfs_write("/sys/devices/system/cpu/cpufreq/policy0/schedutil/rate_limit_us", "500");
    sysfs_write("/sys/devices/system/cpu/cpufreq/policy0/schedutil/hispeed_freq", "1612800");
    sysfs_write("/sys/devices/system/cpu/cpufreq/policy0/scaling_min_freq", "691200");
    sysfs_write("/sys/devices/system/cpu/cpufreq/policy4/scaling_governor", "schedutil");
    sysfs_write("/sys/devices/system/cpu/cpufreq/policy4/schedutil/rate_limit_us", "500");
    sysfs_write("/sys/devices/system/cpu/cpufreq/policy4/schedutil/hispeed_freq", "2131200");
    sysfs_write("/sys/devices/system/cpu/cpufreq/policy4/scaling_min_freq", "710400");
    sysfs_write("/sys/devices/system/cpu/cpufreq/policy7/scaling_governor", "schedutil");
    sysfs_write("/sys/devices/system/cpu/cpufreq/policy7/schedutil/rate_limit_us", "500");
    sysfs_write("/sys/devices/system/cpu/cpufreq/policy7/schedutil/hispeed_freq", "2649600");
    sysfs_write("/sys/devices/system/cpu/cpufreq/policy7/scaling_min_freq", "844800");
    sysfs_write("/proc/sys/kernel/sched_upmigrate", "80");
    sysfs_write("/proc/sys/kernel/sched_downmigrate", "60");
    sysfs_write("/proc/sys/kernel/sched_boost", "1");
}
static void cpu_bal(void)
{
    sysfs_write("/sys/devices/system/cpu/cpufreq/policy0/scaling_governor", "schedutil");
    sysfs_write("/sys/devices/system/cpu/cpufreq/policy0/schedutil/rate_limit_us", "1000");
    sysfs_write("/sys/devices/system/cpu/cpufreq/policy0/schedutil/hispeed_freq", "1324800");
    sysfs_write("/sys/devices/system/cpu/cpufreq/policy0/scaling_min_freq", "300000");
    sysfs_write("/sys/devices/system/cpu/cpufreq/policy4/scaling_governor", "schedutil");
    sysfs_write("/sys/devices/system/cpu/cpufreq/policy4/schedutil/rate_limit_us", "1000");
    sysfs_write("/sys/devices/system/cpu/cpufreq/policy4/schedutil/hispeed_freq", "1766400");
    sysfs_write("/sys/devices/system/cpu/cpufreq/policy4/scaling_min_freq", "710400");
    sysfs_write("/sys/devices/system/cpu/cpufreq/policy7/scaling_governor", "schedutil");
    sysfs_write("/sys/devices/system/cpu/cpufreq/policy7/schedutil/rate_limit_us", "1000");
    sysfs_write("/sys/devices/system/cpu/cpufreq/policy7/schedutil/hispeed_freq", "2188800");
    sysfs_write("/sys/devices/system/cpu/cpufreq/policy7/scaling_min_freq", "844800");
    sysfs_write("/proc/sys/kernel/sched_upmigrate", "90");
    sysfs_write("/proc/sys/kernel/sched_downmigrate", "75");
    sysfs_write("/proc/sys/kernel/sched_boost", "0");
}
static void cpu_bat(void)
{
    sysfs_write("/sys/devices/system/cpu/cpufreq/policy0/scaling_governor", "schedutil");
    sysfs_write("/sys/devices/system/cpu/cpufreq/policy0/schedutil/rate_limit_us", "2000");
    sysfs_write("/sys/devices/system/cpu/cpufreq/policy0/schedutil/hispeed_freq", "1017600");
    sysfs_write("/sys/devices/system/cpu/cpufreq/policy0/scaling_min_freq", "300000");
    sysfs_write("/sys/devices/system/cpu/cpufreq/policy4/scaling_governor", "schedutil");
    sysfs_write("/sys/devices/system/cpu/cpufreq/policy4/schedutil/rate_limit_us", "2000");
    sysfs_write("/sys/devices/system/cpu/cpufreq/policy4/schedutil/hispeed_freq", "1267200");
    sysfs_write("/sys/devices/system/cpu/cpufreq/policy4/scaling_min_freq", "710400");
    sysfs_write("/sys/devices/system/cpu/cpufreq/policy7/scaling_governor", "schedutil");
    sysfs_write("/sys/devices/system/cpu/cpufreq/policy7/schedutil/rate_limit_us", "4000");
    sysfs_write("/sys/devices/system/cpu/cpufreq/policy7/schedutil/hispeed_freq", "1574400");
    sysfs_write("/sys/devices/system/cpu/cpufreq/policy7/scaling_min_freq", "844800");
    sysfs_write("/proc/sys/kernel/sched_upmigrate", "95");
    sysfs_write("/proc/sys/kernel/sched_downmigrate", "85");
    sysfs_write("/proc/sys/kernel/sched_boost", "0");
}

/* === Memory === */
static void mem_perf(void)
{
    sysfs_write("/proc/sys/vm/swappiness", "60");
    sysfs_write("/proc/sys/vm/dirty_ratio", "30");
    sysfs_write("/proc/sys/vm/dirty_background_ratio", "10");
    sysfs_write("/proc/sys/vm/dirty_expire_centisecs", "3000");
    sysfs_write("/proc/sys/vm/dirty_writeback_centisecs", "500");
    sysfs_write("/proc/sys/vm/vfs_cache_pressure", "50");
    sysfs_write("/proc/sys/vm/page-cluster", "0");
    sysfs_write("/proc/sys/vm/min_free_kbytes", "16384");
}
static void mem_bal(void)
{
    sysfs_write("/proc/sys/vm/swappiness", "100");
    sysfs_write("/proc/sys/vm/dirty_ratio", "20");
    sysfs_write("/proc/sys/vm/dirty_background_ratio", "5");
    sysfs_write("/proc/sys/vm/dirty_expire_centisecs", "2000");
    sysfs_write("/proc/sys/vm/dirty_writeback_centisecs", "500");
    sysfs_write("/proc/sys/vm/vfs_cache_pressure", "100");
    sysfs_write("/proc/sys/vm/page-cluster", "0");
    sysfs_write("/proc/sys/vm/min_free_kbytes", "11584");
}
static void mem_bat(void)
{
    sysfs_write("/proc/sys/vm/swappiness", "150");
    sysfs_write("/proc/sys/vm/dirty_ratio", "10");
    sysfs_write("/proc/sys/vm/dirty_background_ratio", "3");
    sysfs_write("/proc/sys/vm/dirty_expire_centisecs", "1000");
    sysfs_write("/proc/sys/vm/dirty_writeback_centisecs", "300");
    sysfs_write("/proc/sys/vm/vfs_cache_pressure", "150");
    sysfs_write("/proc/sys/vm/page-cluster", "3");
    sysfs_write("/proc/sys/vm/min_free_kbytes", "8192");
}

/* === I/O === */
static void io_perf(void)
{
    sysfs_write("/sys/block/sda/queue/scheduler", "mq-deadline");
    sysfs_write("/sys/block/sda/queue/read_ahead_kb", "512");
    sysfs_write("/sys/block/sda/queue/nr_requests", "256");
    sysfs_write("/sys/block/sda/queue/iostats", "0");
    sysfs_write("/sys/block/sda/queue/add_random", "0");
    sysfs_write("/sys/block/sda/queue/rq_affinity", "2");
    sysfs_write("/sys/block/sdb/queue/scheduler", "mq-deadline");
    sysfs_write("/sys/block/sdb/queue/read_ahead_kb", "512");
    sysfs_write("/sys/block/sdb/queue/iostats", "0");
}
static void io_bal(void)
{
    sysfs_write("/sys/block/sda/queue/scheduler", "bfq");
    sysfs_write("/sys/block/sda/queue/read_ahead_kb", "256");
    sysfs_write("/sys/block/sda/queue/nr_requests", "128");
    sysfs_write("/sys/block/sda/queue/iostats", "0");
    sysfs_write("/sys/block/sda/queue/add_random", "0");
    sysfs_write("/sys/block/sda/queue/rq_affinity", "1");
    sysfs_write("/sys/block/sdb/queue/scheduler", "bfq");
    sysfs_write("/sys/block/sdb/queue/read_ahead_kb", "256");
    sysfs_write("/sys/block/sdb/queue/iostats", "0");
}
static void io_bat(void)
{
    sysfs_write("/sys/block/sda/queue/scheduler", "bfq");
    sysfs_write("/sys/block/sda/queue/read_ahead_kb", "128");
    sysfs_write("/sys/block/sda/queue/nr_requests", "64");
    sysfs_write("/sys/block/sda/queue/iostats", "0");
    sysfs_write("/sys/block/sda/queue/add_random", "0");
    sysfs_write("/sys/block/sda/queue/rq_affinity", "1");
    sysfs_write("/sys/block/sdb/queue/scheduler", "bfq");
    sysfs_write("/sys/block/sdb/queue/read_ahead_kb", "128");
    sysfs_write("/sys/block/sdb/queue/iostats", "0");
}

/* === Network === */
static void net_perf(void)
{
    sysfs_write("/proc/sys/net/core/rmem_max", "8388608");
    sysfs_write("/proc/sys/net/core/wmem_max", "8388608");
    sysfs_write("/proc/sys/net/ipv4/tcp_fastopen", "3");
    sysfs_write("/proc/sys/net/ipv4/tcp_keepalive_time", "300");
    sysfs_write("/proc/sys/net/ipv4/tcp_timestamps", "1");
    sysfs_write("/proc/sys/net/ipv4/tcp_sack", "1");
    sysfs_write("/proc/sys/net/ipv4/tcp_window_scaling", "1");
    sysfs_write("/proc/sys/net/core/netdev_budget", "600");
    sysfs_write("/proc/sys/net/core/netdev_max_backlog", "5000");
}
static void net_bal(void)
{
    sysfs_write("/proc/sys/net/core/rmem_max", "4194304");
    sysfs_write("/proc/sys/net/core/wmem_max", "4194304");
    sysfs_write("/proc/sys/net/ipv4/tcp_fastopen", "3");
    sysfs_write("/proc/sys/net/ipv4/tcp_keepalive_time", "600");
    sysfs_write("/proc/sys/net/core/netdev_budget", "300");
    sysfs_write("/proc/sys/net/core/netdev_max_backlog", "2500");
}
static void net_bat(void)
{
    sysfs_write("/proc/sys/net/core/rmem_max", "2097152");
    sysfs_write("/proc/sys/net/core/wmem_max", "2097152");
    sysfs_write("/proc/sys/net/ipv4/tcp_fastopen", "3");
    sysfs_write("/proc/sys/net/ipv4/tcp_keepalive_time", "1200");
    sysfs_write("/proc/sys/net/core/netdev_budget", "150");
    sysfs_write("/proc/sys/net/core/netdev_max_backlog", "1000");
}

/* === GPU/Power === */
static void pwr_perf(void)
{
    sysfs_write("/sys/module/workqueue/parameters/power_efficient", "N");
    sysfs_write("/sys/class/kgsl/kgsl-3d0/default_pwrlevel", "0");
    sysfs_write("/sys/class/kgsl/kgsl-3d0/force_clk_on", "1");
    sysfs_write("/sys/class/kgsl/kgsl-3d0/idle_timer", "10000");
    sysfs_write("/sys/class/kgsl/kgsl-3d0/throttling", "0");
}
static void pwr_bal(void)
{
    sysfs_write("/sys/module/workqueue/parameters/power_efficient", "N");
    sysfs_write("/sys/class/kgsl/kgsl-3d0/default_pwrlevel", "4");
    sysfs_write("/sys/class/kgsl/kgsl-3d0/force_clk_on", "0");
    sysfs_write("/sys/class/kgsl/kgsl-3d0/idle_timer", "64");
    sysfs_write("/sys/class/kgsl/kgsl-3d0/throttling", "1");
}
static void pwr_bat(void)
{
    sysfs_write("/sys/module/workqueue/parameters/power_efficient", "Y");
    sysfs_write("/sys/class/kgsl/kgsl-3d0/default_pwrlevel", "6");
    sysfs_write("/sys/class/kgsl/kgsl-3d0/force_clk_on", "0");
    sysfs_write("/sys/class/kgsl/kgsl-3d0/idle_timer", "32");
    sysfs_write("/sys/class/kgsl/kgsl-3d0/throttling", "1");
}

/* === Apply === */
static void apply(int p)
{
    if (p == PROF_PERF) { cpu_perf(); mem_perf(); io_perf(); net_perf(); pwr_perf(); }
    else if (p == PROF_BAT) { cpu_bat(); mem_bat(); io_bat(); net_bat(); pwr_bat(); }
    else { cpu_bal(); mem_bal(); io_bal(); net_bal(); pwr_bal(); }
    cur_profile = p;
}

/* === KPM Callbacks === */
static long kpm_init(const char *args, const char *event, void *__user reserved)
{
    pr_info("op9pro-perf v2.1.0 init, kpver: 0x%x\n", kpver);

    k_filp_open = (typeof(k_filp_open))kallsyms_lookup_name("filp_open");
    k_kernel_write = (typeof(k_kernel_write))kallsyms_lookup_name("kernel_write");
    k_filp_close = (typeof(k_filp_close))kallsyms_lookup_name("filp_close");

    pr_info("op9pro-perf: filp_open=%llx kernel_write=%llx filp_close=%llx\n",
            (unsigned long long)k_filp_open,
            (unsigned long long)k_kernel_write,
            (unsigned long long)k_filp_close);

    if (!k_filp_open || !k_kernel_write || !k_filp_close) {
        pr_err("op9pro-perf: failed to resolve kernel funcs\n");
        return -1;
    }

    int p = PROF_BAL;
    if (args && args[0] == 'p') p = PROF_PERF;
    else if (args && args[0] == 'b' && args[1] == 'a' && args[2] == 't') p = PROF_BAT;

    pr_info("op9pro-perf: applying profile %d\n", p);
    apply(p);
    pr_info("op9pro-perf: done\n");
    return 0;
}

static long kpm_ctl0(const char *args, char *__user out_msg, int outlen)
{
    if (!args) return 0;
    pr_info("op9pro-perf: ctl0 args=%s\n", args);

    if (args[0] == 'p') apply(PROF_PERF);
    else if (args[0] == 'b' && args[1] == 'a' && args[2] == 't') apply(PROF_BAT);
    else if (args[0] == 'b') apply(PROF_BAL);
    else if (args[0] == 's') {
        char msg[64] = "profile: ";
        if (cur_profile == PROF_PERF) strncat(msg, "performance", 20);
        else if (cur_profile == PROF_BAT) strncat(msg, "battery", 20);
        else strncat(msg, "balanced", 20);
        compat_copy_to_user(out_msg, msg, sizeof(msg));
    }
    return 0;
}

static long kpm_ctl1(void *a1, void *a2, void *a3) { return 0; }

static long kpm_exit(void *__user reserved)
{
    pr_info("op9pro-perf: restoring balanced\n");
    if (k_filp_open && k_kernel_write && k_filp_close) apply(PROF_BAL);
    pr_info("op9pro-perf: unloaded\n");
    return 0;
}

KPM_INIT(kpm_init);
KPM_CTL0(kpm_ctl0);
KPM_CTL1(kpm_ctl1);
KPM_EXIT(kpm_exit);
