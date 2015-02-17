#include <linux/init.h>
#include <linux/module.h>
#include <linux/usb.h>

static int __init falcon_init(void);
static void __exit falcon_exit(void);
static int falcon_probe(struct usb_interface*, const struct usb_device_id*);
static void falcon_disconnect(struct usb_interface *interfac);

static struct usb_device_id falcon_table[] = {
	{ USB_DEVICE(0x0403, 0xcb48) },
	{}
};

static struct usb_driver falcon_driver = {
	.name = "Novint Falcon",
	.id_table = falcon_table,
	.probe = falcon_probe,
	.disconnect = falcon_disconnect
};

static int falcon_probe(struct usb_interface* interface, const struct usb_device_id* id) {
	printk(KERN_ALERT "[*] Novint Falcon (%04X:%04X): connected\n", id->idVendor, id->idProduct);
	return 0;
}

static void falcon_disconnect(struct usb_interface *interface) {
	printk(KERN_ALERT "[*] Novint Falcon: disconnected\n");
	return;
}

static int __init falcon_init(void) {
	printk(KERN_ALERT "[*] Novint Falcon module inserted\n");
	return usb_register(&falcon_driver);
}

static void __exit falcon_exit(void) {
	printk(KERN_ALERT "[*] Novint Falcon module removed\n");
	usb_deregister(&falcon_driver);
	return;
}

module_init(falcon_init);
module_exit(falcon_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Rodrigo Reichert");
MODULE_DESCRIPTION("Novint Falcon USB Driver");
MODULE_DEVICE_TABLE(usb, falcon_table);
