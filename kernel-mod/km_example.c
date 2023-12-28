#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("A simple example Linux module.");
MODULE_VERSION("0.01");

static int __init km_example_init(void) {
  pr_info("Hello, World!\n");
  return 0;
}

static void __exit km_example_exit(void) {
  pr_info("Goodbye, World!\n");
  return;
}
module_init(km_example_init);
module_exit(km_example_exit);