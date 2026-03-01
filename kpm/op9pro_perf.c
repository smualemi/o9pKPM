/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright (C) 2024 smualemi. All Rights Reserved.
 *
 * OP9Pro Performance KPM - KernelPatch Module for OnePlus 9 Pro (SM8350)
 * Compatible with OxygenOS 14 QGKI Kernel
 *
 * Features:
 *   - CPU Frequency tuning (schedutil/UAG parameters per cluster)
 *   - Memory management optimization (vm tunables, ZRAM)
 *   - I/O scheduler tuning (BFQ/deadline, read_ahead)
 *   - Network optimization (TCP buffers, congestion)
 *   - Power management (workqueue efficiency, GPU)
 *   - 3 profiles: performance / balanced / battery
 */

#include <compiler.h>
#include <kpmodule.h>
#include <common.h>
#include <log.h>
#include <linux/printk.h>
#include <linux/string.h>
#include <kallsyms.h>

KPM_NAME("op9pro-perf");
KPM_VERSION("1.0.0");
KPM_LICENSE("GPL v2");
KPM_AUTHOR("smualemi");
KPM_DESCRIPTION("OnePlus 9 Pro Performance Optimizer for APatch");

/* ============================================================
 *  Profile Definitions
 *
 *  SM8350 (Lahaina) Cluster Layout:
 *    Cluster 0 (Silver): CPU 0-3  - Cortex-A55
 *    Cluster 1 (Gold):   CPU 4-6  - Cortex-A78
 *    Cluster 2 (Prime):  CPU 7    - Cortex-X1
 * ============================================================ */

#define PROFILE_PERFORMANCE  0
#define PROFILE_BALANCED     1
#define PROFILE_BATTERY      2

static int current_profile = PROFILE_BALANCED;

/* Kernel function pointers resolved via kallsyms */
typedef void *(*filp_open_func_t)(const char *, int, unsigned short);
typedef int (*kernel_write_func_t)(void *, const void *, unsigned long, long long *);
typedef int (*filp_close_func_t)(void *, void *);

static filp_open_func_t k_filp_open = 0;
static kernel_write_func_t k_kernel_write = 0;
static filp_close_func_t k_filp_close = 0;
static int funcs_resolved = 0;

/* Resolve kernel functions once */
static int resolve_kernel_funcs(void)
{
    if (funcs_resolved) return 0;

    k_filp_open = (filp_open_func_t)kallsyms_lookup_name("filp_open");
    k_kernel_write = (kernel_write_func_t)kallsyms_lookup_name("kernel_write");
    k_filp_close = (filp_close_func_t)kallsyms_lookup_name("filp_close");

    if (!k_filp_open || !k_kernel_write || !k_filp_close) {
        logke("op9pro-perf: failed to resolve kernel funcs\n");
        logke("  filp_open=%llx kernel_write=%llx filp_close=%llx\n",
              (unsigned long long)k_filp_open,
              (unsigned long long)k_kernel_write,
              (unsigned long long)k_filp_close);
        return -1;
    }

    logki("op9pro-perf: kernel funcs resolved OK\n");
    funcs_resolved = 1;
    return 0;
}

/* ============================================================
 *  Sysfs/procfs write helper
 * ============================================================ */
static int write_sysfs(const char *path, const char *value)
{
    if (!funcs_resolved) {
        if (resolve_kernel_funcs() != 0) return -1;
    }

    void *filp = k_filp_open(path, 0x0001 | 0x0200, 0); /* O_WRONLY | O_TRUNC */
    if (!filp || (long)filp < 0) {
        return -1;
    }

    long long pos = 0;
    unsigned long len = 0;
    const char *p = value;
    while (*p++) len++;

    int ret = k_kernel_write(filp, value, len, &pos);
    k_filp_close(filp, 0);

    return (ret < 0) ? ret : 0;
}

/* ============================================================
 *  CPU Frequency Tuning
 * ============================================================ */
static void apply_cpu_performance(void)
{
    logki("op9pro-perf: CPU -> performance\n");

    /* Silver cluster (CPU 0-3) */
    write_sysfs("/sys/devices/system/cpu/cpufreq/policy0/scaling_governor", "schedutil");
    write_sysfs("/sys/devices/system/cpu/cpufreq/policy0/schedutil/rate_limit_us", "500");
    write_sysfs("/sys/devices/system/cpu/cpufreq/policy0/schedutil/hispeed_freq", "1612800");
    write_sysfs("/sys/devices/system/cpu/cpufreq/policy0/scaling_min_freq", "691200");

    /* Gold cluster (CPU 4-6) */
    write_sysfs("/sys/devices/system/cpu/cpufreq/policy4/scaling_governor", "schedutil");
    write_sysfs("/sys/devices/system/cpu/cpufreq/policy4/schedutil/rate_limit_us", "500");
    write_sysfs("/sys/devices/system/cpu/cpufreq/policy4/schedutil/hispeed_freq", "2131200");
    write_sysfs("/sys/devices/system/cpu/cpufreq/policy4/scaling_min_freq", "710400");

    /* Prime cluster (CPU 7) */
    write_sysfs("/sys/devices/system/cpu/cpufreq/policy7/scaling_governor", "schedutil");
    write_sysfs("/sys/devices/system/cpu/cpufreq/policy7/schedutil/rate_limit_us", "500");
    write_sysfs("/sys/devices/system/cpu/cpufreq/policy7/schedutil/hispeed_freq", "2649600");
    write_sysfs("/sys/devices/system/cpu/cpufreq/policy7/scaling_min_freq", "844800");

    write_sysfs("/proc/sys/kernel/sched_upmigrate", "80");
    write_sysfs("/proc/sys/kernel/sched_downmigrate", "60");
    write_sysfs("/proc/sys/kernel/sched_boost", "1");
}

static void apply_cpu_balanced(void)
{
    logki("op9pro-perf: CPU -> balanced\n");

    write_sysfs("/sys/devices/system/cpu/cpufreq/policy0/scaling_governor", "schedutil");
    write_sysfs("/sys/devices/system/cpu/cpufreq/policy0/schedutil/rate_limit_us", "1000");
    write_sysfs("/sys/devices/system/cpu/cpufreq/policy0/schedutil/hispeed_freq", "1324800");
    write_sysfs("/sys/devices/system/cpu/cpufreq/policy0/scaling_min_freq", "300000");

    write_sysfs("/sys/devices/system/cpu/cpufreq/policy4/scaling_governor", "schedutil");
    write_sysfs("/sys/devices/system/cpu/cpufreq/policy4/schedutil/rate_limit_us", "1000");
    write_sysfs("/sys/devices/system/cpu/cpufreq/policy4/schedutil/hispeed_freq", "1766400");
    write_sysfs("/sys/devices/system/cpu/cpufreq/policy4/scaling_min_freq", "710400");

    write_sysfs("/sys/devices/system/cpu/cpufreq/policy7/scaling_governor", "schedutil");
    write_sysfs("/sys/devices/system/cpu/cpufreq/policy7/schedutil/rate_limit_us", "1000");
    write_sysfs("/sys/devices/system/cpu/cpufreq/policy7/schedutil/hispeed_freq", "2188800");
    write_sysfs("/sys/devices/system/cpu/cpufreq/policy7/scaling_min_freq", "844800");

    write_sysfs("/proc/sys/kernel/sched_upmigrate", "90");
    write_sysfs("/proc/sys/kernel/sched_downmigrate", "75");
    write_sysfs("/proc/sys/kernel/sched_boost", "0");
}

static void apply_cpu_battery(void)
{
    logki("op9pro-perf: CPU -> battery\n");

    write_sysfs("/sys/devices/system/cpu/cpufreq/policy0/scaling_governor", "schedutil");
    write_sysfs("/sys/devices/system/cpu/cpufreq/policy0/schedutil/rate_limit_us", "2000");
    write_sysfs("/sys/devices/system/cpu/cpufreq/policy0/schedutil/hispeed_freq", "1017600");
    write_sysfs("/sys/devices/system/cpu/cpufreq/policy0/scaling_min_freq", "300000");

    write_sysfs("/sys/devices/system/cpu/cpufreq/policy4/scaling_governor", "schedutil");
    write_sysfs("/sys/devices/system/cpu/cpufreq/policy4/schedutil/rate_limit_us", "2000");
    write_sysfs("/sys/devices/system/cpu/cpufreq/policy4/schedutil/hispeed_freq", "1267200");
    write_sysfs("/sys/devices/system/cpu/cpufreq/policy4/scaling_min_freq", "710400");

    write_sysfs("/sys/devices/system/cpu/cpufreq/policy7/scaling_governor", "schedutil");
    write_sysfs("/sys/devices/system/cpu/cpufreq/policy7/schedutil/rate_limit_us", "4000");
    write_sysfs("/sys/devices/system/cpu/cpufreq/policy7/schedutil/hispeed_freq", "1574400");
    write_sysfs("/sys/devices/system/cpu/cpufreq/policy7/scaling_min_freq", "844800");

    write_sysfs("/proc/sys/kernel/sched_upmigrate", "95");
    write_sysfs("/proc/sys/kernel/sched_downmigrate", "85");
    write_sysfs("/proc/sys/kernel/sched_boost", "0");
}

/* ============================================================
 *  Memory Tuning
 * ============================================================ */
static void apply_mem_performance(void)
{
    logki("op9pro-perf: MEM -> performance\n");
    write_sysfs("/proc/sys/vm/swappiness", "60");
    write_sysfs("/proc/sys/vm/dirty_ratio", "30");
    write_sysfs("/proc/sys/vm/dirty_background_ratio", "10");
    write_sysfs("/proc/sys/vm/dirty_expire_centisecs", "3000");
    write_sysfs("/proc/sys/vm/dirty_writeback_centisecs", "500");
    write_sysfs("/proc/sys/vm/vfs_cache_pressure", "50");
    write_sysfs("/proc/sys/vm/page-cluster", "0");
    write_sysfs("/proc/sys/vm/overcommit_memory", "1");
    write_sysfs("/proc/sys/vm/min_free_kbytes", "16384");
}

static void apply_mem_balanced(void)
{
    logki("op9pro-perf: MEM -> balanced\n");
    write_sysfs("/proc/sys/vm/swappiness", "100");
    write_sysfs("/proc/sys/vm/dirty_ratio", "20");
    write_sysfs("/proc/sys/vm/dirty_background_ratio", "5");
    write_sysfs("/proc/sys/vm/dirty_expire_centisecs", "2000");
    write_sysfs("/proc/sys/vm/dirty_writeback_centisecs", "500");
    write_sysfs("/proc/sys/vm/vfs_cache_pressure", "100");
    write_sysfs("/proc/sys/vm/page-cluster", "0");
    write_sysfs("/proc/sys/vm/overcommit_memory", "0");
    write_sysfs("/proc/sys/vm/min_free_kbytes", "11584");
}

static void apply_mem_battery(void)
{
    logki("op9pro-perf: MEM -> battery\n");
    write_sysfs("/proc/sys/vm/swappiness", "150");
    write_sysfs("/proc/sys/vm/dirty_ratio", "10");
    write_sysfs("/proc/sys/vm/dirty_background_ratio", "3");
    write_sysfs("/proc/sys/vm/dirty_expire_centisecs", "1000");
    write_sysfs("/proc/sys/vm/dirty_writeback_centisecs", "300");
    write_sysfs("/proc/sys/vm/vfs_cache_pressure", "150");
    write_sysfs("/proc/sys/vm/page-cluster", "3");
    write_sysfs("/proc/sys/vm/overcommit_memory", "0");
    write_sysfs("/proc/sys/vm/min_free_kbytes", "8192");
}

/* ============================================================
 *  I/O Tuning
 * ============================================================ */
static void apply_io_performance(void)
{
    logki("op9pro-perf: I/O -> performance\n");
    write_sysfs("/sys/block/sda/queue/scheduler", "mq-deadline");
    write_sysfs("/sys/block/sda/queue/read_ahead_kb", "512");
    write_sysfs("/sys/block/sda/queue/nr_requests", "256");
    write_sysfs("/sys/block/sda/queue/iostats", "0");
    write_sysfs("/sys/block/sda/queue/add_random", "0");
    write_sysfs("/sys/block/sda/queue/rq_affinity", "2");
    write_sysfs("/sys/block/sdb/queue/scheduler", "mq-deadline");
    write_sysfs("/sys/block/sdb/queue/read_ahead_kb", "512");
    write_sysfs("/sys/block/sdb/queue/nr_requests", "256");
    write_sysfs("/sys/block/sdb/queue/iostats", "0");
}

static void apply_io_balanced(void)
{
    logki("op9pro-perf: I/O -> balanced\n");
    write_sysfs("/sys/block/sda/queue/scheduler", "bfq");
    write_sysfs("/sys/block/sda/queue/read_ahead_kb", "256");
    write_sysfs("/sys/block/sda/queue/nr_requests", "128");
    write_sysfs("/sys/block/sda/queue/iostats", "0");
    write_sysfs("/sys/block/sda/queue/add_random", "0");
    write_sysfs("/sys/block/sda/queue/rq_affinity", "1");
    write_sysfs("/sys/block/sdb/queue/scheduler", "bfq");
    write_sysfs("/sys/block/sdb/queue/read_ahead_kb", "256");
    write_sysfs("/sys/block/sdb/queue/nr_requests", "128");
    write_sysfs("/sys/block/sdb/queue/iostats", "0");
}

static void apply_io_battery(void)
{
    logki("op9pro-perf: I/O -> battery\n");
    write_sysfs("/sys/block/sda/queue/scheduler", "bfq");
    write_sysfs("/sys/block/sda/queue/read_ahead_kb", "128");
    write_sysfs("/sys/block/sda/queue/nr_requests", "64");
    write_sysfs("/sys/block/sda/queue/iostats", "0");
    write_sysfs("/sys/block/sda/queue/add_random", "0");
    write_sysfs("/sys/block/sda/queue/rq_affinity", "1");
    write_sysfs("/sys/block/sdb/queue/scheduler", "bfq");
    write_sysfs("/sys/block/sdb/queue/read_ahead_kb", "128");
    write_sysfs("/sys/block/sdb/queue/nr_requests", "64");
    write_sysfs("/sys/block/sdb/queue/iostats", "0");
}

/* ============================================================
 *  Network Tuning
 * ============================================================ */
static void apply_net_performance(void)
{
    logki("op9pro-perf: NET -> performance\n");
    write_sysfs("/proc/sys/net/core/rmem_max", "8388608");
    write_sysfs("/proc/sys/net/core/wmem_max", "8388608");
    write_sysfs("/proc/sys/net/ipv4/tcp_fastopen", "3");
    write_sysfs("/proc/sys/net/ipv4/tcp_keepalive_time", "300");
    write_sysfs("/proc/sys/net/ipv4/tcp_keepalive_intvl", "30");
    write_sysfs("/proc/sys/net/ipv4/tcp_keepalive_probes", "5");
    write_sysfs("/proc/sys/net/ipv4/tcp_syn_retries", "2");
    write_sysfs("/proc/sys/net/ipv4/tcp_synack_retries", "2");
    write_sysfs("/proc/sys/net/ipv4/tcp_timestamps", "1");
    write_sysfs("/proc/sys/net/ipv4/tcp_sack", "1");
    write_sysfs("/proc/sys/net/ipv4/tcp_window_scaling", "1");
    write_sysfs("/proc/sys/net/core/netdev_budget", "600");
    write_sysfs("/proc/sys/net/core/netdev_max_backlog", "5000");
}

static void apply_net_balanced(void)
{
    logki("op9pro-perf: NET -> balanced\n");
    write_sysfs("/proc/sys/net/core/rmem_max", "4194304");
    write_sysfs("/proc/sys/net/core/wmem_max", "4194304");
    write_sysfs("/proc/sys/net/ipv4/tcp_fastopen", "3");
    write_sysfs("/proc/sys/net/ipv4/tcp_keepalive_time", "600");
    write_sysfs("/proc/sys/net/ipv4/tcp_keepalive_intvl", "60");
    write_sysfs("/proc/sys/net/ipv4/tcp_keepalive_probes", "5");
    write_sysfs("/proc/sys/net/core/netdev_budget", "300");
    write_sysfs("/proc/sys/net/core/netdev_max_backlog", "2500");
}

static void apply_net_battery(void)
{
    logki("op9pro-perf: NET -> battery\n");
    write_sysfs("/proc/sys/net/core/rmem_max", "2097152");
    write_sysfs("/proc/sys/net/core/wmem_max", "2097152");
    write_sysfs("/proc/sys/net/ipv4/tcp_fastopen", "3");
    write_sysfs("/proc/sys/net/ipv4/tcp_keepalive_time", "1200");
    write_sysfs("/proc/sys/net/ipv4/tcp_keepalive_intvl", "120");
    write_sysfs("/proc/sys/net/ipv4/tcp_keepalive_probes", "3");
    write_sysfs("/proc/sys/net/core/netdev_budget", "150");
    write_sysfs("/proc/sys/net/core/netdev_max_backlog", "1000");
}

/* ============================================================
 *  Power Management Tuning
 * ============================================================ */
static void apply_power_performance(void)
{
    logki("op9pro-perf: POWER -> performance\n");
    write_sysfs("/sys/module/workqueue/parameters/power_efficient", "N");
    write_sysfs("/sys/class/kgsl/kgsl-3d0/default_pwrlevel", "0");
    write_sysfs("/sys/class/kgsl/kgsl-3d0/force_clk_on", "1");
    write_sysfs("/sys/class/kgsl/kgsl-3d0/idle_timer", "10000");
    write_sysfs("/sys/class/kgsl/kgsl-3d0/throttling", "0");
}

static void apply_power_balanced(void)
{
    logki("op9pro-perf: POWER -> balanced\n");
    write_sysfs("/sys/module/workqueue/parameters/power_efficient", "N");
    write_sysfs("/sys/class/kgsl/kgsl-3d0/default_pwrlevel", "4");
    write_sysfs("/sys/class/kgsl/kgsl-3d0/force_clk_on", "0");
    write_sysfs("/sys/class/kgsl/kgsl-3d0/idle_timer", "64");
    write_sysfs("/sys/class/kgsl/kgsl-3d0/throttling", "1");
}

static void apply_power_battery(void)
{
    logki("op9pro-perf: POWER -> battery\n");
    write_sysfs("/sys/module/workqueue/parameters/power_efficient", "Y");
    write_sysfs("/sys/class/kgsl/kgsl-3d0/default_pwrlevel", "6");
    write_sysfs("/sys/class/kgsl/kgsl-3d0/force_clk_on", "0");
    write_sysfs("/sys/class/kgsl/kgsl-3d0/idle_timer", "32");
    write_sysfs("/sys/class/kgsl/kgsl-3d0/throttling", "1");
}

/* ============================================================
 *  Profile Application
 * ============================================================ */
static void apply_profile(int profile)
{
    switch (profile) {
    case PROFILE_PERFORMANCE:
        apply_cpu_performance();
        apply_mem_performance();
        apply_io_performance();
        apply_net_performance();
        apply_power_performance();
        break;
    case PROFILE_BALANCED:
        apply_cpu_balanced();
        apply_mem_balanced();
        apply_io_balanced();
        apply_net_balanced();
        apply_power_balanced();
        break;
    case PROFILE_BATTERY:
        apply_cpu_battery();
        apply_mem_battery();
        apply_io_battery();
        apply_net_battery();
        apply_power_battery();
        break;
    default:
        logke("op9pro-perf: unknown profile %d\n", profile);
        return;
    }
    current_profile = profile;
}

/* ============================================================
 *  KPM Callbacks
 * ============================================================ */

static long op9pro_perf_init(const char *args, const char *event, void *__user reserved)
{
    logki("========================================\n");
    logki("op9pro-perf: KPM v1.0.0 initializing\n");
    logki("op9pro-perf: device: OnePlus 9 Pro (SM8350)\n");
    logki("op9pro-perf: kernelpatch ver: %x\n", kpver);
    logki("========================================\n");

    /* Resolve kernel functions */
    if (resolve_kernel_funcs() != 0) {
        logke("op9pro-perf: FATAL: cannot resolve kernel funcs\n");
        return -1;
    }

    /* Parse initial profile from args */
    int profile = PROFILE_BALANCED;

    if (args && args[0] != '\0') {
        if (args[0] == 'p') {
            profile = PROFILE_PERFORMANCE;
        } else if (args[0] == 'b' && args[1] == 'a') {
            profile = PROFILE_BATTERY;
        }
        /* default: balanced */
    }

    logki("op9pro-perf: profile = %d\n", profile);
    apply_profile(profile);
    logki("op9pro-perf: init complete\n");
    return 0;
}

static long op9pro_perf_control0(const char *args, char *__user out_msg, int outlen)
{
    logki("op9pro-perf: ctl0 args: %s\n", args ? args : "(null)");

    if (!args || args[0] == '\0') {
        return 0;
    }

    if (args[0] == 'p') {
        apply_profile(PROFILE_PERFORMANCE);
    } else if (args[0] == 'b' && args[1] == 'a') {
        apply_profile(PROFILE_BATTERY);
    } else if (args[0] == 'b') {
        apply_profile(PROFILE_BALANCED);
    } else if (args[0] == 's') {
        logki("op9pro-perf: current profile = %d\n", current_profile);
    }

    return 0;
}

static long op9pro_perf_control1(void *a1, void *a2, void *a3)
{
    return 0;
}

static long op9pro_perf_exit(void *__user reserved)
{
    logki("op9pro-perf: restoring balanced...\n");
    apply_profile(PROFILE_BALANCED);
    logki("op9pro-perf: unloaded\n");
    return 0;
}

KPM_INIT(op9pro_perf_init);
KPM_CTL0(op9pro_perf_control0);
KPM_CTL1(op9pro_perf_control1);
KPM_EXIT(op9pro_perf_exit);
