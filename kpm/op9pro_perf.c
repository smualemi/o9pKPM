/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright (C) 2024 smualemi. All Rights Reserved.
 *
 * OP9Pro Performance KPM - KernelPatch Module for OnePlus 9 Pro (SM8350)
 * Compatible with OxygenOS 14 QGKI Kernel
 *
 * Features:
 *   - CPU Frequency tuning (schedutil/UAG parameters per cluster)
 *   - Memory management optimization (vm tunables, ZRAM, LMK)
 *   - I/O scheduler tuning (BFQ/deadline, read_ahead)
 *   - Network optimization (TCP buffers, congestion)
 *   - Power management (workqueue efficiency, CPU idle)
 *   - 3 profiles: performance / balanced / battery
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
KPM_DESCRIPTION("OnePlus 9 Pro Performance Optimizer for APatch");

/* ============================================================
 *  Profile Definitions
 * ============================================================
 *
 *  SM8350 (Lahaina) Cluster Layout:
 *    Cluster 0 (Silver): CPU 0-3  - Cortex-A55 @ up to 1.8GHz
 *    Cluster 1 (Gold):   CPU 4-6  - Cortex-A78 @ up to 2.4GHz
 *    Cluster 2 (Prime):  CPU 7    - Cortex-X1  @ up to 2.84GHz
 * ============================================================ */

/* Profile IDs */
#define PROFILE_PERFORMANCE  0
#define PROFILE_BALANCED     1
#define PROFILE_BATTERY      2

static int current_profile = PROFILE_BALANCED;

/* ============================================================
 *  Kernel sysctl/sysfs write helpers
 * ============================================================ */

/**
 * write_sysfs - Write a string value to a sysfs/procfs path
 * @path: Full path to the sysfs/procfs file
 * @value: String value to write
 *
 * Uses kernel file operations to write values.
 * Returns 0 on success, negative on error.
 */
static int write_sysfs(const char *path, const char *value)
{
    /* Use KernelPatch's kernel file write utility */
    void *filp;
    int ret;
    long len;

    len = strlen(value);

    /* kernel_write_file equivalent via KP API */
    filp = kfunc(filp_open)(path, 0x0001 | 0x0200, 0); /* O_WRONLY | O_TRUNC */
    if (!filp || (long)filp < 0) {
        pr_err("op9pro-perf: failed to open %s\n", path);
        return -1;
    }

    ret = kfunc(kernel_write)(filp, value, len, 0);
    kfunc(filp_close)(filp, 0);

    if (ret < 0) {
        pr_err("op9pro-perf: failed to write %s to %s\n", value, path);
        return ret;
    }

    return 0;
}

/* ============================================================
 *  CPU Frequency Tuning
 * ============================================================
 *
 *  The kernel uses:
 *    - CONFIG_CPU_FREQ_DEFAULT_GOV_PERFORMANCE
 *    - CONFIG_OPLUS_CPU_FREQ_GOV_UAG (OPLUS Unified Adaptive Governor)
 *    - CONFIG_OPLUS_FEATURE_SUGOV_TL (Schedutil Target Load)
 *    - CONFIG_OPLUS_MULTI_LV_TL (Multi-level Target Load)
 *
 *  We tune schedutil/UAG parameters per cluster for each profile.
 * ============================================================ */

static void apply_cpu_performance(void)
{
    pr_info("op9pro-perf: applying CPU performance profile\n");

    /* Silver cluster (CPU 0-3): Cortex-A55 */
    write_sysfs("/sys/devices/system/cpu/cpufreq/policy0/scaling_governor", "schedutil");
    write_sysfs("/sys/devices/system/cpu/cpufreq/policy0/schedutil/rate_limit_us", "500");
    write_sysfs("/sys/devices/system/cpu/cpufreq/policy0/schedutil/hispeed_freq", "1612800");
    write_sysfs("/sys/devices/system/cpu/cpufreq/policy0/scaling_min_freq", "691200");

    /* Gold cluster (CPU 4-6): Cortex-A78 */
    write_sysfs("/sys/devices/system/cpu/cpufreq/policy4/scaling_governor", "schedutil");
    write_sysfs("/sys/devices/system/cpu/cpufreq/policy4/schedutil/rate_limit_us", "500");
    write_sysfs("/sys/devices/system/cpu/cpufreq/policy4/schedutil/hispeed_freq", "2131200");
    write_sysfs("/sys/devices/system/cpu/cpufreq/policy4/scaling_min_freq", "710400");

    /* Prime cluster (CPU 7): Cortex-X1 */
    write_sysfs("/sys/devices/system/cpu/cpufreq/policy7/scaling_governor", "schedutil");
    write_sysfs("/sys/devices/system/cpu/cpufreq/policy7/schedutil/rate_limit_us", "500");
    write_sysfs("/sys/devices/system/cpu/cpufreq/policy7/schedutil/hispeed_freq", "2649600");
    write_sysfs("/sys/devices/system/cpu/cpufreq/policy7/scaling_min_freq", "844800");

    /* CPU input boost equivalent - set aggressive ramp */
    write_sysfs("/proc/sys/kernel/sched_upmigrate", "80");
    write_sysfs("/proc/sys/kernel/sched_downmigrate", "60");
    write_sysfs("/proc/sys/kernel/sched_boost", "1");
}

static void apply_cpu_balanced(void)
{
    pr_info("op9pro-perf: applying CPU balanced profile\n");

    /* Silver cluster */
    write_sysfs("/sys/devices/system/cpu/cpufreq/policy0/scaling_governor", "schedutil");
    write_sysfs("/sys/devices/system/cpu/cpufreq/policy0/schedutil/rate_limit_us", "1000");
    write_sysfs("/sys/devices/system/cpu/cpufreq/policy0/schedutil/hispeed_freq", "1324800");
    write_sysfs("/sys/devices/system/cpu/cpufreq/policy0/scaling_min_freq", "300000");

    /* Gold cluster */
    write_sysfs("/sys/devices/system/cpu/cpufreq/policy4/scaling_governor", "schedutil");
    write_sysfs("/sys/devices/system/cpu/cpufreq/policy4/schedutil/rate_limit_us", "1000");
    write_sysfs("/sys/devices/system/cpu/cpufreq/policy4/schedutil/hispeed_freq", "1766400");
    write_sysfs("/sys/devices/system/cpu/cpufreq/policy4/scaling_min_freq", "710400");

    /* Prime cluster */
    write_sysfs("/sys/devices/system/cpu/cpufreq/policy7/scaling_governor", "schedutil");
    write_sysfs("/sys/devices/system/cpu/cpufreq/policy7/schedutil/rate_limit_us", "1000");
    write_sysfs("/sys/devices/system/cpu/cpufreq/policy7/schedutil/hispeed_freq", "2188800");
    write_sysfs("/sys/devices/system/cpu/cpufreq/policy7/scaling_min_freq", "844800");

    /* Scheduler - balanced migration */
    write_sysfs("/proc/sys/kernel/sched_upmigrate", "90");
    write_sysfs("/proc/sys/kernel/sched_downmigrate", "75");
    write_sysfs("/proc/sys/kernel/sched_boost", "0");
}

static void apply_cpu_battery(void)
{
    pr_info("op9pro-perf: applying CPU battery profile\n");

    /* Silver cluster - prefer low freqs */
    write_sysfs("/sys/devices/system/cpu/cpufreq/policy0/scaling_governor", "schedutil");
    write_sysfs("/sys/devices/system/cpu/cpufreq/policy0/schedutil/rate_limit_us", "2000");
    write_sysfs("/sys/devices/system/cpu/cpufreq/policy0/schedutil/hispeed_freq", "1017600");
    write_sysfs("/sys/devices/system/cpu/cpufreq/policy0/scaling_min_freq", "300000");

    /* Gold cluster */
    write_sysfs("/sys/devices/system/cpu/cpufreq/policy4/scaling_governor", "schedutil");
    write_sysfs("/sys/devices/system/cpu/cpufreq/policy4/schedutil/rate_limit_us", "2000");
    write_sysfs("/sys/devices/system/cpu/cpufreq/policy4/schedutil/hispeed_freq", "1267200");
    write_sysfs("/sys/devices/system/cpu/cpufreq/policy4/scaling_min_freq", "710400");

    /* Prime cluster - conservative */
    write_sysfs("/sys/devices/system/cpu/cpufreq/policy7/scaling_governor", "schedutil");
    write_sysfs("/sys/devices/system/cpu/cpufreq/policy7/schedutil/rate_limit_us", "4000");
    write_sysfs("/sys/devices/system/cpu/cpufreq/policy7/schedutil/hispeed_freq", "1574400");
    write_sysfs("/sys/devices/system/cpu/cpufreq/policy7/scaling_min_freq", "844800");

    /* Scheduler - keep tasks on little cores */
    write_sysfs("/proc/sys/kernel/sched_upmigrate", "95");
    write_sysfs("/proc/sys/kernel/sched_downmigrate", "85");
    write_sysfs("/proc/sys/kernel/sched_boost", "0");
}

/* ============================================================
 *  Memory Management Tuning
 * ============================================================
 *
 *  Relevant defconfig features:
 *    - CONFIG_TRANSPARENT_HUGEPAGE_ALWAYS=y
 *    - CONFIG_ZRAM=y with CONT_PTE_HUGEPAGE_64K_ZRAM
 *    - CONFIG_SPECULATIVE_PAGE_FAULT=y
 *    - CONFIG_OPLUS_FEATURE_ZRAM_OPT=y
 *    - CONFIG_DYNAMIC_TUNNING_SWAPPINESS=y
 *    - CONFIG_MULTIPLE_KSWAPD=y
 *    - CONFIG_KVMALLOC_OPTIMIZATION=y
 * ============================================================ */

static void apply_mem_performance(void)
{
    pr_info("op9pro-perf: applying memory performance profile\n");

    /* Reduce swappiness - keep more in RAM */
    write_sysfs("/proc/sys/vm/swappiness", "60");

    /* Dirty page writeback - delay writes for throughput */
    write_sysfs("/proc/sys/vm/dirty_ratio", "30");
    write_sysfs("/proc/sys/vm/dirty_background_ratio", "10");
    write_sysfs("/proc/sys/vm/dirty_expire_centisecs", "3000");
    write_sysfs("/proc/sys/vm/dirty_writeback_centisecs", "500");

    /* VFS cache - keep dentries/inodes longer */
    write_sysfs("/proc/sys/vm/vfs_cache_pressure", "50");

    /* Page cluster for swap readahead */
    write_sysfs("/proc/sys/vm/page-cluster", "0");

    /* OOM score adjustment for better app retention */
    write_sysfs("/proc/sys/vm/overcommit_memory", "1");

    /* Readahead for better sequential read */
    write_sysfs("/proc/sys/vm/min_free_kbytes", "16384");

    /* ZRAM optimization */
    write_sysfs("/sys/block/zram0/max_comp_streams", "8");
}

static void apply_mem_balanced(void)
{
    pr_info("op9pro-perf: applying memory balanced profile\n");

    write_sysfs("/proc/sys/vm/swappiness", "100");
    write_sysfs("/proc/sys/vm/dirty_ratio", "20");
    write_sysfs("/proc/sys/vm/dirty_background_ratio", "5");
    write_sysfs("/proc/sys/vm/dirty_expire_centisecs", "2000");
    write_sysfs("/proc/sys/vm/dirty_writeback_centisecs", "500");
    write_sysfs("/proc/sys/vm/vfs_cache_pressure", "100");
    write_sysfs("/proc/sys/vm/page-cluster", "0");
    write_sysfs("/proc/sys/vm/overcommit_memory", "0");
    write_sysfs("/proc/sys/vm/min_free_kbytes", "11584");
    write_sysfs("/sys/block/zram0/max_comp_streams", "8");
}

static void apply_mem_battery(void)
{
    pr_info("op9pro-perf: applying memory battery profile\n");

    /* Higher swappiness to free RAM aggressively */
    write_sysfs("/proc/sys/vm/swappiness", "150");
    write_sysfs("/proc/sys/vm/dirty_ratio", "10");
    write_sysfs("/proc/sys/vm/dirty_background_ratio", "3");
    write_sysfs("/proc/sys/vm/dirty_expire_centisecs", "1000");
    write_sysfs("/proc/sys/vm/dirty_writeback_centisecs", "300");
    write_sysfs("/proc/sys/vm/vfs_cache_pressure", "150");
    write_sysfs("/proc/sys/vm/page-cluster", "3");
    write_sysfs("/proc/sys/vm/overcommit_memory", "0");
    write_sysfs("/proc/sys/vm/min_free_kbytes", "8192");
    write_sysfs("/sys/block/zram0/max_comp_streams", "4");
}

/* ============================================================
 *  I/O Scheduler Tuning
 * ============================================================
 *
 *  Available schedulers:
 *    - CONFIG_MQ_IOSCHED_DEADLINE=y
 *    - CONFIG_MQ_IOSCHED_KYBER=y
 *    - CONFIG_IOSCHED_BFQ=y
 *    - CONFIG_BFQ_GROUP_IOSCHED=y
 * ============================================================ */

static void apply_io_performance(void)
{
    pr_info("op9pro-perf: applying I/O performance profile\n");

    /* Use deadline for low latency */
    write_sysfs("/sys/block/sda/queue/scheduler", "mq-deadline");
    write_sysfs("/sys/block/sda/queue/read_ahead_kb", "512");
    write_sysfs("/sys/block/sda/queue/nr_requests", "256");
    write_sysfs("/sys/block/sda/queue/iostats", "0");
    write_sysfs("/sys/block/sda/queue/add_random", "0");
    write_sysfs("/sys/block/sda/queue/rq_affinity", "2");
    write_sysfs("/sys/block/sda/queue/nomerges", "0");

    /* Same for sdb, sdc if present */
    write_sysfs("/sys/block/sdb/queue/scheduler", "mq-deadline");
    write_sysfs("/sys/block/sdb/queue/read_ahead_kb", "512");
    write_sysfs("/sys/block/sdb/queue/nr_requests", "256");
    write_sysfs("/sys/block/sdb/queue/iostats", "0");

    /* dm-0 (data partition) */
    write_sysfs("/sys/block/dm-0/queue/read_ahead_kb", "512");
}

static void apply_io_balanced(void)
{
    pr_info("op9pro-perf: applying I/O balanced profile\n");

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

    write_sysfs("/sys/block/dm-0/queue/read_ahead_kb", "256");
}

static void apply_io_battery(void)
{
    pr_info("op9pro-perf: applying I/O battery profile\n");

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

    write_sysfs("/sys/block/dm-0/queue/read_ahead_kb", "128");
}

/* ============================================================
 *  Network Tuning
 * ============================================================
 *
 *  Current config:
 *    - CONFIG_TCP_CONG_CUBIC (default)
 *    - CONFIG_INET=y, CONFIG_IPV6=y
 * ============================================================ */

static void apply_net_performance(void)
{
    pr_info("op9pro-perf: applying network performance profile\n");

    /* TCP buffer sizes: min, default, max */
    write_sysfs("/proc/sys/net/core/rmem_max", "8388608");
    write_sysfs("/proc/sys/net/core/wmem_max", "8388608");
    write_sysfs("/proc/sys/net/ipv4/tcp_rmem", "4096 524288 8388608");
    write_sysfs("/proc/sys/net/ipv4/tcp_wmem", "4096 524288 8388608");

    /* Enable TCP fast open */
    write_sysfs("/proc/sys/net/ipv4/tcp_fastopen", "3");

    /* Reduce TCP keepalive for faster detection */
    write_sysfs("/proc/sys/net/ipv4/tcp_keepalive_time", "300");
    write_sysfs("/proc/sys/net/ipv4/tcp_keepalive_intvl", "30");
    write_sysfs("/proc/sys/net/ipv4/tcp_keepalive_probes", "5");

    /* Optimize TCP retransmission */
    write_sysfs("/proc/sys/net/ipv4/tcp_syn_retries", "2");
    write_sysfs("/proc/sys/net/ipv4/tcp_synack_retries", "2");

    /* Timestamps and SACK */
    write_sysfs("/proc/sys/net/ipv4/tcp_timestamps", "1");
    write_sysfs("/proc/sys/net/ipv4/tcp_sack", "1");
    write_sysfs("/proc/sys/net/ipv4/tcp_window_scaling", "1");

    /* Netdev budget for higher throughput */
    write_sysfs("/proc/sys/net/core/netdev_budget", "600");
    write_sysfs("/proc/sys/net/core/netdev_max_backlog", "5000");
}

static void apply_net_balanced(void)
{
    pr_info("op9pro-perf: applying network balanced profile\n");

    write_sysfs("/proc/sys/net/core/rmem_max", "4194304");
    write_sysfs("/proc/sys/net/core/wmem_max", "4194304");
    write_sysfs("/proc/sys/net/ipv4/tcp_rmem", "4096 131072 4194304");
    write_sysfs("/proc/sys/net/ipv4/tcp_wmem", "4096 131072 4194304");
    write_sysfs("/proc/sys/net/ipv4/tcp_fastopen", "3");
    write_sysfs("/proc/sys/net/ipv4/tcp_keepalive_time", "600");
    write_sysfs("/proc/sys/net/ipv4/tcp_keepalive_intvl", "60");
    write_sysfs("/proc/sys/net/ipv4/tcp_keepalive_probes", "5");
    write_sysfs("/proc/sys/net/core/netdev_budget", "300");
    write_sysfs("/proc/sys/net/core/netdev_max_backlog", "2500");
}

static void apply_net_battery(void)
{
    pr_info("op9pro-perf: applying network battery profile\n");

    write_sysfs("/proc/sys/net/core/rmem_max", "2097152");
    write_sysfs("/proc/sys/net/core/wmem_max", "2097152");
    write_sysfs("/proc/sys/net/ipv4/tcp_rmem", "4096 87380 2097152");
    write_sysfs("/proc/sys/net/ipv4/tcp_wmem", "4096 65536 2097152");
    write_sysfs("/proc/sys/net/ipv4/tcp_fastopen", "3");
    write_sysfs("/proc/sys/net/ipv4/tcp_keepalive_time", "1200");
    write_sysfs("/proc/sys/net/ipv4/tcp_keepalive_intvl", "120");
    write_sysfs("/proc/sys/net/ipv4/tcp_keepalive_probes", "3");
    write_sysfs("/proc/sys/net/core/netdev_budget", "150");
    write_sysfs("/proc/sys/net/core/netdev_max_backlog", "1000");
}

/* ============================================================
 *  Power Management Tuning
 * ============================================================
 *
 *  Key defconfig notes:
 *    - CONFIG_WQ_POWER_EFFICIENT_DEFAULT is NOT set
 *    - CONFIG_CPU_IDLE=y (with QCOM LPM)
 *    - CONFIG_ENERGY_MODEL=y
 * ============================================================ */

static void apply_power_performance(void)
{
    pr_info("op9pro-perf: applying power performance profile\n");

    /* Disable power-efficient workqueues for max throughput */
    write_sysfs("/sys/module/workqueue/parameters/power_efficient", "N");

    /* GPU - favor performance */
    write_sysfs("/sys/class/kgsl/kgsl-3d0/default_pwrlevel", "0");
    write_sysfs("/sys/class/kgsl/kgsl-3d0/force_clk_on", "1");
    write_sysfs("/sys/class/kgsl/kgsl-3d0/idle_timer", "10000");
    write_sysfs("/sys/class/kgsl/kgsl-3d0/throttling", "0");
    write_sysfs("/sys/class/devfreq/3d00000.qcom,kgsl-3d0/governor", "performance");

    /* Bus bandwidth - aggressive */
    write_sysfs("/sys/class/devfreq/soc:qcom,cpu-llcc-ddr-bw/governor", "bw_hwmon");
    write_sysfs("/sys/class/devfreq/soc:qcom,cpu-llcc-ddr-bw/min_freq", "762");
}

static void apply_power_balanced(void)
{
    pr_info("op9pro-perf: applying power balanced profile\n");

    write_sysfs("/sys/module/workqueue/parameters/power_efficient", "N");

    /* GPU - balanced */
    write_sysfs("/sys/class/kgsl/kgsl-3d0/default_pwrlevel", "4");
    write_sysfs("/sys/class/kgsl/kgsl-3d0/force_clk_on", "0");
    write_sysfs("/sys/class/kgsl/kgsl-3d0/idle_timer", "64");
    write_sysfs("/sys/class/kgsl/kgsl-3d0/throttling", "1");
    write_sysfs("/sys/class/devfreq/3d00000.qcom,kgsl-3d0/governor", "msm-adreno-tz");

    write_sysfs("/sys/class/devfreq/soc:qcom,cpu-llcc-ddr-bw/governor", "bw_hwmon");
}

static void apply_power_battery(void)
{
    pr_info("op9pro-perf: applying power battery profile\n");

    /* Enable power-efficient workqueues */
    write_sysfs("/sys/module/workqueue/parameters/power_efficient", "Y");

    /* GPU - power save */
    write_sysfs("/sys/class/kgsl/kgsl-3d0/default_pwrlevel", "6");
    write_sysfs("/sys/class/kgsl/kgsl-3d0/force_clk_on", "0");
    write_sysfs("/sys/class/kgsl/kgsl-3d0/idle_timer", "32");
    write_sysfs("/sys/class/kgsl/kgsl-3d0/throttling", "1");
    write_sysfs("/sys/class/devfreq/3d00000.qcom,kgsl-3d0/governor", "powersave");

    write_sysfs("/sys/class/devfreq/soc:qcom,cpu-llcc-ddr-bw/governor", "powersave");
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
        pr_err("op9pro-perf: unknown profile %d\n", profile);
        return;
    }
    current_profile = profile;
}

/* ============================================================
 *  KPM Callbacks
 * ============================================================ */

/**
 * Module initialization
 *
 * @param args: Optional args string from APatch
 *              Supports: "performance", "balanced", "battery"
 *              Default: "balanced"
 * @param event: Init event type
 * @param reserved: Reserved
 */
static long op9pro_perf_init(const char *args, const char *event, void *__user reserved)
{
    pr_info("========================================\n");
    pr_info("op9pro-perf: KPM initializing...\n");
    pr_info("op9pro-perf: version 1.0.0\n");
    pr_info("op9pro-perf: device: OnePlus 9 Pro (SM8350)\n");
    pr_info("op9pro-perf: kernelpatch version: %x\n", kpver);
    pr_info("========================================\n");

    /* Parse initial profile from args */
    int profile = PROFILE_BALANCED;

    if (args && strlen(args) > 0) {
        if (strncmp(args, "performance", 11) == 0) {
            profile = PROFILE_PERFORMANCE;
        } else if (strncmp(args, "balanced", 8) == 0) {
            profile = PROFILE_BALANCED;
        } else if (strncmp(args, "battery", 7) == 0) {
            profile = PROFILE_BATTERY;
        } else {
            pr_warn("op9pro-perf: unknown profile '%s', using balanced\n", args);
        }
    }

    pr_info("op9pro-perf: applying profile: %s\n",
            profile == PROFILE_PERFORMANCE ? "performance" :
            profile == PROFILE_BALANCED ? "balanced" : "battery");

    apply_profile(profile);

    pr_info("op9pro-perf: initialization complete!\n");
    return 0;
}

/**
 * Module control interface (from APatch UI)
 *
 * @param args: Control command string
 *              "performance" - switch to performance profile
 *              "balanced"    - switch to balanced profile
 *              "battery"     - switch to battery profile
 *              "status"      - report current profile
 * @param out_msg: Output message buffer (to APatch)
 * @param outlen: Output buffer length
 */
static long op9pro_perf_control0(const char *args, char *__user out_msg, int outlen)
{
    pr_info("op9pro-perf: control command: %s\n", args);

    if (!args || strlen(args) == 0) {
        char msg[] = "Usage: performance|balanced|battery|status";
        compat_copy_to_user(out_msg, msg, sizeof(msg));
        return 0;
    }

    if (strncmp(args, "performance", 11) == 0) {
        apply_profile(PROFILE_PERFORMANCE);
        char msg[] = "Profile: performance applied";
        compat_copy_to_user(out_msg, msg, sizeof(msg));
    } else if (strncmp(args, "balanced", 8) == 0) {
        apply_profile(PROFILE_BALANCED);
        char msg[] = "Profile: balanced applied";
        compat_copy_to_user(out_msg, msg, sizeof(msg));
    } else if (strncmp(args, "battery", 7) == 0) {
        apply_profile(PROFILE_BATTERY);
        char msg[] = "Profile: battery applied";
        compat_copy_to_user(out_msg, msg, sizeof(msg));
    } else if (strncmp(args, "status", 6) == 0) {
        char msg[64];
        const char *profile_name =
            current_profile == PROFILE_PERFORMANCE ? "performance" :
            current_profile == PROFILE_BALANCED ? "balanced" : "battery";
        /* manual snprintf-like */
        int i = 0;
        const char *prefix = "Current profile: ";
        while (*prefix && i < 63) msg[i++] = *prefix++;
        while (*profile_name && i < 63) msg[i++] = *profile_name++;
        msg[i] = '\0';
        compat_copy_to_user(out_msg, msg, i + 1);
    } else {
        char msg[] = "Unknown command. Use: performance|balanced|battery|status";
        compat_copy_to_user(out_msg, msg, sizeof(msg));
    }

    return 0;
}

static long op9pro_perf_control1(void *a1, void *a2, void *a3)
{
    pr_info("op9pro-perf: control1 called\n");
    return 0;
}

/**
 * Module exit - restore defaults
 */
static long op9pro_perf_exit(void *__user reserved)
{
    pr_info("op9pro-perf: restoring balanced profile before exit...\n");
    apply_profile(PROFILE_BALANCED);
    pr_info("op9pro-perf: KPM unloaded\n");
    return 0;
}

/* Register KPM callbacks */
KPM_INIT(op9pro_perf_init);
KPM_CTL0(op9pro_perf_control0);
KPM_CTL1(op9pro_perf_control1);
KPM_EXIT(op9pro_perf_exit);
