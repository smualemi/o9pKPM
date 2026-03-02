# OP9Pro Performance KPM

KernelPatch Module for OnePlus 9 Pro (SM8350/Lahaina) performance tuning.

## Architecture

**KPM + Shell Script** - The KPM loads into the kernel as a module marker. 
The shell script applies actual sysfs/procfs tuning.

> **Why two parts?** KernelPatch's module loader only resolves its own exported symbols.
> `kallsyms_lookup_name` (needed for file I/O in kernel space) is NOT exported,
> so kernel-level tuning inside the KPM is not possible without a fully-linked build.

## Installation

1. Download both files from [Releases](../../actions) (Actions → latest build → Artifacts)
2. Load `op9pro_perf.kpm` in APatch → KPM Manager
3. Copy `op9pro_perf.sh` to `/data/adb/kpm/` on your device
4. Fix line endings and permissions:
```sh
sed -i 's/\r$//' /data/adb/kpm/op9pro_perf.sh
chmod +x /data/adb/kpm/op9pro_perf.sh
```

## Usage

Switch profiles:
```sh
sh /data/adb/kpm/op9pro_perf.sh performance
sh /data/adb/kpm/op9pro_perf.sh balanced
sh /data/adb/kpm/op9pro_perf.sh battery
```

## Auto-apply on boot

Create `/data/adb/post-fs-data.d/op9pro_perf.sh`:
```sh
#!/system/bin/sh
sh /data/adb/kpm/op9pro_perf.sh balanced
```
```sh
chmod +x /data/adb/post-fs-data.d/op9pro_perf.sh
```

## Profiles

| Setting | Performance | Balanced | Battery |
|---------|------------|----------|--------|
| CPU Governor | schedutil (aggressive) | schedutil (default) | schedutil (conservative) |
| GPU | Max clock, no throttle | Auto | Low power |
| I/O Scheduler | mq-deadline | bfq | bfq |
| Memory | Low swap, large cache | Default | High swap |
| Network | Large buffers | Medium | Small |

## Technical Details

- The `W()` function in the shell script uses `2>/dev/null` to silently skip
  files that don't exist or require special permissions
- All errors are non-fatal — the script applies what it can

## ⚠️ Warning

This module modifies kernel parameters. Use at your own risk.
