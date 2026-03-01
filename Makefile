KPM_NAME := oplus_optimize
OUT_DIR := out

# تحديد مسار المترجم (سيتم توفيره عبر build.yml)
CC := $(NDK_PATH)/toolchains/llvm/prebuilt/linux-x86_64/bin/aarch64-linux-android30-clang

# مسارات الترويسات الخاصة ببيئة KernelPatch المستقلة (لا نستخدم شجرة النواة)
INCLUDES := -I$(KP_DIR)/kernel/patch/include \
            -I$(KP_DIR)/kernel/linux/include \
            -I$(KP_DIR)/kernel/linux/arch/arm64/include \
            -I$(KP_DIR)/kernel/linux/tools/arch/arm64/include

# إعدادات الترجمة لإجبار حقن المقاطع وإخراج ملف .ko صالح
CFLAGS := -target aarch64-linux-android -fPIC -ffreestanding -O2 -Wall $(INCLUDES)

all: build_kpm

build_kpm: main.c kpm.json
	mkdir -p $(OUT_DIR)
	# 1. ترجمة الكود إلى ملف كائن (.o)
	$(CC) $(CFLAGS) -c main.c -o $(OUT_DIR)/main.o
	# 2. تغيير الامتداد إلى .ko ليتعرف عليه APatch (وهو في الواقع مجرد Object File)
	cp $(OUT_DIR)/main.o $(OUT_DIR)/module.ko
	# 3. نسخ التكوين وحزمه
	cp kpm.json $(OUT_DIR)/
	cd $(OUT_DIR) && zip -r $(KPM_NAME).kpm module.ko kpm.json
	cp $(OUT_DIR)/$(KPM_NAME).kpm ./

clean:
	rm -rf $(OUT_DIR) *.kpm
