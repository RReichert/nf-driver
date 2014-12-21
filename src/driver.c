#include <linux/init.h>
#include <linux/module.h>

MODULE_LICENSE("GLP");

static int __init falcon_init(void) {
  printk(KERN_ALERT "Hello");
  return 0;
}

static void __exit falcon_exit(void) {
  printk(KERN_ALERT "Good bye\n");
}

module_init(falcon_init);
module_exit(falcon_exit);
