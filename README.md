# 🚀 OP9Pro Performance KPM

KernelPatch Module (KPM) for **OnePlus 9 Pro** (Snapdragon 888 / SM8350 / Lahaina) — Fine-tune your device with **4 performance profiles**.

---

## 📦 Installation

### Requirements
- OnePlus 9 Pro with **APatch** or **KernelPatch** installed
- Root access (ADB shell as root)

### Steps

1. Go to [**Actions**](../../actions) → latest successful build → download **op9pro-perf-kpm** and **op9pro-perf-script**
2. Load `op9pro_perf.kpm` via APatch → **KPM Manager**
3. Push the shell script to your device:
```sh
adb push op9pro_perf.sh /data/adb/kpm/
adb shell chmod +x /data/adb/kpm/op9pro_perf.sh
```
4. Fix line endings (if downloaded from Windows):
```sh
sed -i 's/\r$//' /data/adb/kpm/op9pro_perf.sh
```

---

## 🎮 Profiles

### Usage
```sh
sh /data/adb/kpm/op9pro_perf.sh gaming        # 🔴 Max power (external cooling required!)
sh /data/adb/kpm/op9pro_perf.sh performance    # 🟠 High performance
sh /data/adb/kpm/op9pro_perf.sh balanced       # 🟢 Daily use (default)
sh /data/adb/kpm/op9pro_perf.sh battery        # 🔵 Battery saving
```

---

### 🔴 Gaming Mode
> ⚠️ **WARNING: Disables thermal throttling! Use ONLY with external cooling fan!**

Locks everything to maximum. Designed for competitive gaming sessions with active cooling.

| Component | Setting | Value |
|-----------|---------|-------|
| **CPU Little (policy0)** | Governor | `performance` |
| | Frequency | Locked at **1804 MHz** |
| **CPU Big (policy4)** | Governor | `performance` |
| | Frequency | Locked at **2419 MHz** |
| **CPU Prime (policy7)** | Governor | `performance` |
| | Frequency | Locked at **2841 MHz** |
| **CPU Idle** | Deep idle (state1) | **Disabled** (lowest latency) |
| **CPU Scheduler** | sched_upmigrate | `60` |
| | sched_downmigrate | `30` |
| | sched_boost | `1` |
| | child_runs_first | `1` |
| **GPU (Adreno 660)** | Governor | `performance` |
| | Power Level | `0` (maximum) |
| | force_clk_on | `1` |
| | force_bus_on | `1` |
| | force_rail_on | `1` |
| | force_no_nap | `1` |
| | idle_timer | `10000000` |
| | throttling | `0` (off) |
| **Memory** | swappiness | `10` |
| | dirty_ratio | `50` |
| | dirty_background_ratio | `20` |
| | vfs_cache_pressure | `30` |
| | min_free_kbytes | `32768` |
| | page-cluster | `0` |
| **I/O** | scheduler | `none` (direct dispatch) |
| | read_ahead_kb | `2048` |
| | nr_requests | `512` |
| | nomerges | `2` |
| **Network** | rmem_max / wmem_max | `16777216` (16MB) |
| | netdev_budget | `1200` |
| | netdev_max_backlog | `10000` |
| | tcp_low_latency | `1` |
| **Thermal** | thermal_zone0/1 | **disabled** ⚠️ |
| **Power** | power_efficient | `N` |
| | sleep_disabled | `1` |
| **Bus (DDR/LLCC)** | Governor | `performance` |

---

### 🟠 Performance Mode
High performance with thermal protection still active. Good for demanding apps and light gaming without external cooling.

| Component | Setting | Value |
|-----------|---------|-------|
| **CPU Little (policy0)** | Governor | `schedutil` |
| | rate_limit_us | `500` |
| | hispeed_freq | `1612800` |
| | scaling_min_freq | `691200` |
| **CPU Big (policy4)** | Governor | `schedutil` |
| | rate_limit_us | `500` |
| | hispeed_freq | `2131200` |
| | scaling_min_freq | `710400` |
| **CPU Prime (policy7)** | Governor | `schedutil` |
| | rate_limit_us | `500` |
| | hispeed_freq | `2649600` |
| | scaling_min_freq | `844800` |
| **CPU Scheduler** | sched_upmigrate | `80` |
| | sched_downmigrate | `60` |
| | sched_boost | `1` |
| **GPU** | default_pwrlevel | `0` |
| | force_clk_on | `1` |
| | idle_timer | `10000` |
| | throttling | `0` |
| **Memory** | swappiness | `60` |
| | dirty_ratio | `30` |
| | vfs_cache_pressure | `50` |
| | min_free_kbytes | `16384` |
| **I/O** | scheduler | `mq-deadline` |
| | read_ahead_kb | `512` |
| | nr_requests | `256` |
| **Network** | rmem_max / wmem_max | `8388608` (8MB) |
| | netdev_budget | `600` |

---

### 🟢 Balanced Mode (Default)
Optimal balance between performance and battery life. Recommended for daily use.

| Component | Setting | Value |
|-----------|---------|-------|
| **CPU Little (policy0)** | Governor | `schedutil` |
| | rate_limit_us | `1000` |
| | hispeed_freq | `1324800` |
| | scaling_min_freq | `300000` |
| **CPU Big (policy4)** | Governor | `schedutil` |
| | rate_limit_us | `1000` |
| | hispeed_freq | `1766400` |
| | scaling_min_freq | `710400` |
| **CPU Prime (policy7)** | Governor | `schedutil` |
| | rate_limit_us | `1000` |
| | hispeed_freq | `2188800` |
| | scaling_min_freq | `844800` |
| **CPU Scheduler** | sched_upmigrate | `90` |
| | sched_downmigrate | `75` |
| | sched_boost | `0` |
| **GPU** | default_pwrlevel | `4` |
| | idle_timer | `64` |
| | throttling | `1` (on) |
| **Memory** | swappiness | `100` |
| | dirty_ratio | `20` |
| | vfs_cache_pressure | `100` |
| | min_free_kbytes | `11584` |
| **I/O** | scheduler | `bfq` |
| | read_ahead_kb | `256` |
| | nr_requests | `128` |
| **Network** | rmem_max / wmem_max | `4194304` (4MB) |

---

### 🔵 Battery Mode
Maximizes battery life by reducing performance. Best for standby, reading, and light tasks.

| Component | Setting | Value |
|-----------|---------|-------|
| **CPU Little (policy0)** | Governor | `schedutil` |
| | rate_limit_us | `2000` |
| | hispeed_freq | `1017600` |
| | scaling_min_freq | `300000` |
| **CPU Big (policy4)** | Governor | `schedutil` |
| | rate_limit_us | `2000` |
| | hispeed_freq | `1267200` |
| **CPU Prime (policy7)** | Governor | `schedutil` |
| | rate_limit_us | `4000` |
| | hispeed_freq | `1574400` |
| **CPU Scheduler** | sched_upmigrate | `95` |
| | sched_downmigrate | `85` |
| | sched_boost | `0` |
| **GPU** | default_pwrlevel | `6` |
| | idle_timer | `32` |
| | throttling | `1` (on) |
| **Memory** | swappiness | `150` |
| | dirty_ratio | `10` |
| | vfs_cache_pressure | `150` |
| | min_free_kbytes | `8192` |
| **I/O** | scheduler | `bfq` |
| | read_ahead_kb | `128` |
| | nr_requests | `64` |
| **Network** | rmem_max / wmem_max | `2097152` (2MB) |

---

## 🔍 Verification Commands

After applying a profile, verify with these commands:

### CPU
```sh
echo "=== CPU Governor ==="
cat /sys/devices/system/cpu/cpufreq/policy0/scaling_governor
cat /sys/devices/system/cpu/cpufreq/policy4/scaling_governor
cat /sys/devices/system/cpu/cpufreq/policy7/scaling_governor

echo "=== CPU Min Frequency ==="
cat /sys/devices/system/cpu/cpufreq/policy0/scaling_min_freq
cat /sys/devices/system/cpu/cpufreq/policy4/scaling_min_freq
cat /sys/devices/system/cpu/cpufreq/policy7/scaling_min_freq

echo "=== CPU Current Frequency ==="
cat /sys/devices/system/cpu/cpufreq/policy0/scaling_cur_freq
cat /sys/devices/system/cpu/cpufreq/policy4/scaling_cur_freq
cat /sys/devices/system/cpu/cpufreq/policy7/scaling_cur_freq

echo "=== Scheduler Tuning ==="
cat /proc/sys/kernel/sched_upmigrate
cat /proc/sys/kernel/sched_downmigrate
cat /proc/sys/kernel/sched_boost
```

### GPU
```sh
echo "=== GPU ==="
cat /sys/class/kgsl/kgsl-3d0/devfreq/governor
cat /sys/class/kgsl/kgsl-3d0/default_pwrlevel
cat /sys/class/kgsl/kgsl-3d0/force_clk_on
cat /sys/class/kgsl/kgsl-3d0/idle_timer
cat /sys/class/kgsl/kgsl-3d0/throttling
cat /sys/class/kgsl/kgsl-3d0/gpuclk
```

### Memory
```sh
echo "=== Memory ==="
cat /proc/sys/vm/swappiness
cat /proc/sys/vm/dirty_ratio
cat /proc/sys/vm/dirty_background_ratio
cat /proc/sys/vm/vfs_cache_pressure
cat /proc/sys/vm/min_free_kbytes
cat /proc/sys/vm/page-cluster
```

### I/O
```sh
echo "=== I/O ==="
cat /sys/block/sda/queue/scheduler
cat /sys/block/sda/queue/read_ahead_kb
cat /sys/block/sda/queue/nr_requests
cat /sys/block/sda/queue/rq_affinity
```

### Network
```sh
echo "=== Network ==="
cat /proc/sys/net/core/rmem_max
cat /proc/sys/net/core/wmem_max
cat /proc/sys/net/ipv4/tcp_fastopen
cat /proc/sys/net/core/netdev_budget
```

### Thermal (Gaming mode check)
```sh
echo "=== Thermal ==="
cat /sys/class/thermal/thermal_zone0/mode
cat /sys/class/thermal/thermal_zone1/mode
cat /sys/class/thermal/thermal_zone0/temp
```

### All-in-one check
```sh
echo "=== Quick Status ==="
echo "CPU0: $(cat /sys/devices/system/cpu/cpufreq/policy0/scaling_governor) @ $(cat /sys/devices/system/cpu/cpufreq/policy0/scaling_cur_freq)"
echo "CPU4: $(cat /sys/devices/system/cpu/cpufreq/policy4/scaling_governor) @ $(cat /sys/devices/system/cpu/cpufreq/policy4/scaling_cur_freq)"
echo "CPU7: $(cat /sys/devices/system/cpu/cpufreq/policy7/scaling_governor) @ $(cat /sys/devices/system/cpu/cpufreq/policy7/scaling_cur_freq)"
echo "GPU:  pwrlevel=$(cat /sys/class/kgsl/kgsl-3d0/default_pwrlevel) throttle=$(cat /sys/class/kgsl/kgsl-3d0/throttling)"
echo "Swap: $(cat /proc/sys/vm/swappiness)  Dirty: $(cat /proc/sys/vm/dirty_ratio)%"
echo "I/O:  $(cat /sys/block/sda/queue/scheduler) ra=$(cat /sys/block/sda/queue/read_ahead_kb)KB"
echo "Net:  rmem=$(cat /proc/sys/net/core/rmem_max) wmem=$(cat /proc/sys/net/core/wmem_max)"
```

---

## ⚡ Auto-apply on Boot

To automatically apply a profile when the device boots:

```sh
# Create boot script
cat > /data/adb/post-fs-data.d/op9pro_perf.sh << 'EOF'
#!/system/bin/sh
sleep 30
sh /data/adb/kpm/op9pro_perf.sh balanced
EOF

chmod +x /data/adb/post-fs-data.d/op9pro_perf.sh
```

Change `balanced` to your preferred default profile.

---

## 🏗️ Architecture

```
┌──────────────────────┐     ┌────────────────────────┐
│   op9pro_perf.kpm    │     │   op9pro_perf.sh       │
│   (Kernel Module)    │     │   (Shell Script)       │
│                      │     │                        │
│  • Loads into kernel │     │  • Writes to sysfs     │
│  • Uses KP exports   │     │  • 4 profiles          │
│  • Tracks profile    │     │  • Runs as root        │
│  • APatch UI         │     │  • Error-tolerant      │
└──────────────────────┘     └────────────────────────┘
```

**Why two parts?** KernelPatch's module loader can only resolve its own exported symbols (`KP_EXPORT_SYMBOL`). `kallsyms_lookup_name` — needed for kernel-space file I/O — is not exported, so sysfs tuning must happen from userspace via a shell script.

---

## ⚠️ Warning

- This module modifies kernel parameters at runtime
- **Gaming mode disables thermal protection** — only use with external cooling
- Always have a way to switch back to `balanced` mode
- Use at your own risk

---

## 📄 License

GPL v2
