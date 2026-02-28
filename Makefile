KPM_NAME := oplus_optimize

ifneq ($(KERNELRELEASE),)
    # استخدام $(src) لضمان دقة المسار داخل بيئة Kbuild
    # استخدام EXTRA_CFLAGS كضمان إضافي ضد عمليات المسح الخاصة بـ OPLUS
    EXTRA_CFLAGS += -I$(src)/KernelPatch/kernel/patch/include
    EXTRA_CFLAGS += -I$(src)/KernelPatch/kernel/linux/include
    ccflags-y += -I$(src)/KernelPatch/kernel/patch/include
    ccflags-y += -I$(src)/KernelPatch/kernel/linux/include
    
    obj-m += main.o

else
    OUT_DIR := $(PWD)/out

all:
	# لم يعد هناك حاجة لتمرير KP_DIR كمتغير خارجي هنا
	$(MAKE) -C $(KERNEL_DIR) M=$(PWD) ARCH=arm64 CROSS_COMPILE=aarch64-linux-android- CLANG_TRIPLE=aarch64-linux-gnu- LLVM=1 LLVM_IAS=1 modules
	mkdir -p $(OUT_DIR)
	cp kpm.json $(OUT_DIR)/
	cp main.ko $(OUT_DIR)/module.ko
	cd $(OUT_DIR) && zip -r $(KPM_NAME).kpm module.ko kpm.json
	cp $(OUT_DIR)/$(KPM_NAME).kpm $(PWD)/

clean:
	$(MAKE) -C $(KERNEL_DIR) M=$(PWD) clean
	rm -rf $(OUT_DIR) *.kpm

endif
