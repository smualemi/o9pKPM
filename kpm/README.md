# OP9Pro Performance KPM

KernelPatch Module (KPM) لتحسين أداء OnePlus 9 Pro (SM8350/Lahaina) مع OxygenOS 14.

## المميزات

- **3 أوضاع أداء**: Performance / Balanced / Battery
- **تحسين CPU**: ضبط schedutil/UAG tunables لكل كلستر (Silver, Gold, Prime)
- **تحسين الذاكرة**: vm.swappiness, dirty pages, ZRAM, VFS cache
- **تحسين I/O**: اختيار scheduler مناسب, read_ahead, queue depth
- **تحسين الشبكة**: TCP buffers, fast open, keepalive
- **إدارة الطاقة**: GPU governor, workqueue efficiency, bus bandwidth

## الكلسترات (SM8350)

| Cluster | CPUs | Core | Max Freq |
|---------|------|------|----------|
| Silver | 0-3 | Cortex-A55 | 1.8 GHz |
| Gold | 4-6 | Cortex-A78 | 2.4 GHz |
| Prime | 7 | Cortex-X1 | 2.84 GHz |

## طريقة البناء

### المتطلبات
1. **Bare-metal cross compiler** (aarch64-none-elf)
   - [تحميل من ARM](https://developer.arm.com/downloads/-/arm-gnu-toolchain-downloads)
2. **KernelPatch source code**
   - `git clone https://github.com/bmax121/KernelPatch.git`

### خطوات البناء

```bash
# 1. استنساخ KernelPatch
git clone https://github.com/bmax121/KernelPatch.git
cd KernelPatch

# 2. نسخ ملفات KPM
git clone https://github.com/smualemi/o9pKPM.git
cp -r o9pKPM/kpm kpms/op9pro-kpm

# 3. تعيين المترجم
export TARGET_COMPILE=aarch64-none-elf-

# 4. البناء
cd kpms/op9pro-kpm
make
```

سيتم إنتاج ملف `op9pro_perf.kpm`.

## طريقة التثبيت

1. ثبّت **APatch** على هاتفك
2. تأكد أن KernelPatch مثبت ومتوافق مع كرنلك
3. افتح APatch → KPM → أضف موديول
4. اختر ملف `op9pro_perf.kpm`
5. في **Args** اكتب الوضع المطلوب:
   - `performance` - أقصى أداء (ألعاب)
   - `balanced` - متوازن (افتراضي)
   - `battery` - توفير بطارية

## التبديل بين الأوضاع

يمكنك التبديل بين الأوضاع بدون إعادة تشغيل عبر APatch:
- افتح APatch → KPM → op9pro-perf → Control
- اكتب أحد الأوامر:
  - `performance`
  - `balanced`
  - `battery`
  - `status` (لمعرفة الوضع الحالي)

## التحقق من العمل

```bash
# فحص السجلات
adb shell dmesg | grep op9pro-perf

# فحص إعدادات CPU
adb shell cat /sys/devices/system/cpu/cpufreq/policy0/scaling_governor
adb shell cat /sys/devices/system/cpu/cpufreq/policy4/scaling_governor
adb shell cat /sys/devices/system/cpu/cpufreq/policy7/scaling_governor

# فحص إعدادات الذاكرة
adb shell cat /proc/sys/vm/swappiness
adb shell cat /proc/sys/vm/dirty_ratio

# فحص I/O scheduler
adb shell cat /sys/block/sda/queue/scheduler
```

## ملاحظات هامة

> ⚠️ **تحذير**: هذا الموديول يعمل على مستوى الكرنل. استخدمه على مسؤوليتك.
>
> - عمل نسخة احتياطية قبل التجربة
> - إذا واجهت مشاكل، أزل الموديول من APatch وأعد التشغيل
> - الوضع الافتراضي (balanced) هو الأكثر أماناً

## التوافق

- **الجهاز**: OnePlus 9 Pro
- **المعالج**: Snapdragon 888 (SM8350/Lahaina)
- **النظام**: OxygenOS 14
- **الكرنل**: QGKI (Clang 11.0.2)
- **يتطلب**: APatch + KernelPatch

## الرخصة

GPL v2
