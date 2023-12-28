#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/miscdevice.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/proc_fs.h>
#include <linux/uaccess.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("lz");

enum { ADD, SUB, MUL = 3, DIV, MOD };
static long ioctl_demo(struct file *filp, unsigned int cmd, unsigned long arg) {
  int nums[2];
  int ret;

  switch (cmd) {
  case ADD:
    if (copy_from_user(nums, (void *)arg, sizeof(nums)))
      return -EFAULT;
    ret = nums[0] + nums[1];
    printk(KERN_INFO "%d + %d = %d\n", nums[0], nums[1], ret);
    break;
  case SUB:
    if (copy_from_user(nums, (void *)arg, sizeof(nums)))
      return -EFAULT;
    ret = nums[0] - nums[1];
    printk(KERN_INFO "%d - %d = %d\n", nums[0], nums[1], ret);
    break;
  case MUL:
    if (copy_from_user(nums, (void *)arg, sizeof(nums)))
      return -EFAULT;
    ret = nums[0] * nums[1];
    printk(KERN_INFO "%d * %d = %d\n", nums[0], nums[1], ret);
    break;
  case DIV:
    if (copy_from_user(nums, (void *)arg, sizeof(nums)) || nums[1] == 0)
      return -EFAULT;
    ret = nums[0] / nums[1];
    printk(KERN_INFO "%d / %d = %d\n", nums[0], nums[1], ret);
    break;
  case MOD:
    if (copy_from_user(nums, (void *)arg, sizeof(nums)) || nums[1] == 0)
      return -EFAULT;
    ret = nums[0] % nums[1];
    printk(KERN_INFO "%d %% %d = %d\n", nums[0], nums[1], ret);
    break;
  default:
    printk(KERN_ALERT "unsupported op: %d\n", cmd);
    return -EFAULT;
  }

  if (copy_to_user(((int *)arg) + 2, &ret, sizeof(ret)))
    return -EFAULT;

  return 0;
}

static struct file_operations ioctl_ops = {
    .unlocked_ioctl = ioctl_demo,
};

static struct miscdevice ioctl_dev = {
    MISC_DYNAMIC_MINOR,
    "ioctl_demo",
    &ioctl_ops,
};

static int ioctl_mod_init(void) {
  int ret;

  ret = misc_register(&ioctl_dev);
  if (ret) {
    pr_err("ioctl_demo: misc device register failed\n");
    return -1;
  }

  printk(KERN_ALERT "ioctl module init!\n");

  return 0;
}

static void ioctl_mod_exit(void) {
  misc_deregister(&ioctl_dev);
  printk(KERN_ALERT "ioctl module exit!\n");
}

module_init(ioctl_mod_init);
module_exit(ioctl_mod_exit);