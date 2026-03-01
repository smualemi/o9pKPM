#!/system/bin/sh
# OP9Pro Performance Tuning Script
# Run via APatch post-fs-data or manually with: sh /data/adb/kpm/op9pro_perf.sh <profile>
#
# Usage: op9pro_perf.sh performance|balanced|battery

PROFILE="${1:-balanced}"
LOG_TAG="op9pro-perf"

log() { log -t "$LOG_TAG" "$1" 2>/dev/null || echo "[$LOG_TAG] $1"; }

w() {
    if [ -f "$1" ]; then
        echo "$2" > "$1" 2>/dev/null && log "  $1 = $2"
    fi
}

log "Applying profile: $PROFILE"

# === CPU ===
case "$PROFILE" in
performance)
    for p in 0 4 7; do
        w /sys/devices/system/cpu/cpufreq/policy${p}/scaling_governor schedutil
    done
    w /sys/devices/system/cpu/cpufreq/policy0/schedutil/rate_limit_us 500
    w /sys/devices/system/cpu/cpufreq/policy0/schedutil/hispeed_freq 1612800
    w /sys/devices/system/cpu/cpufreq/policy0/scaling_min_freq 691200
    w /sys/devices/system/cpu/cpufreq/policy4/schedutil/rate_limit_us 500
    w /sys/devices/system/cpu/cpufreq/policy4/schedutil/hispeed_freq 2131200
    w /sys/devices/system/cpu/cpufreq/policy4/scaling_min_freq 710400
    w /sys/devices/system/cpu/cpufreq/policy7/schedutil/rate_limit_us 500
    w /sys/devices/system/cpu/cpufreq/policy7/schedutil/hispeed_freq 2649600
    w /sys/devices/system/cpu/cpufreq/policy7/scaling_min_freq 844800
    w /proc/sys/kernel/sched_upmigrate 80
    w /proc/sys/kernel/sched_downmigrate 60
    w /proc/sys/kernel/sched_boost 1
    ;;
battery)
    for p in 0 4 7; do
        w /sys/devices/system/cpu/cpufreq/policy${p}/scaling_governor schedutil
    done
    w /sys/devices/system/cpu/cpufreq/policy0/schedutil/rate_limit_us 2000
    w /sys/devices/system/cpu/cpufreq/policy0/schedutil/hispeed_freq 1017600
    w /sys/devices/system/cpu/cpufreq/policy0/scaling_min_freq 300000
    w /sys/devices/system/cpu/cpufreq/policy4/schedutil/rate_limit_us 2000
    w /sys/devices/system/cpu/cpufreq/policy4/schedutil/hispeed_freq 1267200
    w /sys/devices/system/cpu/cpufreq/policy4/scaling_min_freq 710400
    w /sys/devices/system/cpu/cpufreq/policy7/schedutil/rate_limit_us 4000
    w /sys/devices/system/cpu/cpufreq/policy7/schedutil/hispeed_freq 1574400
    w /sys/devices/system/cpu/cpufreq/policy7/scaling_min_freq 844800
    w /proc/sys/kernel/sched_upmigrate 95
    w /proc/sys/kernel/sched_downmigrate 85
    w /proc/sys/kernel/sched_boost 0
    ;;
*) # balanced
    for p in 0 4 7; do
        w /sys/devices/system/cpu/cpufreq/policy${p}/scaling_governor schedutil
    done
    w /sys/devices/system/cpu/cpufreq/policy0/schedutil/rate_limit_us 1000
    w /sys/devices/system/cpu/cpufreq/policy0/schedutil/hispeed_freq 1324800
    w /sys/devices/system/cpu/cpufreq/policy0/scaling_min_freq 300000
    w /sys/devices/system/cpu/cpufreq/policy4/schedutil/rate_limit_us 1000
    w /sys/devices/system/cpu/cpufreq/policy4/schedutil/hispeed_freq 1766400
    w /sys/devices/system/cpu/cpufreq/policy4/scaling_min_freq 710400
    w /sys/devices/system/cpu/cpufreq/policy7/schedutil/rate_limit_us 1000
    w /sys/devices/system/cpu/cpufreq/policy7/schedutil/hispeed_freq 2188800
    w /sys/devices/system/cpu/cpufreq/policy7/scaling_min_freq 844800
    w /proc/sys/kernel/sched_upmigrate 90
    w /proc/sys/kernel/sched_downmigrate 75
    w /proc/sys/kernel/sched_boost 0
    ;;
esac

# === Memory ===
case "$PROFILE" in
performance)
    w /proc/sys/vm/swappiness 60
    w /proc/sys/vm/dirty_ratio 30
    w /proc/sys/vm/dirty_background_ratio 10
    w /proc/sys/vm/dirty_expire_centisecs 3000
    w /proc/sys/vm/dirty_writeback_centisecs 500
    w /proc/sys/vm/vfs_cache_pressure 50
    w /proc/sys/vm/page-cluster 0
    w /proc/sys/vm/min_free_kbytes 16384
    ;;
battery)
    w /proc/sys/vm/swappiness 150
    w /proc/sys/vm/dirty_ratio 10
    w /proc/sys/vm/dirty_background_ratio 3
    w /proc/sys/vm/dirty_expire_centisecs 1000
    w /proc/sys/vm/dirty_writeback_centisecs 300
    w /proc/sys/vm/vfs_cache_pressure 150
    w /proc/sys/vm/page-cluster 3
    w /proc/sys/vm/min_free_kbytes 8192
    ;;
*)
    w /proc/sys/vm/swappiness 100
    w /proc/sys/vm/dirty_ratio 20
    w /proc/sys/vm/dirty_background_ratio 5
    w /proc/sys/vm/dirty_expire_centisecs 2000
    w /proc/sys/vm/dirty_writeback_centisecs 500
    w /proc/sys/vm/vfs_cache_pressure 100
    w /proc/sys/vm/page-cluster 0
    w /proc/sys/vm/min_free_kbytes 11584
    ;;
esac

# === I/O ===
case "$PROFILE" in
performance)
    for d in sda sdb; do
        w /sys/block/$d/queue/scheduler mq-deadline
        w /sys/block/$d/queue/read_ahead_kb 512
        w /sys/block/$d/queue/iostats 0
    done
    w /sys/block/sda/queue/nr_requests 256
    w /sys/block/sda/queue/add_random 0
    w /sys/block/sda/queue/rq_affinity 2
    ;;
battery)
    for d in sda sdb; do
        w /sys/block/$d/queue/scheduler bfq
        w /sys/block/$d/queue/read_ahead_kb 128
        w /sys/block/$d/queue/iostats 0
    done
    w /sys/block/sda/queue/nr_requests 64
    w /sys/block/sda/queue/add_random 0
    w /sys/block/sda/queue/rq_affinity 1
    ;;
*)
    for d in sda sdb; do
        w /sys/block/$d/queue/scheduler bfq
        w /sys/block/$d/queue/read_ahead_kb 256
        w /sys/block/$d/queue/iostats 0
    done
    w /sys/block/sda/queue/nr_requests 128
    w /sys/block/sda/queue/add_random 0
    w /sys/block/sda/queue/rq_affinity 1
    ;;
esac

# === Network ===
case "$PROFILE" in
performance)
    w /proc/sys/net/core/rmem_max 8388608
    w /proc/sys/net/core/wmem_max 8388608
    w /proc/sys/net/ipv4/tcp_fastopen 3
    w /proc/sys/net/ipv4/tcp_keepalive_time 300
    w /proc/sys/net/ipv4/tcp_timestamps 1
    w /proc/sys/net/ipv4/tcp_sack 1
    w /proc/sys/net/ipv4/tcp_window_scaling 1
    w /proc/sys/net/core/netdev_budget 600
    w /proc/sys/net/core/netdev_max_backlog 5000
    ;;
battery)
    w /proc/sys/net/core/rmem_max 2097152
    w /proc/sys/net/core/wmem_max 2097152
    w /proc/sys/net/ipv4/tcp_fastopen 3
    w /proc/sys/net/ipv4/tcp_keepalive_time 1200
    w /proc/sys/net/core/netdev_budget 150
    w /proc/sys/net/core/netdev_max_backlog 1000
    ;;
*)
    w /proc/sys/net/core/rmem_max 4194304
    w /proc/sys/net/core/wmem_max 4194304
    w /proc/sys/net/ipv4/tcp_fastopen 3
    w /proc/sys/net/ipv4/tcp_keepalive_time 600
    w /proc/sys/net/core/netdev_budget 300
    w /proc/sys/net/core/netdev_max_backlog 2500
    ;;
esac

# === GPU / Power ===
case "$PROFILE" in
performance)
    w /sys/module/workqueue/parameters/power_efficient N
    w /sys/class/kgsl/kgsl-3d0/default_pwrlevel 0
    w /sys/class/kgsl/kgsl-3d0/force_clk_on 1
    w /sys/class/kgsl/kgsl-3d0/idle_timer 10000
    w /sys/class/kgsl/kgsl-3d0/throttling 0
    ;;
battery)
    w /sys/module/workqueue/parameters/power_efficient Y
    w /sys/class/kgsl/kgsl-3d0/default_pwrlevel 6
    w /sys/class/kgsl/kgsl-3d0/force_clk_on 0
    w /sys/class/kgsl/kgsl-3d0/idle_timer 32
    w /sys/class/kgsl/kgsl-3d0/throttling 1
    ;;
*)
    w /sys/module/workqueue/parameters/power_efficient N
    w /sys/class/kgsl/kgsl-3d0/default_pwrlevel 4
    w /sys/class/kgsl/kgsl-3d0/force_clk_on 0
    w /sys/class/kgsl/kgsl-3d0/idle_timer 64
    w /sys/class/kgsl/kgsl-3d0/throttling 1
    ;;
esac

log "Profile $PROFILE applied successfully"
