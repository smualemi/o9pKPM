#!/system/bin/sh
# OP9Pro Performance Tuner v4.1
# Usage: sh /data/adb/kpm/op9pro_perf.sh [gaming|performance|balanced|battery]

PROFILE="$1"
if [ -z "$PROFILE" ]; then
    echo "Usage: sh $0 [gaming|performance|balanced|battery]"
    exit 1
fi

W() { (echo "$2" > "$1") 2>/dev/null; }

echo "[op9pro] Applying: $PROFILE"

if [ "$PROFILE" = "gaming" ]; then
    # ============ GAMING MODE ============
    # Maximum performance - use with external cooling!

    # CPU: Force max frequencies on all clusters
    W /sys/devices/system/cpu/cpufreq/policy0/scaling_governor performance
    W /sys/devices/system/cpu/cpufreq/policy0/scaling_min_freq 1804800
    W /sys/devices/system/cpu/cpufreq/policy0/scaling_max_freq 1804800
    W /sys/devices/system/cpu/cpufreq/policy4/scaling_governor performance
    W /sys/devices/system/cpu/cpufreq/policy4/scaling_min_freq 2419200
    W /sys/devices/system/cpu/cpufreq/policy4/scaling_max_freq 2419200
    W /sys/devices/system/cpu/cpufreq/policy7/scaling_governor performance
    W /sys/devices/system/cpu/cpufreq/policy7/scaling_min_freq 2841600
    W /sys/devices/system/cpu/cpufreq/policy7/scaling_max_freq 2841600

    # CPU: Aggressive scheduling
    W /proc/sys/kernel/sched_upmigrate 60
    W /proc/sys/kernel/sched_downmigrate 30
    W /proc/sys/kernel/sched_boost 1
    W /proc/sys/kernel/sched_child_runs_first 1
    W /proc/sys/kernel/perf_cpu_time_max_percent 25

    # All cores online
    W /sys/devices/system/cpu/cpu0/online 1
    W /sys/devices/system/cpu/cpu1/online 1
    W /sys/devices/system/cpu/cpu2/online 1
    W /sys/devices/system/cpu/cpu3/online 1
    W /sys/devices/system/cpu/cpu4/online 1
    W /sys/devices/system/cpu/cpu5/online 1
    W /sys/devices/system/cpu/cpu6/online 1
    W /sys/devices/system/cpu/cpu7/online 1

    # Disable CPU idle deep states for lowest latency
    W /sys/devices/system/cpu/cpu0/cpuidle/state1/disable 1
    W /sys/devices/system/cpu/cpu4/cpuidle/state1/disable 1
    W /sys/devices/system/cpu/cpu7/cpuidle/state1/disable 1

    # Memory: Optimize for speed
    W /proc/sys/vm/swappiness 10
    W /proc/sys/vm/dirty_ratio 50
    W /proc/sys/vm/dirty_background_ratio 20
    W /proc/sys/vm/dirty_expire_centisecs 5000
    W /proc/sys/vm/dirty_writeback_centisecs 1000
    W /proc/sys/vm/vfs_cache_pressure 30
    W /proc/sys/vm/page-cluster 0
    W /proc/sys/vm/min_free_kbytes 32768
    W /proc/sys/vm/extra_free_kbytes 24300
    W /proc/sys/vm/stat_interval 10

    # I/O: Maximum throughput + lowest latency
    W /sys/block/sda/queue/scheduler none
    W /sys/block/sda/queue/read_ahead_kb 2048
    W /sys/block/sda/queue/nr_requests 512
    W /sys/block/sda/queue/iostats 0
    W /sys/block/sda/queue/add_random 0
    W /sys/block/sda/queue/rq_affinity 2
    W /sys/block/sda/queue/nomerges 2
    W /sys/block/sdb/queue/scheduler none
    W /sys/block/sdb/queue/read_ahead_kb 2048
    W /sys/block/sdb/queue/nr_requests 512
    W /sys/block/sdb/queue/iostats 0
    W /sys/block/sdb/queue/add_random 0
    W /sys/block/sdb/queue/nomerges 2

    # Network: Maximum buffers
    W /proc/sys/net/core/rmem_max 16777216
    W /proc/sys/net/core/wmem_max 16777216
    W /proc/sys/net/core/rmem_default 1048576
    W /proc/sys/net/core/wmem_default 1048576
    W /proc/sys/net/ipv4/tcp_fastopen 3
    W /proc/sys/net/ipv4/tcp_keepalive_time 120
    W /proc/sys/net/ipv4/tcp_timestamps 1
    W /proc/sys/net/ipv4/tcp_sack 1
    W /proc/sys/net/ipv4/tcp_window_scaling 1
    W /proc/sys/net/ipv4/tcp_low_latency 1
    W /proc/sys/net/ipv4/tcp_nodelay 1
    W /proc/sys/net/core/netdev_budget 1200
    W /proc/sys/net/core/netdev_max_backlog 10000

    # GPU: Absolute maximum
    W /sys/class/kgsl/kgsl-3d0/devfreq/governor performance
    W /sys/class/kgsl/kgsl-3d0/default_pwrlevel 0
    W /sys/class/kgsl/kgsl-3d0/force_clk_on 1
    W /sys/class/kgsl/kgsl-3d0/force_bus_on 1
    W /sys/class/kgsl/kgsl-3d0/force_rail_on 1
    W /sys/class/kgsl/kgsl-3d0/force_no_nap 1
    W /sys/class/kgsl/kgsl-3d0/idle_timer 10000000
    W /sys/class/kgsl/kgsl-3d0/throttling 0
    W /sys/class/kgsl/kgsl-3d0/bus_split 0
    W /sys/class/kgsl/kgsl-3d0/popp 0

    # Power: Disable all power saving
    W /sys/module/workqueue/parameters/power_efficient N
    W /sys/module/lpm_levels/parameters/sleep_disabled 1

    # Thermal: Reduce throttling (use with external cooling!)
    W /sys/class/thermal/thermal_zone0/mode disabled
    W /sys/class/thermal/thermal_zone1/mode disabled

    # Bus bandwidth (LPDDR5)
    W /sys/class/devfreq/soc:qcom,cpu-llcc-ddr-bw/governor performance
    W /sys/class/devfreq/soc:qcom,cpu-cpu-llcc-bw/governor performance

    echo "[op9pro] WARNING: Thermal throttling disabled!"
    echo "[op9pro] Make sure external cooling is active!"

elif [ "$PROFILE" = "performance" ]; then
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
    # Restore idle states
    W /sys/devices/system/cpu/cpu0/cpuidle/state1/disable 0
    W /sys/devices/system/cpu/cpu4/cpuidle/state1/disable 0
    W /sys/devices/system/cpu/cpu7/cpuidle/state1/disable 0
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
    W /sys/class/kgsl/kgsl-3d0/devfreq/governor msm-adreno-tz
    W /sys/module/workqueue/parameters/power_efficient N
    W /sys/class/kgsl/kgsl-3d0/default_pwrlevel 0
    W /sys/class/kgsl/kgsl-3d0/force_clk_on 1
    W /sys/class/kgsl/kgsl-3d0/idle_timer 10000
    W /sys/class/kgsl/kgsl-3d0/throttling 0
    # Re-enable thermal
    W /sys/class/thermal/thermal_zone0/mode enabled
    W /sys/class/thermal/thermal_zone1/mode enabled
    W /sys/module/lpm_levels/parameters/sleep_disabled 0

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
    W /sys/devices/system/cpu/cpu0/cpuidle/state1/disable 0
    W /sys/devices/system/cpu/cpu4/cpuidle/state1/disable 0
    W /sys/devices/system/cpu/cpu7/cpuidle/state1/disable 0
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
    W /sys/class/kgsl/kgsl-3d0/devfreq/governor msm-adreno-tz
    W /sys/module/workqueue/parameters/power_efficient Y
    W /sys/class/kgsl/kgsl-3d0/default_pwrlevel 6
    W /sys/class/kgsl/kgsl-3d0/force_clk_on 0
    W /sys/class/kgsl/kgsl-3d0/idle_timer 32
    W /sys/class/kgsl/kgsl-3d0/throttling 1
    W /sys/class/thermal/thermal_zone0/mode enabled
    W /sys/class/thermal/thermal_zone1/mode enabled
    W /sys/module/lpm_levels/parameters/sleep_disabled 0
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
    W /sys/devices/system/cpu/cpu0/cpuidle/state1/disable 0
    W /sys/devices/system/cpu/cpu4/cpuidle/state1/disable 0
    W /sys/devices/system/cpu/cpu7/cpuidle/state1/disable 0
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
    W /sys/class/kgsl/kgsl-3d0/devfreq/governor msm-adreno-tz
    W /sys/module/workqueue/parameters/power_efficient N
    W /sys/class/kgsl/kgsl-3d0/default_pwrlevel 4
    W /sys/class/kgsl/kgsl-3d0/force_clk_on 0
    W /sys/class/kgsl/kgsl-3d0/idle_timer 64
    W /sys/class/kgsl/kgsl-3d0/throttling 1
    W /sys/class/thermal/thermal_zone0/mode enabled
    W /sys/class/thermal/thermal_zone1/mode enabled
    W /sys/module/lpm_levels/parameters/sleep_disabled 0
fi

echo "[op9pro] Done: $PROFILE"
