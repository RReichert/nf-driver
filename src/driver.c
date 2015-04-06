#include <linux/usb.h>
#include <linux/slab.h>
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

static struct usb_class_driver device_class =
{
  .name = "usb/falcon%d",
  .fops = &device_fops
};

struct device_data
{
  struct usb_device    *usb_device;
  struct usb_interface *interface;
};

static int device_open(struct inode *inode, struct file *file)
{
  try_module_get(THIS_MODULE);
  LOG("device open");
  return SUCCESS;
}

static ssize_t device_read(struct file *file, char __user *user, size_t size, loff_t *offset)
{
  LOG("device read");
  return 0;
}

static ssize_t device_write(struct file *file, const char __user *user, size_t size, loff_t *offset)
{
  LOG("device write");
  return size;
}

static int device_release(struct inode *inode, struct file *file)
{
  LOG("device release");
  module_put(THIS_MODULE);
  return SUCCESS;
}

static int device_probe(struct usb_interface* interface, const struct usb_device_id* id)
{
  // function variables
  int result;
  unsigned i;
  struct device_data *data;
  struct usb_host_endpoint *endpoint;

  // allocate device data
  data = (struct device_data*) kmalloc(sizeof(struct device_data), GFP_KERNEL);
  if(!data) {
    result = -ENOMEM;
    goto fail_device_data_kmalloc;
  }

  // fill device data
  usb_set_intfdata(interface, data);

  data->interface  = interface;
  data->usb_device = interface_to_usbdev(interface);

  for(i=0; i<interface->cur_altsetting->desc.bNumEndpoints; ++i) {
    endpoint = &interface->cur_altsetting->endpoint[i];
  }

  // register usb device
  result = usb_register_dev(interface, &device_class);
  if(result) {
    goto fail_usb_register_dev;
  }

  return SUCCESS;

fail_usb_register_dev:
  kfree(data);

fail_device_data_kmalloc:

  return result;
}

static void device_disconnect(struct usb_interface *interface)
{
  // function variables
  struct device_data *data;

  // obtain device data
  data = usb_get_intfdata(interface);
  usb_set_intfdata(interface, NULL);

  // unregister device data
  usb_deregister_dev(data->interface, &device_class);

  // free up device data
  kfree(data);

  return;
}

static int __init device_init(void) {
  return usb_register(&device_driver);
}

static void __exit device_exit(void) {
  usb_deregister(&device_driver);
}

module_init(device_init);
module_exit(device_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Rodrigo Reichert");
MODULE_DEVICE_TABLE(usb, device_id_table);
