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
#include "sfs.h"

#define SFS_SUPER_MAGIC 0x19980122

static struct inode *sfs_make_inode (struct super_block *sb, int mode, dev_t dev)
{
	struct inode *inode = new_inode (sb);

	printk (KERN_INFO "sfs_make_inode\n");

	if (inode)
	{
		inode->i_mode = mode;
		inode->i_uid = inode->i_gid = 0;
		inode->i_blocks = 0;
		inode->i_atime = inode->i_mtime = inode->i_ctime = CURRENT_TIME;
		switch (mode & S_IFMT) {
			case S_IFREG:
				inode->i_op = &sfs_file_inode_operations;
				inode->i_fop = &sfs_file_operations;
				break;
			case S_IFDIR:
				inode->i_op = &sfs_dir_inode_operations;
				inode->i_fop = &simple_dir_operations;
				break;
			default:
				init_special_inode (inode, mode, dev);
				break;
		}
	}
	return inode;
}

static int sfs_mknod (struct inode *dir, struct dentry *dentry, int mode, dev_t dev)
{
	struct inode *inode = sfs_make_inode (dir->i_sb, mode, dev);
	int error = -ENOSPC;

	if (inode) {
		d_instantiate (dentry, inode);
		dget (dentry);
		error = 0;
		dir->i_mtime = dir->i_ctime = CURRENT_TIME;
	}

	return error;
}

static int sfs_mkdir (struct inode *dir, struct dentry *dentry, int mode)
{
	int retval = sfs_mknod (dir, dentry, mode | S_IFDIR, 0);

	return retval;
}

static int sfs_create (struct inode *dir, struct dentry *dentry, int mode, struct nameidata *nd)
{
	return sfs_mknod (dir, dentry, mode | S_IFREG, 0);
}

static int sfs_symlink (struct inode *dir, struct dentry *dentry, const char *symname)
{
	struct inode *inode;
	int error = -ENOSPC;

	inode = sfs_make_inode (dir->i_sb, S_IFLNK|S_IRWXUGO, 0);
	if (inode) {
		error = page_symlink (inode, symname, strlen (symname) + 1);
		if (!error) {
			d_instantiate (dentry, inode);
			dget (dentry);
			dir->i_mtime = dir->i_ctime = CURRENT_TIME;
		} else
			iput (inode);
	}
	return error;
}

static const struct inode_operations sfs_dir_inode_operations = {
	.create		= sfs_create,
	.lookup		= simple_lookup,
	.link		= simple_link,
	.unlink		= simple_unlink,
	.symlink	= sfs_symlink,
	.mkdir		= sfs_mkdir,
	.rmdir		= simple_rmdir,
	.mknod		= sfs_mknod,
	.rename		= simple_rename,
};

static const struct super_operations sfs_s_ops = {
	.statfs		= simple_statfs,
	.drop_inode	= generic_delete_inode,
	.show_options	= generic_show_options,
};

static int sfs_fill_super (struct super_block *sb, void *data, int silent)
{
	struct inode *root;
	struct dentry *root_dentry;

	printk (KERN_INFO "sfs_fill_super\n");

	/* defining block size, magic number and superblock ops */
	sb->s_blocksize = PAGE_CACHE_SIZE;
	sb->s_blocksize_bits = PAGE_CACHE_SHIFT;
	sb->s_magic = SFS_SUPER_MAGIC;
	sb->s_op = &sfs_s_ops;

	/* creating the root inode */
	root = sfs_make_inode (sb, S_IFDIR | 0755, 0);
	if (! root)
		goto out;
	root->i_op = &sfs_dir_inode_operations;
	root->i_fop = &simple_dir_operations;

	/* create the root directory */
	root_dentry = d_alloc_root (root);
	if (! root_dentry)
		goto out;
	sb->s_root = root_dentry;

	//sfs_create_files (sb, root_dentry);
	return 0;

out:
	return -ENOMEM;
}

static int sfs_get_sb (struct file_system_type *fs_type, int flags,
	const char *dev_name, void *data, struct vfsmount *mnt)
{
	printk (KERN_INFO "sfs_get_sb %s\n", dev_name);
	//return get_sb_bdev (fs_type, flags, dev_name, data, sfs_fill_super, mnt);
	//return get_sb_single (fs_type, flags, data, sfs_fill_super, mnt);
	return get_sb_nodev (fs_type, flags, data, sfs_fill_super, mnt);
}

static struct file_system_type sfs_type = {
	.owner		= THIS_MODULE,
	.name		= "sfs",
	.get_sb		= sfs_get_sb,
	.kill_sb	= kill_litter_super,
	.fs_flags	= FS_REQUIRES_DEV,
	.next		= NULL,
};

static int __init init_sfs (void)
{
	printk (KERN_INFO "init_sfs\n");
	return register_filesystem (&sfs_type);
}

static void __exit exit_sfs (void)
{
	printk (KERN_INFO "exit_sfs\n");
	unregister_filesystem (&sfs_type);
}

MODULE_AUTHOR ("Wilson Felipe");
MODULE_DESCRIPTION ("A simple filesystem");
MODULE_LICENSE ("GPL");
module_init (init_sfs);
module_exit (exit_sfs);
