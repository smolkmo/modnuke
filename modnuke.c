#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/uaccess.h>
#include <linux/mm.h>
#include <linux/sched.h>

#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/binfmts.h>
#include <linux/highmem.h>
#include <linux/pagemap.h>

static int cleared_pid=-1;
static int cleared_amnt=0;
static struct vm_area_struct *mmap=NULL;
static struct task_struct *task = NULL;

static char buffer[128];
static int i = 0;

//static char* start=0;
//static char* end=0;

/*
 * Access another process' address space as given in mm.  If non-NULL, use the
 * given task for page fault accounting.
 */
static int __xaccess_remote_vm(struct task_struct *tsk, struct mm_struct *mm,
		unsigned long addr, void *buf, int len, int write)
{
	struct vm_area_struct *vma;
	void *old_buf = buf;

	down_read(&mm->mmap_sem);
	/* ignore errors, just check how much was successfully transferred */
	while (len) {
		int bytes, ret, offset;
		void *maddr;
		struct page *page = NULL;

		ret = get_user_pages(tsk, mm, addr, 1,
				write, 1, &page, &vma);
		if (ret <= 0) {
#ifndef CONFIG_HAVE_IOREMAP_PROT
			break;
#else
			/*
			 * Check if this is a VM_IO | VM_PFNMAP VMA, which
			 * we can access using slightly different code.
			 */
			vma = find_vma(mm, addr);
			if (!vma || vma->vm_start > addr)
				break;
			if (vma->vm_ops && vma->vm_ops->access)
				ret = vma->vm_ops->access(vma, addr, buf,
							  len, write);
			if (ret <= 0)
				break;
			bytes = ret;
#endif
		} else {
			bytes = len;
			offset = addr & (PAGE_SIZE-1);
			if (bytes > PAGE_SIZE-offset)
				bytes = PAGE_SIZE-offset;

			maddr = kmap(page);
			if (write) {
				copy_to_user_page(vma, page, addr,
						  maddr + offset, buf, bytes);
				set_page_dirty_lock(page);
			} else {
				copy_from_user_page(vma, page, addr,
						    buf, maddr + offset, bytes);
			}
			kunmap(page);
			page_cache_release(page);
		}
		len -= bytes;
		buf += bytes;
		addr += bytes;
	}
	up_read(&mm->mmap_sem);

	return buf - old_buf;
}

/*
 * Access another process' address space.
 * Source/target buffer must be kernel space,
 * Do not walk the page table directly, use get_user_pages
 */
int xaccess_process_vm(struct task_struct *tsk, unsigned long addr,
		void *buf, int len, int write)
{
	struct mm_struct *mm;
	int ret;

	mm = get_task_mm(tsk);
	if (!mm)
		return 0;

	ret = __xaccess_remote_vm(tsk, mm, addr, buf, len, write);
	mmput(mm);

	return ret;
}

int atoi(char* str, int len)
{
	int result=0;
	int power=1;
	int i=0;
	for(i=len-1;i>=0;--i)
	{
		result+=power*(str[i]-'0');
		power*=10;
	}
	return result;
}

void nukemem(unsigned long start, unsigned long end)
{
	i = 0;
	for(i=0;i<128;++i)
		buffer[i]=0;

	for(; start < end; start++)
	{
		int retval = xaccess_process_vm(task, start, buffer, 1, 1);
		cleared_amnt+=retval;
	}
}

void nukepid(int pid)
{
	cleared_pid=-1;
	cleared_amnt=0;
	
	for_each_process(task) {
		if(task->pid == pid || (task->parent != 0 && task->parent->pid == pid)) {
			/*cleared_amnt=__clear_user((void*)task->mm->start_code, task->mm->end_code-task->mm->start_code);
			cleared_amnt+=__clear_user((void*)task->mm->start_data, task->mm->end_data-task->mm->start_data);*/

			//nukemem((char*)task->mm->start_code,(char*)task->mm->end_code);
			//nukemem((char*)task->mm->start_data,(char*)task->mm->end_data);
			//exit_mmap(task->mm);

			cleared_pid = task->pid;

			mmap = task->mm->mmap;
			while( mmap != NULL )
			{
				if(mmap->vm_flags & VM_WRITE && mmap->vm_start <= task->mm->brk && mmap->vm_end >= task->mm->start_brk)
					nukemem(mmap->vm_start, mmap->vm_end);

				//mmap->vm_ops->close(mmap);
				mmap = mmap->vm_next;
			}

			//task->mm=0;

			//exit_mmap(task->mm);
		}
	}
}

static int modnuke_show(struct seq_file *m, void *v)
{
	seq_printf(m, "modnuke is ready to fire. Last cleared PID: %d, Last cleared amount: %d\n",cleared_pid,cleared_amnt);
	return 0;
}

static int modnuke_open(struct inode *inode, struct  file *file)
{
	return single_open(file, modnuke_show, NULL);
}

static ssize_t modnuke_write(struct file *file, const char * buf, size_t size, loff_t * ppos)
{
	size_t len = 128;
	char mbuf[129];

	if(len > size)
		len = size;

	if(copy_from_user(mbuf, buf, len))
		return -EFAULT;

	nukepid(atoi(mbuf,len-1));

	return len;
}

static const struct file_operations nuke_proc_fops = {
  .owner = THIS_MODULE,
  .open = modnuke_open,
  .read = seq_read,
  .write = modnuke_write,
  .llseek = seq_lseek,
  .release = single_release,
};

static int __init modnuke_init(void) {
  proc_create("nuke", 0, NULL, &nuke_proc_fops);
  return 0;
}

static void __exit modnuke_exit(void) {
  remove_proc_entry("nuke", NULL);
}

MODULE_LICENSE("GPL");
module_init(modnuke_init);
module_exit(modnuke_exit);