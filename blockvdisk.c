#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/vmalloc.h>
#include <linux/blkdev.h>
#include <linux/fs.h>

static unsigned char *ramdisk_ptr = NULL;
static unsigned long ramdisk_size = 4 * 1024 * 1024;

static unsigned int ramdisk_major = 0;

#define RAMDISK_NAME	"bramdisk"

static struct gendisk *ramdisk_gd = NULL;

static spinlock_t queue_lock;
static struct request_queue *q = NULL;

void ramdisk_strategy(struct request_queue *q) {

struct request *req;
unsigned char *starting_addrs;
unsigned long length;

	req = blk_fetch_request(q);
	while (req) {
		starting_addrs = ramdisk_ptr + (blk_rq_pos(req) * 512);
		length = blk_rq_cur_sectors(req) * 512;
	
		if (rq_data_dir(req) == READ) {
			memcpy(req->buffer, starting_addrs, length);
		} else {
			memcpy(starting_addrs, req->buffer, length);
		}

		if(!__blk_end_request_cur(req, 0)) {
			req = blk_fetch_request(q);
		}
	}
	
	return;
}

struct block_device_operations ramdisk_ops = {
	.owner = THIS_MODULE
};

static int __init ramdisk_init(void)
{
	int	ret;

	printk(KERN_INFO "Entering fn %s\n", __FUNCTION__);

	ramdisk_ptr = vmalloc(ramdisk_size * sizeof (char));
	if (!ramdisk_ptr) {
		printk(KERN_ERR "out of memory\n");
		ret = -ENOMEM;
		goto err;
	}

	ramdisk_major = register_blkdev(0, RAMDISK_NAME);
	if (ramdisk_major  < 0) {
		printk(KERN_ERR "no major no\n");
		ret = -EINVAL;
		goto err1;
	}
	
	spin_lock_init(&queue_lock);
	q = blk_init_queue(ramdisk_strategy, &queue_lock);
	if (!q) {
		printk(KERN_ERR "no request queue available\n");
		ret = -EINVAL;
		goto err2;
	}

	ramdisk_gd = alloc_disk(1);
	if (!ramdisk_gd) {
		printk(KERN_ERR "no gendisk obj\n");
		ret =  -EINVAL;
		goto err3;
	}

	ramdisk_gd->major = ramdisk_major;
	ramdisk_gd->first_minor = 0;
	ramdisk_gd->minors = 1;
	ramdisk_gd->fops = &ramdisk_ops;
	ramdisk_gd->queue = q;
	sprintf(ramdisk_gd->disk_name, RAMDISK_NAME);
	set_capacity(ramdisk_gd, ramdisk_size/512);

	add_disk(ramdisk_gd);

	return 0;

err3:
	blk_cleanup_queue(q);
err2:
	unregister_blkdev(ramdisk_major, RAMDISK_NAME);
err1: 
	vfree(ramdisk_ptr);
err:
	return ret;	
}

static void __exit ramdisk_fini(void)
{
	printk(KERN_INFO "Entering fn %s\n", __FUNCTION__);
	del_gendisk(ramdisk_gd);
	put_disk(ramdisk_gd);
	blk_cleanup_queue(q);
	unregister_blkdev(ramdisk_major, RAMDISK_NAME);	
	vfree(ramdisk_ptr);
	return;
}

module_init(ramdisk_init);
module_exit(ramdisk_fini);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("nk@gmail.com");

MODULE_DESCRIPTION("This is simple ramdisk block device driver");
