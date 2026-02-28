KPM_NAME := oplus_optimize
OUT_DIR := $(PWD)/out

# إجبار نظام Kbuild على تضمين ترويسات KernelPatch
ccflags-y += -I$(KP_DIR)/kernel/patch/include
ccflags-y += -I$(KP_DIR)/kernel/linux/include

obj-m += main.o

all:
	$(MAKE) -C $(KERNEL_DIR) M=$(PWD) ARCH=arm64 CROSS_COMPILE=aarch64-linux-android- CLANG_TRIPLE=aarch64-linux-gnu- LLVM=1 LLVM_IAS=1 modules
	mkdir -p $(OUT_DIR)
	cp kpm.json $(OUT_DIR)/
	# تغيير اسم الملف الداخلي إلى module.ko لضمان قراءته بواسطة APatch
	cp main.ko $(OUT_DIR)/module.ko
	cd $(OUT_DIR) && zip -r $(KPM_NAME).kpm module.ko kpm.json
	cp $(OUT_DIR)/$(KPM_NAME).kpm $(PWD)/

clean:
	$(MAKE) -C $(KERNEL_DIR) M=$(PWD) clean
	rm -rf $(OUT_DIR) *.kpm
