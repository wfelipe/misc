#include <linux/module.h>
#include <linux/string.h>
#include <linux/fs.h>
#include <linux/slab.h>
#include <linux/init.h>
#include <linux/blkdev.h>
#include <linux/parser.h>
#include <linux/random.h>
#include <linux/buffer_head.h>
#include <linux/exportfs.h>
#include <linux/smp_lock.h>
#include <linux/vfs.h>
#include <linux/seq_file.h>
#include <linux/mount.h>
#include <linux/log2.h>
#include <linux/quotaops.h>
#include <asm/uaccess.h>
#include "cfs.h"

#define cfs_SUPER_MAGIC 0x19980122

static const struct super_operations cfs_s_ops = {
	.alloc_inode	= cfs_alloc_inode,
	.destroy_inode	= cfs_destroy_inode,
	.statfs		= simple_statfs,
	.drop_inode	= generic_delete_inode,
	.show_options	= generic_show_options,
};

static int cfs_fill_super (struct super_block *sb, void *data, int silent)
{
	struct inode *root;
	struct dentry *root_dentry;

	printk (KERN_INFO "cfs_fill_super\n");

	/* defining block size, magic number and superblock ops */
	sb->s_blocksize = PAGE_CACHE_SIZE;
	sb->s_blocksize_bits = PAGE_CACHE_SHIFT;
	sb->s_magic = cfs_SUPER_MAGIC;
	sb->s_op = &cfs_s_ops;

	/* creating the root inode */
	root = cfs_make_inode (sb, S_IFDIR | 0755, 0);
	if (! root)
		goto out;
	root->i_op = &cfs_dir_inode_operations;
	root->i_fop = &simple_dir_operations;

	/* create the root directory */
	root_dentry = d_alloc_root (root);
	if (! root_dentry)
		goto out;
	sb->s_root = root_dentry;

	//cfs_create_files (sb, root_dentry);
	return 0;

out:
	return -ENOMEM;
}

static int cfs_get_sb (struct file_system_type *fs_type, int flags,
	const char *dev_name, void *data, struct vfsmount *mnt)
{
	printk (KERN_INFO "cfs_get_sb %s\n", dev_name);
	//return get_sb_bdev (fs_type, flags, dev_name, data, cfs_fill_super, mnt);
	//return get_sb_single (fs_type, flags, data, cfs_fill_super, mnt);
	return get_sb_nodev (fs_type, flags, data, cfs_fill_super, mnt);
}

static struct file_system_type cfs_type = {
	.owner		= THIS_MODULE,
	.name		= "cfs",
	.get_sb		= cfs_get_sb,
	.kill_sb	= kill_litter_super,
	.fs_flags	= FS_REQUIRES_DEV,
	.next		= NULL,
};

static int __init init_cfs (void)
{
	int err;
	printk (KERN_INFO "init_cfs\n");

	err = cfs_init_cachep ();
	if (err) return err;

	return register_filesystem (&cfs_type);
}

static void __exit exit_cfs (void)
{
	printk (KERN_INFO "exit_cfs\n");

	cfs_destroy_cachep ();
	unregister_filesystem (&cfs_type);
}

MODULE_AUTHOR ("Wilson Felipe");
MODULE_DESCRIPTION ("A simple filesystem");
MODULE_LICENSE ("GPL");
module_init (init_cfs);
module_exit (exit_cfs);
