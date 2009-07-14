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

MODULE_LICENSE ("GPL");
MODULE_AUTHOR ("Wilson Felipe");

#define SFS_SUPER_MAGIC 0x19980122
//static struct inode *sfs_make_inode (struct super_block *sb, int mode);
//static struct inode *sfs_alloc_inode (struct super_block *sb);

//static struct kmem_cache *sfs_inode_cachep;

static struct inode *sfs_alloc_inode (struct super_block *sb)
{
	struct inode *vfs_inode;
	//vfs_inode = (struct inode *) kmem_cache_alloc (sfs_inode_cachep, GFP_KERNEL);
	vfs_inode = (struct inode *) kmalloc (sizeof (struct inode), GFP_KERNEL);

	vfs_inode->i_version = 1;

	return vfs_inode;
}

static struct inode *sfs_make_inode (struct super_block *sb, int mode)
{
	struct inode *ret = new_inode (sb);

	printk (KERN_INFO "sfs_make_inode\n");

	if (ret)
	{
		ret->i_mode = mode;
		ret->i_uid = ret->i_gid = 0;
		ret->i_blocks = 0;
		ret->i_atime = ret->i_mtime = ret->i_ctime = CURRENT_TIME;
	}
	return ret;
}

static struct super_operations sfs_s_ops = {
	.statfs		= simple_statfs,
	.drop_inode	= generic_delete_inode,
//	.alloc_inode	= sfs_alloc_inode,
//	.alloc_inode	= sfs_make_inode,
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
	root = sfs_make_inode (sb, S_IFDIR | 0755);
	if (! root)
		goto out;
	root->i_op = &simple_dir_inode_operations;
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
	printk (KERN_INFO "sfs_get_sb\n");
	//return get_sb_bdev (fs_type, flags, dev_name, data, sfs_fill_super, mnt);
	return get_sb_single (fs_type, flags, data, sfs_fill_super, mnt);
}

static struct file_system_type sfs_type = {
	.owner		= THIS_MODULE,
	.name		= "sfs",
	.get_sb		= sfs_get_sb,
	.kill_sb	= kill_litter_super,
//	.kill_sb	= kill_block_super,
//	.fs_flags	= FS_REQUIRES_DEV,
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

module_init (init_sfs);
module_exit (exit_sfs);
