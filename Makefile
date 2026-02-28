KPM_NAME := oplus_optimize
OUT_DIR := out

CC := $(NDK_PATH)/toolchains/llvm/prebuilt/linux-x86_64/bin/aarch64-linux-android30-clang

# تحديد مسارات الترويسات الخاصة بـ KernelPatch
INCLUDE_FLAGS := -I$(KP_DIR)/kernel/patch/include \
                 -I$(KP_DIR)/kernel/linux/include \
                 -I$(KP_DIR)/kernel/linux/arch/arm64/include \
                 -I$(KP_DIR)/kernel/linux/tools/arch/arm64/include

# إعدادات الترجمة
CFLAGS := -target aarch64-linux-android -fPIC -ffreestanding -O2 -Wall $(INCLUDE_FLAGS)

all: build_kpm

build_kpm: main.c kpm.json
	mkdir -p $(OUT_DIR)
	$(CC) $(CFLAGS) -c main.c -o $(OUT_DIR)/main.o
	cp kpm.json $(OUT_DIR)/
	cd $(OUT_DIR) && zip -r $(KPM_NAME).kpm main.o kpm.json
	cp $(OUT_DIR)/$(KPM_NAME).kpm ./

clean:
	rm -rf $(OUT_DIR) *.kpm
