#include <linux/usb.h>
#include <linux/slab.h>
#include <linux/init.h>
#include <linux/module.h>

/* PRE-PROCESSOR FUNCTION */
#define MODULE_NAME "usbfalcon"
#define DEVICE_NAME "Novint Falcon"
#define SUCCESS 0

/* LibFTDI CONSTANTS */
#define SIO_RESET          0
#define SIO_MODEM_CTRL     1
#define SIO_SET_FLOW_CTRL  2
#define SIO_SET_BAUD_RATE  3
#define SIO_SET_DATA       4

#define SIO_RESET_REQUEST_TYPE  0x40
#define SIO_RESET_REQUEST       SIO_RESET
#define SIO_RESET_SIO           0
#define SIO_RESET_PURGE_RX      1
#define SIO_RESET_PURGE_TX      2

#define SIO_SET_MODEM_CTRL_REQUEST_TYPE  0x40
#define SIO_SET_MODEM_CTRL_REQUEST       SIO_MODEM_CTRL

#define SIO_SET_FLOW_CTRL_REQUEST_TYPE   0x40
#define SIO_SET_FLOW_CTRL_REQUEST        SIO_SET_FLOW_CTRL

#define SIO_SET_BAUDRATE_REQUEST_TYPE    0x40
#define SIO_SET_BAUDRATE_REQUEST         SIO_SET_BAUD_RATE

#define SIO_SET_DATA_REQUEST_TYPE        0x40
#define SIO_SET_DATA_REQUEST             SIO_SET_DATA

#define SIO_DISABLE_FLOW_CTRL  0x0
#define SIO_RTS_CTS_HS        (0x1 << 8)
#define SIO_DTR_DSR_HS        (0x2 << 8)
#define SIO_XON_XOFF_HS       (0x4 << 8)

#define SIO_SET_DTR_MASK  0x1
#define SIO_SET_DTR_HIGH  ( 1 | ( SIO_SET_DTR_MASK  << 8))
#define SIO_SET_DTR_LOW   ( 0 | ( SIO_SET_DTR_MASK  << 8))
#define SIO_SET_RTS_MASK  0x2
#define SIO_SET_RTS_HIGH  ( 2 | ( SIO_SET_RTS_MASK << 8 ))
#define SIO_SET_RTS_LOW   ( 0 | ( SIO_SET_RTS_MASK << 8 ))

#define SIO_RTS_CTS_HS (0x1 << 8)

#define INTERFACE_ANY 0
#define INTERFACE_A   1
#define INTERFACE_B   2

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
  .name = "falcon%d",
  .fops = &device_fops
};

struct device_data
{
  struct usb_device    *usb_device;
  struct usb_interface *interface;
};

static int device_open(struct inode *inode, struct file *file)
{
  // function variables
  int result;
  unsigned minor;
  struct usb_device *device;
  struct usb_interface *interface;

  // increment module usage counter
  try_module_get(THIS_MODULE);

  // obtain device details
  minor = iminor(inode);
  interface = usb_find_interface(&device_driver, minor);
  if(!interface) {
    result = -ENXIO;
    goto failed_interface;
  }
  device = interface_to_usbdev(interface);

  // purge communication
  if((result = usb_control_msg(device, usb_sndctrlpipe(device,0), SIO_RESET_REQUEST, SIO_RESET_REQUEST_TYPE, SIO_RESET_PURGE_RX, INTERFACE_ANY, NULL, 0, 1e6)) != 0) {
    goto failed_purge_rx;
  }
  if((result = usb_control_msg(device, usb_sndctrlpipe(device,0), SIO_RESET_REQUEST, SIO_RESET_REQUEST_TYPE, SIO_RESET_PURGE_TX, INTERFACE_ANY, NULL, 0, 1e6)) != 0) {
    goto failed_purge_tx;
  }

  // setup normal mode
  if((result = usb_control_msg(device, usb_sndctrlpipe(device,0), 0x09, 0x40, 1, INTERFACE_ANY, NULL, 0, 1e6)) != 0) {
    goto failed_latency_timer;
  }
  if((result = usb_control_msg(device, usb_sndctrlpipe(device,0), SIO_SET_BAUDRATE_REQUEST, SIO_SET_BAUDRATE_REQUEST_TYPE, 0x2, INTERFACE_ANY, NULL, 0, 0)) != 0) {
    goto failed_baud_rate;
  }
  if((result = usb_control_msg(device, usb_sndctrlpipe(device,0), SIO_RESET_REQUEST, SIO_RESET_REQUEST_TYPE, SIO_RESET_PURGE_RX, INTERFACE_ANY, NULL, 0, 1e6)) != 0) {
    goto failed_purge_rx;
  }
  if((result = usb_control_msg(device, usb_sndctrlpipe(device,0), SIO_RESET_REQUEST, SIO_RESET_REQUEST_TYPE, SIO_RESET_PURGE_TX, INTERFACE_ANY, NULL, 0, 1e6)) != 0) {
    goto failed_purge_tx;
  }

  // report successfully
  return SUCCESS;

failed_interface:
failed_purge_tx:
failed_purge_rx:
failed_latency_timer:
failed_baud_rate:

  // report error
  module_put(THIS_MODULE);
  return result;
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
