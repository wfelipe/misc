#ifndef __CFS_H__
#define __CFS_H__

struct cfs_compress {
	u32 compress;
	u32 uncompress;
};

struct cfs_inode {
	u64 compress_size;
	u64 real_size;
	struct cfs_compress *compress;
	struct inode vfs_inode;
};

static const struct super_operations cfs_s_ops;
static const struct address_space cfs_aops;
static const struct inode_operations cfs_file_inode_operations;
static const struct inode_operations cfs_dir_inode_operations;
static const struct file_operations cfs_file_operations;

/*
static struct inode *cfs_alloc_inode (struct super_block *sb);
static void cfs_destroy_inode (struct inode *inode);
*/

static inline struct cfs_inode *CFS_I (const struct inode *inode)
{
	return container_of (inode, struct cfs_inode, vfs_inode);
}

#endif