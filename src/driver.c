#include <linux/usb.h>
#include <linux/init.h>
#include <linux/module.h>

/* PRE-PROCESSOR FUNCTION */
#define MODULE_NAME "usbfalcon"
#define DEVICE_NAME "Novint Falcon"
#define SUCCESS 0

#if defined DEBUG
#define LOG(format, ...) printk(KERN_ALERT "[*] " DEVICE_NAME ": " format "\n", ##__VA_ARGS__)
#else
#define LOG(format, ...) //
#endif

#define ERROR(format, ...) printk(KERN_ERR "[*] " DEVICE_NAME ": " format "\n", ##__VA_ARGS__)

/* PROTOTYPES */
static int __init device_init(void);
static void __exit device_exit(void);

static int device_probe(struct usb_interface*, const struct usb_device_id*);
static void device_disconnect(struct usb_interface *interfac);

static int device_open(struct inode *, struct file *);
static ssize_t device_read(struct file *, char __user *, size_t, loff_t *);
static ssize_t device_write(struct file *, const char __user *, size_t, loff_t *);
static int device_release(struct inode *, struct file *);

/* GLOBAL VARIABLES */
static unsigned int major;

static struct usb_device_id device_id_table[] =
{
  { USB_DEVICE(0x0403, 0xcb48) },
  {}
};

static struct usb_driver device_driver =
{
  .name       = MODULE_NAME,
  .id_table   = device_id_table,
  .probe      = device_probe,
  .disconnect = device_disconnect
};

static struct file_operations device_fops =
{
    .open    = device_open,
    .read    = device_read,
    .write   = device_write,
    .release = device_release,
};

static int device_open(struct inode *inode, struct file *file)
{
  try_module_get(THIS_MODULE);
  return SUCCESS;
}

static ssize_t device_read(struct file *file, char __user *user, size_t size, loff_t *offset)
{
  return 0;
}

static ssize_t device_write(struct file *file, const char __user *user, size_t size, loff_t *offset)
{
  return size;
}

static int device_release(struct inode *inode, struct file *file)
{
  module_put(THIS_MODULE);
  return SUCCESS;
}

static int device_probe(struct usb_interface* interface, const struct usb_device_id* id)
{
  return SUCCESS;
}

static void device_disconnect(struct usb_interface *interface)
{
  return;
}

static int __init device_init(void)
{
  int result;

  result = usb_register(&device_driver);
  if(result) goto fail_usb_register;

  result = register_chrdev(0, MODULE_NAME, &device_fops);
  if(result < 0) goto fail_register_chrdev;
  else {
    major = result;
    LOG("using major (%u)", major);
  }

  return SUCCESS;

  fail_register_chrdev: usb_deregister(&device_driver);
  fail_usb_register: return result;
}

static void __exit device_exit(void) {
  unregister_chrdev(major, MODULE_NAME);
  usb_deregister(&device_driver);
}

module_init(device_init);
module_exit(device_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Rodrigo Reichert");
MODULE_DEVICE_TABLE(usb, device_id_table);
