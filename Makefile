obj-m += main.o
KPM_NAME := oplus_optimize
OUT_DIR := $(PWD)/out

all:
	# استدعاء نظام Kbuild للنواة
	$(MAKE) -C $(KERNEL_DIR) M=$(PWD) ARCH=arm64 CC=clang modules
	mkdir -p $(OUT_DIR)
	cp kpm.json $(OUT_DIR)/
	cp main.ko $(OUT_DIR)/
	# حزم الملفات في صيغة KPM
	cd $(OUT_DIR) && zip -r $(KPM_NAME).kpm main.ko kpm.json
	cp $(OUT_DIR)/$(KPM_NAME).kpm $(PWD)/

clean:
	$(MAKE) -C $(KERNEL_DIR) M=$(PWD) clean
	rm -rf $(OUT_DIR) *.kpm
