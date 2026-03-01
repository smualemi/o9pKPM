# OP9Pro Performance KPM

KernelPatch Module for OnePlus 9 Pro (SM8350/Lahaina) - OxygenOS 14

## Architecture

This module uses a **two-part** architecture:
1. **KPM Module** (`op9pro_perf.kpm`) - Loaded by APatch/KernelPatch, registers in the kernel
2. **Shell Script** (`op9pro_perf.sh`) - Applies actual sysfs/procfs tuning with root permissions

## Profiles

| Profile | CPU | Memory | I/O | GPU | Battery |
|---------|-----|--------|-----|-----|----------|
| `performance` | Max freq, fast ramp | Low swappiness, large cache | mq-deadline, 512KB readahead | Max clock, no throttle | Higher drain |
| `balanced` | Default schedutil | Default tunables | BFQ, 256KB readahead | Auto governor | Normal |
| `battery` | Low freq cap, slow ramp | High swappiness, aggressive writeback | BFQ, 128KB readahead | Low clock, throttled | Maximum savings |

## Installation

1. Download `op9pro_perf-kpm` from [Releases/Actions](../../actions)
2. Unzip to get `op9pro_perf.kpm`
3. In APatch → KPM tab → Load Module → select `op9pro_perf.kpm`
4. Copy `op9pro_perf.sh` to `/data/adb/kpm/` on your device
5. Run profile: `sh /data/adb/kpm/op9pro_perf.sh performance`

## Auto-apply on boot

Create `/data/adb/post-fs-data.d/op9pro_perf.sh`:
```sh
#!/system/bin/sh
sh /data/adb/kpm/op9pro_perf.sh performance
```
Make executable: `chmod +x /data/adb/post-fs-data.d/op9pro_perf.sh`

## Quick switch profiles

```sh
# Performance mode
sh /data/adb/kpm/op9pro_perf.sh performance

# Balanced mode  
sh /data/adb/kpm/op9pro_perf.sh balanced

# Battery mode
sh /data/adb/kpm/op9pro_perf.sh battery
```

⚠️ **Warning**: Kernel modifications can cause instability. Use at your own risk.
