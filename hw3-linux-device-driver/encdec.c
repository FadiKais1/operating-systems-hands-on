#include <linux/ctype.h>
#include <linux/config.h>
#include <linux/module.h>
#include <linux/kernel.h>  	
#include <linux/slab.h>
#include <linux/fs.h>       		
#include <linux/errno.h>  
#include <linux/types.h> 
#include <linux/proc_fs.h>
#include <linux/fcntl.h>
#include <asm/system.h>
#include <asm/uaccess.h>
#include <linux/string.h>

#include "encdec.h"

#define MODULE_NAME "encdec"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Fadi Kees, Daniel Ifrem");

int 	encdec_open(struct inode *inode, struct file *filp);
int 	encdec_release(struct inode *inode, struct file *filp);
int 	encdec_ioctl(struct inode *inode, struct file *filp, unsigned int cmd, unsigned long arg);

ssize_t encdec_read_caesar( struct file *filp, char *buf, size_t count, loff_t *f_pos );
ssize_t encdec_write_caesar(struct file *filp, const char *buf, size_t count, loff_t *f_pos);

ssize_t encdec_read_xor( struct file *filp, char *buf, size_t count, loff_t *f_pos );
ssize_t encdec_write_xor(struct file *filp, const char *buf, size_t count, loff_t *f_pos);

int memory_size = 0;

MODULE_PARM(memory_size, "i");

int major = 0;

// The two device buffers. One for the Caesar device, one for the XOR device.
// Both have size memory_size (set when the module is loaded).
char *caesar_buf = NULL;
char *xor_buf = NULL;

struct file_operations fops_caesar = {
	.open 	 =	encdec_open,
	.release =	encdec_release,
	.read 	 =	encdec_read_caesar,
	.write 	 =	encdec_write_caesar,
	.llseek  =	NULL,
	.ioctl 	 =	encdec_ioctl,
	.owner 	 =	THIS_MODULE
};

struct file_operations fops_xor = {
	.open 	 =	encdec_open,
	.release =	encdec_release,
	.read 	 =	encdec_read_xor,
	.write 	 =	encdec_write_xor,
	.llseek  =	NULL,
	.ioctl 	 =	encdec_ioctl,
	.owner 	 =	THIS_MODULE
};

// Implemetation suggestion:
// -------------------------
// Use this structure as your file-object's private data structure
typedef struct {
	unsigned char key;
	int read_state;
} encdec_private_date;

int init_module(void)
{
	// register the device-driver and get a major number
	major = register_chrdev(major, MODULE_NAME, &fops_caesar);
	if(major < 0)
	{	
		return major;
	}

	// allocate the buffer for the Caesar device
	caesar_buf = kmalloc(memory_size, GFP_KERNEL);
	if(caesar_buf == NULL)
	{
		unregister_chrdev(major, MODULE_NAME);
		return -ENOMEM;
	}

	// allocate the buffer for the XOR device
	xor_buf = kmalloc(memory_size, GFP_KERNEL);
	if(xor_buf == NULL)
	{
		kfree(caesar_buf);
		unregister_chrdev(major, MODULE_NAME);
		return -ENOMEM;
	}

	// start with clean buffers
	memset(caesar_buf, 0, memory_size);
	memset(xor_buf, 0, memory_size);

	return 0;
}

void cleanup_module(void)
{
	// remove the device-driver
	unregister_chrdev(major, MODULE_NAME);

	// free the two device buffers
	kfree(caesar_buf);
	kfree(xor_buf);
}

int encdec_open(struct inode *inode, struct file *filp)
{
	int minor = MINOR(inode->i_rdev);
	encdec_private_date *pd;

	// choose the right file-operations according to the minor
	if(minor == 0)
	{
		filp->f_op = &fops_caesar;
	}
	else
	{
		filp->f_op = &fops_xor;
	}

	// allocate the private data for this open file
	pd = kmalloc(sizeof(encdec_private_date), GFP_KERNEL);
	if(pd == NULL)
	{
		return -ENOMEM;
	}

	// default state for a new file descriptor
	pd->key = 0;
	pd->read_state = ENCDEC_READ_STATE_RAW;

	filp->private_data = pd;

	return 0;
}

int encdec_release(struct inode *inode, struct file *filp)
{
	// free the private data that was allocated in open
	kfree(filp->private_data);

	return 0;
}

int encdec_ioctl(struct inode *inode, struct file *filp, unsigned int cmd, unsigned long arg)
{
	int minor = MINOR(inode->i_rdev);
	encdec_private_date *pd = (encdec_private_date *)filp->private_data;

	switch(cmd)
	{
		// set the encryption key for this file descriptor
		case ENCDEC_CMD_CHANGE_KEY:
			pd->key = (unsigned char)arg;
			break;

		// set the read mode (raw / decrypt) for this file descriptor
		case ENCDEC_CMD_SET_READ_STATE:
			pd->read_state = (int)arg;
			break;

		// reset the data buffer of this device (shared by all its fds)
		case ENCDEC_CMD_ZERO:
			if(minor == 0)
			{
				memset(caesar_buf, 0, memory_size);
			}
			else
			{
				memset(xor_buf, 0, memory_size);
			}
			break;

		default:
			return -ENOTTY;
	}

	return 0;
}

// ----- Caesar device: encrypt (c + key) % 128, decrypt ((c - key) + 128) % 128 -----

ssize_t encdec_read_caesar( struct file *filp, char *buf, size_t count, loff_t *f_pos )
{
	encdec_private_date *pd = (encdec_private_date *)filp->private_data;
	size_t i;
	size_t to_read;
	char *temp;

	// nothing more to read if we are already at the end of the buffer
	if(*f_pos >= memory_size)
	{
		return -EINVAL;
	}

	// read at most until the end of the buffer
	to_read = count;
	if(to_read > memory_size - *f_pos)
	{
		to_read = memory_size - *f_pos;
	}

	// work on a temporary copy so we never change the stored data
	temp = kmalloc(to_read, GFP_KERNEL);
	if(temp == NULL)
	{
		return -ENOMEM;
	}

	for(i = 0; i < to_read; i++)
	{
		if(pd->read_state == ENCDEC_READ_STATE_DECRYPT)
		{
			temp[i] = ((caesar_buf[*f_pos + i] - pd->key) + 128) % 128;
		}
		else
		{
			temp[i] = caesar_buf[*f_pos + i];
		}
	}

	if(copy_to_user(buf, temp, to_read) != 0)
	{
		kfree(temp);
		return -EFAULT;
	}

	kfree(temp);
	*f_pos += to_read;
	return to_read;
}

ssize_t encdec_write_caesar(struct file *filp, const char *buf, size_t count, loff_t *f_pos)
{
	encdec_private_date *pd = (encdec_private_date *)filp->private_data;
	size_t i;
	size_t to_write;
	char *temp;

	// no space left to write
	if(*f_pos >= memory_size)
	{
		return -ENOSPC;
	}

	// write at most until the end of the buffer
	to_write = count;
	if(to_write > memory_size - *f_pos)
	{
		to_write = memory_size - *f_pos;
	}

	// copy the user's data into a temporary buffer
	temp = kmalloc(to_write, GFP_KERNEL);
	if(temp == NULL)
	{
		return -ENOMEM;
	}

	if(copy_from_user(temp, buf, to_write) != 0)
	{
		kfree(temp);
		return -EFAULT;
	}

	// encrypt into the device buffer
	for(i = 0; i < to_write; i++)
	{
		caesar_buf[*f_pos + i] = (temp[i] + pd->key) % 128;
	}

	kfree(temp);
	*f_pos += to_write;
	return to_write;
}

// ----- XOR device: encrypt and decrypt are the same operation (c ^ key) -----

ssize_t encdec_read_xor( struct file *filp, char *buf, size_t count, loff_t *f_pos )
{
	encdec_private_date *pd = (encdec_private_date *)filp->private_data;
	size_t i;
	size_t to_read;
	char *temp;

	// nothing more to read if we are already at the end of the buffer
	if(*f_pos >= memory_size)
	{
		return -EINVAL;
	}

	// read at most until the end of the buffer
	to_read = count;
	if(to_read > memory_size - *f_pos)
	{
		to_read = memory_size - *f_pos;
	}

	// work on a temporary copy so we never change the stored data
	temp = kmalloc(to_read, GFP_KERNEL);
	if(temp == NULL)
	{
		return -ENOMEM;
	}

	for(i = 0; i < to_read; i++)
	{
		if(pd->read_state == ENCDEC_READ_STATE_DECRYPT)
		{
			temp[i] = xor_buf[*f_pos + i] ^ pd->key;
		}
		else
		{
			temp[i] = xor_buf[*f_pos + i];
		}
	}

	if(copy_to_user(buf, temp, to_read) != 0)
	{
		kfree(temp);
		return -EFAULT;
	}

	kfree(temp);
	*f_pos += to_read;
	return to_read;
}

ssize_t encdec_write_xor(struct file *filp, const char *buf, size_t count, loff_t *f_pos)
{
	encdec_private_date *pd = (encdec_private_date *)filp->private_data;
	size_t i;
	size_t to_write;
	char *temp;

	// no space left to write
	if(*f_pos >= memory_size)
	{
		return -ENOSPC;
	}

	// write at most until the end of the buffer
	to_write = count;
	if(to_write > memory_size - *f_pos)
	{
		to_write = memory_size - *f_pos;
	}

	// copy the user's data into a temporary buffer
	temp = kmalloc(to_write, GFP_KERNEL);
	if(temp == NULL)
	{
		return -ENOMEM;
	}

	if(copy_from_user(temp, buf, to_write) != 0)
	{
		kfree(temp);
		return -EFAULT;
	}

	// encrypt into the device buffer
	for(i = 0; i < to_write; i++)
	{
		xor_buf[*f_pos + i] = temp[i] ^ pd->key;
	}

	kfree(temp);
	*f_pos += to_write;
	return to_write;
}
