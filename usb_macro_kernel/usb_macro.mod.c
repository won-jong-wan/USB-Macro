#include <linux/module.h>
#include <linux/export-internal.h>
#include <linux/compiler.h>

MODULE_INFO(name, KBUILD_MODNAME);

__visible struct module __this_module
__section(".gnu.linkonce.this_module") = {
	.name = KBUILD_MODNAME,
	.init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
	.exit = cleanup_module,
#endif
	.arch = MODULE_ARCH_INIT,
};



static const struct modversion_info ____versions[]
__used __section("__versions") = {
	{ 0xdb760f52, "__kfifo_free" },
	{ 0xe03c6278, "usb_put_dev" },
	{ 0xfe487975, "init_wait_entry" },
	{ 0x8c26d495, "prepare_to_wait_event" },
	{ 0x8ddd8aad, "schedule_timeout" },
	{ 0x92540fbf, "finish_wait" },
	{ 0xf0fdf6cb, "__stack_chk_fail" },
	{ 0xe3ec2f2b, "alloc_chrdev_region" },
	{ 0xd0a71c16, "class_create" },
	{ 0x8a44e659, "usb_register_driver" },
	{ 0x87255cc0, "class_destroy" },
	{ 0x6091b333, "unregister_chrdev_region" },
	{ 0x291895cd, "usb_alloc_urb" },
	{ 0x52c5c991, "__kmalloc_noprof" },
	{ 0x88db9f48, "__check_object_size" },
	{ 0x13c49cc2, "_copy_from_user" },
	{ 0x4dfa8d4b, "mutex_lock" },
	{ 0x27460c38, "usb_submit_urb" },
	{ 0x3213f038, "mutex_unlock" },
	{ 0x34db050b, "_raw_spin_lock_irqsave" },
	{ 0xf23fcb99, "__kfifo_in" },
	{ 0xd35cce70, "_raw_spin_unlock_irqrestore" },
	{ 0x6b6959da, "usb_deregister" },
	{ 0x13d0adf7, "__kfifo_out" },
	{ 0x281823c5, "__kfifo_out_peek" },
	{ 0x1000e51, "schedule" },
	{ 0x6b10bee1, "_copy_to_user" },
	{ 0x4c03a563, "random_kmalloc_seed" },
	{ 0x4223fd65, "kmalloc_caches" },
	{ 0xf627133, "__kmalloc_cache_noprof" },
	{ 0x63bf652a, "usb_get_dev" },
	{ 0xcefb0c9f, "__mutex_init" },
	{ 0xd9a5ea54, "__init_waitqueue_head" },
	{ 0x139f2189, "__kfifo_alloc" },
	{ 0x95f56d55, "cdev_init" },
	{ 0xd66d7c25, "cdev_add" },
	{ 0x242442c, "device_create" },
	{ 0xbdfb6dbb, "__fentry__" },
	{ 0x65487097, "__x86_indirect_thunk_rax" },
	{ 0x5b8239ca, "__x86_return_thunk" },
	{ 0xe2964344, "__wake_up" },
	{ 0x37a0cba, "kfree" },
	{ 0x2ced8b13, "usb_free_urb" },
	{ 0x122c3a7e, "_printk" },
	{ 0x8cfb3c37, "usb_kill_urb" },
	{ 0xe2c17b5d, "__SCT__might_resched" },
	{ 0x3a0dda20, "device_destroy" },
	{ 0x180acd55, "cdev_del" },
	{ 0x42009cfd, "module_layout" },
};

MODULE_INFO(depends, "");

MODULE_ALIAS("usb:vCAFEp4000d*dc*dsc*dp*icFFisc00ip00in*");

MODULE_INFO(srcversion, "CC15980B45B829EA3645E70");
