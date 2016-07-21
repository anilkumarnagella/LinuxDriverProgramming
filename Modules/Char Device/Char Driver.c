#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/types.h>
#include <linux/kdev_t.h>
#include <linux/modules.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/moduleparam.h>
#include <linux/fcntl.h>

#define SUCCESS 0
#define DEVICE_NAME "My_First_Character_Device"
#define DEVICE_NUM 4
MODULE_LICENSE("Dual GPL/BSD");
MODULE_AUTHOR("Hamed Jamshidian");

struct scull_dev
{
  struct scull_qset *data;  /*struct to first quantum set*/
  int quantum;              /*the current quantum size*/
  int qset;                 /*the current array size*/
  unsigned long size;       /*amount of data stored here*/
  unsigned int access_key;  /*used by sculluid and scullpriv*/
  struct semaphore sem;     /*mutual exclusion semaphore*/
  struct cdev cdev;         /*char device structre*/
}

static struct file_operations scull_fops = {
  .owner = THIS_MODULE,
  .llseek = cdev_llseek,
  .ioctl = cdev_ioctl,
  .read = cdev_open,
  .write = cdev_write,
  .open = cdev_open,
  .release = cdev_release
}


static int scull_major = 0;
static int scull_minor;
module_param(scull_major, int, 0);
module_param(scull_minor, int, 0);
struct cdev *my_cdev = cdev_alloc();

static int __init cdev_init(void)
{
  dev_t dev;
  int response = 0;
  if(scull_major)
  {
    dev = MKDEV(scull_major, scull_minor);
    response = register_chrdev_region(dev, 1, DEVICE_NAME);
  }
  else
  {
    response = alloc_chrdev_region(&dev, scull_minor, 1, DEVICE_NAME);
    scull_major = MAJOR(dev);
  }
  if(response < 0)
  {
    printk(KERN_ALERT"Character device can not been allocated with %d !!!\n", response);
    return response;
  }
  struct scull_dev scull[4];
  for (int index = 0; index < DEVICE_NUM; index++) {
    scull_setup_cdev(&scull[index], index);
  }
	return SUCCESS;
}
void scull_setup_cdev(struct *scull_dev dev, int index)
{
  cdev_init(&dev->cdev, &scull_fops);
  dev_t devno = MKDEV(scull_major, scull_minor+index);
  dev->cdev.owner = THIS_MODULE;
  dev->cdev.ops = &scull_fops;
  int err = cdev_add(&dev->cdev, devno, 1);
  if(err)
    printk(KERN_INFO"Error %d adding scull%d", err, index);
}
static void __exit cdev_exit(void)
{
  dev_t dev = MKDEV(scull_major, 0);
  unregister_chrdev_region(dev, 1);
  cdev_del(my_cdev);
}
static int cdev_open(struct inode *inode, struct file *filp)
{
  struct scull_dev *dev;
  dev = container_of(inodo->i_cdev, struct scull_dev, cdev);
  filp->private_data = dev;
  if((filp->f_flags & O_ACCMODE) == O_WRONLY)
    scull_trim(dev);
  return SUCCESS;
}
static int cdev_release(struct inode *inode, struct file *filp)
{
  return 0;
}
