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
	{ 0xb1ad28e0, "__gnu_mcount_nc" },
	{ 0xefd6cf06, "__aeabi_unwind_cpp_pr0" },
	{ 0x92997ed8, "_printk" },
	{ 0x6184edb7, "usb_register_driver" },
	{ 0x37a0cba, "kfree" },
	{ 0x57e5197a, "usb_free_urb" },
	{ 0xf3fb3173, "usb_kill_urb" },
	{ 0xb937a6f4, "device_destroy" },
	{ 0x8616ca1d, "class_destroy" },
	{ 0x6e891e07, "cdev_del" },
	{ 0x6091b333, "unregister_chrdev_region" },
	{ 0x55d32af, "usb_put_dev" },
	{ 0x99fb80f, "usb_alloc_urb" },
	{ 0x9b1485da, "__kmalloc_noprof" },
	{ 0x5f754e5a, "memset" },
	{ 0xae353d77, "arm_copy_from_user" },
	{ 0x828ce6bb, "mutex_lock" },
	{ 0x1a513df1, "usb_submit_urb" },
	{ 0x9618ede0, "mutex_unlock" },
	{ 0x800473f, "__cond_resched" },
	{ 0xfe487975, "init_wait_entry" },
	{ 0x1000e51, "schedule" },
	{ 0x647af474, "prepare_to_wait_event" },
	{ 0x49970de8, "finish_wait" },
	{ 0x51a910c0, "arm_copy_to_user" },
	{ 0x2cfde9a2, "warn_slowpath_fmt" },
	{ 0xf0fdf6cb, "__stack_chk_fail" },
	{ 0x8f678b07, "__stack_chk_guard" },
	{ 0x637493f3, "__wake_up" },
	{ 0x3ca75e69, "usb_deregister" },
	{ 0x95abdc5a, "kmalloc_caches" },
	{ 0xfacf5a1d, "__kmalloc_cache_noprof" },
	{ 0xcccd009e, "usb_get_dev" },
	{ 0xde4bf88b, "__mutex_init" },
	{ 0x5bbe49f4, "__init_waitqueue_head" },
	{ 0xe3ec2f2b, "alloc_chrdev_region" },
	{ 0xec7ecdc0, "cdev_init" },
	{ 0xd5bd7a18, "cdev_add" },
	{ 0x2c9676c6, "class_create" },
	{ 0x3d793542, "device_create" },
	{ 0x8a3dbe73, "module_layout" },
};

MODULE_INFO(depends, "");

MODULE_ALIAS("usb:vDEADpBEEFd*dc*dsc*dp*ic*isc*ip*in*");

MODULE_INFO(srcversion, "D85174BBD1D75E74844D0E1");
