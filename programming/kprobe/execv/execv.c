/* Trace do_execv.  Taken basically from Documentation/kprobes.txt */
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/sched.h>
#include <linux/kprobes.h>
#include <linux/kallsyms.h>
#include <linux/binfmts.h>
#include <asm/uaccess.h>

static int uol_do_execve (char * filename,
			char __user *__user *argv,
			char __user *__user *envp,
			struct pt_regs * regs)
{
	int i;
	char __user *p;

//	if (strcmp (current->comm, "bash") != 0)
//		jprobe_return ();
		
	printk ("do_execve by %d, %s -> %s: ", current->uid, current->comm, filename);

	for (i = 0; argv[i] != NULL; i++) {
		printk ("%s ", argv[i]);
	}
	printk ("\n");

	jprobe_return();

	return 0;
}

static struct jprobe uol_jprobe = {
	.entry = (kprobe_opcode_t *) uol_do_execve
};

int init_module (void)
{
	int ret;
	uol_jprobe.kp.addr = 
		0xc047ad00;
//		(kprobe_opcode_t *) kallsyms_lookup_name ("do_execve");
	if (!uol_jprobe.kp.addr) {
		return -1;
	}

	if ((ret = register_jprobe (&uol_jprobe)) <0) {
		printk ("register_jprobe failed, returned %d\n", ret);
		return -1;
	}
	printk ("Planted jprobe at %p, handler addr %p\n",
		uol_jprobe.kp.addr, uol_jprobe.entry);
	return 0;
}

void cleanup_module (void)
{
	unregister_jprobe (&uol_jprobe);
	printk ("jprobe unregistered\n");
}

MODULE_LICENSE("GPL");
