KPM_NAME := oplus_optimize
OUT_DIR := out

# تحديد مسار المترجم
CC := $(NDK_PATH)/toolchains/llvm/prebuilt/linux-x86_64/bin/aarch64-linux-android30-clang

# استخراج جميع المجلدات الفرعية داخل مجلد kernel وتمريرها كمسارات بحث (-I)
# هذا يضمن العثور على أي ملف ترويسة (مثل ksyms.h) بغض النظر عن مكانه
INCLUDES := $(shell find $(KP_DIR)/kernel -type d | sed s/^/-I/)

# إعدادات الترجمة:
# إضافة -D__user="" و -D__force="" لتعريف هذه الكلمات كفراغ وتجاوز خطأ الأقواس
CFLAGS := -target aarch64-linux-android -fPIC -ffreestanding -O2 -Wall \
          -D__user="" -D__force="" -D__kprobes="" \
          $(INCLUDES)

all: build_kpm

build_kpm: main.c kpm.json
	mkdir -p $(OUT_DIR)
	# 1. الترجمة إلى Object File
	$(CC) $(CFLAGS) -c main.c -o $(OUT_DIR)/main.o
	# 2. تحويل الاسم ليطابق متطلبات محمل APatch
	cp $(OUT_DIR)/main.o $(OUT_DIR)/module.ko
	# 3. حزم الملفات
	cp kpm.json $(OUT_DIR)/
	cd $(OUT_DIR) && zip -r $(KPM_NAME).kpm module.ko kpm.json
	cp $(OUT_DIR)/$(KPM_NAME).kpm ./

clean:
	rm -rf $(OUT_DIR) *.kpm
