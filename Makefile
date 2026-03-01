KPM_NAME := oplus_optimize
OUT_DIR := out

# تحديد مسار المترجم
CC := $(NDK_PATH)/toolchains/llvm/prebuilt/linux-x86_64/bin/aarch64-linux-android30-clang

# البحث الديناميكي عن مجلدات الترويسات لتفادي التغيرات الهيكلية للمستودع
KP_PATCH_INC := $(shell find $(KP_DIR) -name "kpmodule.h" | head -n 1 | xargs dirname)
KP_LINUX_INC := $(shell find $(KP_DIR) -path "*/linux/printk.h" | head -n 1 | xargs dirname | xargs dirname)

# تمرير المسارات المستخرجة
INCLUDES := -I$(KP_PATCH_INC) \
            -I$(KP_LINUX_INC) \
            -I$(KP_LINUX_INC)/arch/arm64/include \
            -I$(KP_LINUX_INC)/tools/arch/arm64/include

# إعدادات الترجمة لإجبار حقن المقاطع
CFLAGS := -target aarch64-linux-android -fPIC -ffreestanding -O2 -Wall $(INCLUDES)

all: build_kpm

build_kpm: main.c kpm.json
	mkdir -p $(OUT_DIR)
	# 1. الترجمة المباشرة إلى ملف كائن (Object File)
	$(CC) $(CFLAGS) -c main.c -o $(OUT_DIR)/main.o
	# 2. تغيير الامتداد ليطابق متطلبات محمل APatch
	cp $(OUT_DIR)/main.o $(OUT_DIR)/module.ko
	# 3. نسخ التكوين وحزمه
	cp kpm.json $(OUT_DIR)/
	cd $(OUT_DIR) && zip -r $(KPM_NAME).kpm module.ko kpm.json
	cp $(OUT_DIR)/$(KPM_NAME).kpm ./

clean:
	rm -rf $(OUT_DIR) *.kpm
