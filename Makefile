KPM_NAME := oplus_optimize
OUT_DIR := out

# تحديد مسار مترجم Clang بناءً على متغير NDK_PATH القادم من GitHub Actions
CC := $(NDK_PATH)/toolchains/llvm/prebuilt/linux-x86_64/bin/aarch64-linux-android30-clang

# إعدادات الترجمة: معمارية ARM64، وتضمين مسارات ترويسات KernelPatch والنواة
CFLAGS := -target aarch64-linux-android -fPIC -ffreestanding -O2 -Wall \
          -I. \
          -I$(KERNEL_DIR)/include \
          -I$(KERNEL_DIR)/arch/arm64/include

all: build_kpm

build_kpm: main.c kpm.json
	mkdir -p $(OUT_DIR)
	# ترجمة كود C إلى ملف كائن (Object File)
	$(CC) $(CFLAGS) -c main.c -o $(OUT_DIR)/main.o
	# نسخ ملف التعريف
	cp kpm.json $(OUT_DIR)/
	# ضغط الملفات لتكوين حزمة KPM
	cd $(OUT_DIR) && zip -r $(KPM_NAME).kpm main.o kpm.json
	# نقل الحزمة النهائية إلى المسار الرئيسي ليتمكن GitHub Actions من رفعها
	cp $(OUT_DIR)/$(KPM_NAME).kpm ./

clean:
	rm -rf $(OUT_DIR) *.kpm
