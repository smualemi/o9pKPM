#!/system/bin/sh
# OP9Pro Performance Tuner v4.0
# Usage: sh /data/adb/kpm/op9pro_perf.sh [performance|balanced|battery]

PROFILE="$1"
if [ -z "$PROFILE" ]; then
    echo "Usage: sh $0 [performance|balanced|battery]"
    exit 1
fi

W() { echo "$2" > "$1" 2>/dev/null; }

echo "[op9pro] Applying: $PROFILE"

if [ "$PROFILE" = "performance" ]; then
    # CPU
    W /sys/devices/system/cpu/cpufreq/policy0/scaling_governor schedutil
    W /sys/devices/system/cpu/cpufreq/policy0/schedutil/rate_limit_us 500
    W /sys/devices/system/cpu/cpufreq/policy0/schedutil/hispeed_freq 1612800
    W /sys/devices/system/cpu/cpufreq/policy0/scaling_min_freq 691200
    W /sys/devices/system/cpu/cpufreq/policy4/scaling_governor schedutil
    W /sys/devices/system/cpu/cpufreq/policy4/schedutil/rate_limit_us 500
    W /sys/devices/system/cpu/cpufreq/policy4/schedutil/hispeed_freq 2131200
    W /sys/devices/system/cpu/cpufreq/policy4/scaling_min_freq 710400
    W /sys/devices/system/cpu/cpufreq/policy7/scaling_governor schedutil
    W /sys/devices/system/cpu/cpufreq/policy7/schedutil/rate_limit_us 500
    W /sys/devices/system/cpu/cpufreq/policy7/schedutil/hispeed_freq 2649600
    W /sys/devices/system/cpu/cpufreq/policy7/scaling_min_freq 844800
    W /proc/sys/kernel/sched_upmigrate 80
    W /proc/sys/kernel/sched_downmigrate 60
    W /proc/sys/kernel/sched_boost 1
    # Memory
    W /proc/sys/vm/swappiness 60
    W /proc/sys/vm/dirty_ratio 30
    W /proc/sys/vm/dirty_background_ratio 10
    W /proc/sys/vm/dirty_expire_centisecs 3000
    W /proc/sys/vm/dirty_writeback_centisecs 500
    W /proc/sys/vm/vfs_cache_pressure 50
    W /proc/sys/vm/page-cluster 0
    W /proc/sys/vm/min_free_kbytes 16384
    # I/O
    W /sys/block/sda/queue/scheduler mq-deadline
    W /sys/block/sda/queue/read_ahead_kb 512
    W /sys/block/sda/queue/nr_requests 256
    W /sys/block/sda/queue/iostats 0
    W /sys/block/sda/queue/add_random 0
    W /sys/block/sda/queue/rq_affinity 2
    W /sys/block/sdb/queue/scheduler mq-deadline
    W /sys/block/sdb/queue/read_ahead_kb 512
    W /sys/block/sdb/queue/iostats 0
    # Network
    W /proc/sys/net/core/rmem_max 8388608
    W /proc/sys/net/core/wmem_max 8388608
    W /proc/sys/net/ipv4/tcp_fastopen 3
    W /proc/sys/net/ipv4/tcp_keepalive_time 300
    W /proc/sys/net/ipv4/tcp_timestamps 1
    W /proc/sys/net/ipv4/tcp_sack 1
    W /proc/sys/net/ipv4/tcp_window_scaling 1
    W /proc/sys/net/core/netdev_budget 600
    W /proc/sys/net/core/netdev_max_backlog 5000
    # GPU/Power
    W /sys/module/workqueue/parameters/power_efficient N
    W /sys/class/kgsl/kgsl-3d0/default_pwrlevel 0
    W /sys/class/kgsl/kgsl-3d0/force_clk_on 1
    W /sys/class/kgsl/kgsl-3d0/idle_timer 10000
    W /sys/class/kgsl/kgsl-3d0/throttling 0
elif [ "$PROFILE" = "battery" ]; then
    # CPU
    W /sys/devices/system/cpu/cpufreq/policy0/scaling_governor schedutil
    W /sys/devices/system/cpu/cpufreq/policy0/schedutil/rate_limit_us 2000
    W /sys/devices/system/cpu/cpufreq/policy0/schedutil/hispeed_freq 1017600
    W /sys/devices/system/cpu/cpufreq/policy0/scaling_min_freq 300000
    W /sys/devices/system/cpu/cpufreq/policy4/scaling_governor schedutil
    W /sys/devices/system/cpu/cpufreq/policy4/schedutil/rate_limit_us 2000
    W /sys/devices/system/cpu/cpufreq/policy4/schedutil/hispeed_freq 1267200
    W /sys/devices/system/cpu/cpufreq/policy4/scaling_min_freq 710400
    W /sys/devices/system/cpu/cpufreq/policy7/scaling_governor schedutil
    W /sys/devices/system/cpu/cpufreq/policy7/schedutil/rate_limit_us 4000
    W /sys/devices/system/cpu/cpufreq/policy7/schedutil/hispeed_freq 1574400
    W /sys/devices/system/cpu/cpufreq/policy7/scaling_min_freq 844800
    W /proc/sys/kernel/sched_upmigrate 95
    W /proc/sys/kernel/sched_downmigrate 85
    W /proc/sys/kernel/sched_boost 0
    # Memory
    W /proc/sys/vm/swappiness 150
    W /proc/sys/vm/dirty_ratio 10
    W /proc/sys/vm/dirty_background_ratio 3
    W /proc/sys/vm/dirty_expire_centisecs 1000
    W /proc/sys/vm/dirty_writeback_centisecs 300
    W /proc/sys/vm/vfs_cache_pressure 150
    W /proc/sys/vm/page-cluster 3
    W /proc/sys/vm/min_free_kbytes 8192
    # I/O
    W /sys/block/sda/queue/scheduler bfq
    W /sys/block/sda/queue/read_ahead_kb 128
    W /sys/block/sda/queue/nr_requests 64
    W /sys/block/sda/queue/iostats 0
    W /sys/block/sda/queue/add_random 0
    W /sys/block/sda/queue/rq_affinity 1
    W /sys/block/sdb/queue/scheduler bfq
    W /sys/block/sdb/queue/read_ahead_kb 128
    W /sys/block/sdb/queue/iostats 0
    # Network
    W /proc/sys/net/core/rmem_max 2097152
    W /proc/sys/net/core/wmem_max 2097152
    W /proc/sys/net/ipv4/tcp_fastopen 3
    W /proc/sys/net/ipv4/tcp_keepalive_time 1200
    W /proc/sys/net/core/netdev_budget 150
    W /proc/sys/net/core/netdev_max_backlog 1000
    # GPU/Power
    W /sys/module/workqueue/parameters/power_efficient Y
    W /sys/class/kgsl/kgsl-3d0/default_pwrlevel 6
    W /sys/class/kgsl/kgsl-3d0/force_clk_on 0
    W /sys/class/kgsl/kgsl-3d0/idle_timer 32
    W /sys/class/kgsl/kgsl-3d0/throttling 1
else
    # Balanced (default)
    # CPU
    W /sys/devices/system/cpu/cpufreq/policy0/scaling_governor schedutil
    W /sys/devices/system/cpu/cpufreq/policy0/schedutil/rate_limit_us 1000
    W /sys/devices/system/cpu/cpufreq/policy0/schedutil/hispeed_freq 1324800
    W /sys/devices/system/cpu/cpufreq/policy0/scaling_min_freq 300000
    W /sys/devices/system/cpu/cpufreq/policy4/scaling_governor schedutil
    W /sys/devices/system/cpu/cpufreq/policy4/schedutil/rate_limit_us 1000
    W /sys/devices/system/cpu/cpufreq/policy4/schedutil/hispeed_freq 1766400
    W /sys/devices/system/cpu/cpufreq/policy4/scaling_min_freq 710400
    W /sys/devices/system/cpu/cpufreq/policy7/scaling_governor schedutil
    W /sys/devices/system/cpu/cpufreq/policy7/schedutil/rate_limit_us 1000
    W /sys/devices/system/cpu/cpufreq/policy7/schedutil/hispeed_freq 2188800
    W /sys/devices/system/cpu/cpufreq/policy7/scaling_min_freq 844800
    W /proc/sys/kernel/sched_upmigrate 90
    W /proc/sys/kernel/sched_downmigrate 75
    W /proc/sys/kernel/sched_boost 0
    # Memory
    W /proc/sys/vm/swappiness 100
    W /proc/sys/vm/dirty_ratio 20
    W /proc/sys/vm/dirty_background_ratio 5
    W /proc/sys/vm/dirty_expire_centisecs 2000
    W /proc/sys/vm/dirty_writeback_centisecs 500
    W /proc/sys/vm/vfs_cache_pressure 100
    W /proc/sys/vm/page-cluster 0
    W /proc/sys/vm/min_free_kbytes 11584
    # I/O
    W /sys/block/sda/queue/scheduler bfq
    W /sys/block/sda/queue/read_ahead_kb 256
    W /sys/block/sda/queue/nr_requests 128
    W /sys/block/sda/queue/iostats 0
    W /sys/block/sda/queue/add_random 0
    W /sys/block/sda/queue/rq_affinity 1
    W /sys/block/sdb/queue/scheduler bfq
    W /sys/block/sdb/queue/read_ahead_kb 256
    W /sys/block/sdb/queue/iostats 0
    # Network
    W /proc/sys/net/core/rmem_max 4194304
    W /proc/sys/net/core/wmem_max 4194304
    W /proc/sys/net/ipv4/tcp_fastopen 3
    W /proc/sys/net/ipv4/tcp_keepalive_time 600
    W /proc/sys/net/core/netdev_budget 300
    W /proc/sys/net/core/netdev_max_backlog 2500
    # GPU/Power
    W /sys/module/workqueue/parameters/power_efficient N
    W /sys/class/kgsl/kgsl-3d0/default_pwrlevel 4
    W /sys/class/kgsl/kgsl-3d0/force_clk_on 0
    W /sys/class/kgsl/kgsl-3d0/idle_timer 64
    W /sys/class/kgsl/kgsl-3d0/throttling 1
fi

echo "[op9pro] Done: $PROFILE"
