#include <linux/usb.h>
#include <linux/init.h>
#include <linux/module.h>

#define DEVICE_NAME "Novint Falcon"

#if defined DEBUG
#define LOG(format, ...) printk(KERN_ALERT "[*] " DEVICE_NAME ": " format "\n", ##__VA_ARGS__);
#else
#define LOG(format, ...)
#endif

static int __init falcon_init(void);
static void __exit falcon_exit(void);
static int falcon_probe(struct usb_interface*, const struct usb_device_id*);
static void falcon_disconnect(struct usb_interface *interfac);

static struct usb_device_id falcon_table[] = {
  { USB_DEVICE(0x0403, 0xcb48) },
  {}
};

static struct usb_driver falcon_driver = {
  .name = DEVICE_NAME,
  .id_table = falcon_table,
  .probe = falcon_probe,
  .disconnect = falcon_disconnect
};

static int falcon_probe(struct usb_interface* interface, const struct usb_device_id* id) {
  LOG("device connected (%04X:%04X)", id->idVendor, id->idProduct)
  return 0;
}

static void falcon_disconnect(struct usb_interface *interface) {
  LOG("device disconnected")
  return;
}

static int __init falcon_init(void) {
  LOG("module inserted")
  return usb_register(&falcon_driver);
}

static void __exit falcon_exit(void) {
  LOG("module removed")
  usb_deregister(&falcon_driver);
  return;
}

module_init(falcon_init);
module_exit(falcon_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Rodrigo Reichert");
MODULE_DEVICE_TABLE(usb, falcon_table);
