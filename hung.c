#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/debugfs.h>
#include <linux/fs.h>
#include <linux/list.h>
#include <linux/slab.h>
#include <linux/string.h>
#include <linux/delay.h>

static struct dentry *hung_dentry;
static struct dentry *mutex_dentry;
static struct dentry *msleep_dentry;
static struct dentry *mdelay_dentry;

static DEFINE_MUTEX(mutex);

static void do_mutex_twice(void)
{
	mutex_lock(&mutex);
	mutex_lock(&mutex);
}

static void do_msleep_hung(void)
{
	msleep(1 * 1000 * 1000);
}

static void do_mdelay_hung(void)
{
	mdelay(1 * 1000 * 1000);
}

static ssize_t mutex_store(struct file *file, const char __user *buf,
		size_t size, loff_t *ppos)
{
	int n;

	if (sscanf(buf, "%d", &n) != 1)
		return -EINVAL;

	do_mutex_twice();

	return size;
}

static ssize_t msleep_store(struct file *file, const char __user *buf,
		size_t size, loff_t *ppos)
{
	int n;

	if (sscanf(buf, "%d", &n) != 1)
		return -EINVAL;

	do_msleep_hung();

	return size;
}

static ssize_t mdelay_store(struct file *file, const char __user *buf,
		size_t size, loff_t *ppos)
{
	int n;

	if (sscanf(buf, "%d", &n) != 1)
		return -EINVAL;

	do_mdelay_hung();

	return size;
}

static const struct file_operations mutex_fops = {
	.write = mutex_store,
};

static const struct file_operations msleep_fops = {
	.write = msleep_store,
};

static const struct file_operations mdelay_fops = {
	.write = mdelay_store,
};

static int __init hung_init(void)
{
	hung_dentry = debugfs_create_dir("hung", NULL);

	if (!hung_dentry) {
		pr_err("failed to create /sys/kernel/debug/hung\n");
		return -ENOMEM;
	}

	mutex_dentry = debugfs_create_file("mutex", S_IWUGO, hung_dentry, NULL, &mutex_fops);

	if (!mutex_dentry) {
		pr_err("failed to create /sys/kernel/debug/hung/mutex\n");
		return -ENOMEM;
	}

	msleep_dentry = debugfs_create_file("msleep", S_IWUGO, hung_dentry, NULL, &msleep_fops);

	if (!msleep_dentry) {
		pr_err("failed to create /sys/kernel/debug/hung/msleep\n");
		return -ENOMEM;
	}

	mdelay_dentry = debugfs_create_file("mdelay", S_IWUGO, hung_dentry, NULL, &mdelay_fops);

	if (!mdelay_dentry) {
		pr_err("failed to create /sys/kernel/debug/hung/mdelay\n");
		return -ENOMEM;
	}

	return 0;
}

static void __exit hung_exit(void)
{
	debugfs_remove(mutex_dentry);
	debugfs_remove(msleep_dentry);
	debugfs_remove(mdelay_dentry);
	debugfs_remove(hung_dentry);
}

module_init(hung_init);
module_exit(hung_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("chengyihe");
