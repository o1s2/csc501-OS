/* vcreate.c - vcreate */
    
#include <conf.h>
#include <i386.h>
#include <kernel.h>
#include <proc.h>
#include <sem.h>
#include <mem.h>
#include <io.h>
#include <paging.h>

/*
static unsigned long esp;
*/

LOCAL int	newpid();
/*------------------------------------------------------------------------
 *  create  -  create a process to start running a procedure
 *------------------------------------------------------------------------
 */
SYSCALL vcreate(procaddr,ssize,hsize,priority,name,nargs,args)
	int	*procaddr;		/* procedure address		*/
	int	ssize;			/* stack size in words		*/
	int	hsize;			/* virtual heap size in pages	*/
	int	priority;		/* process priority > 0		*/
	char	*name;			/* name (for debugging)		*/
	int	nargs;			/* number of args that follow	*/
	long	args;			/* arguments (treated like an	*/
					/* array in the code)		*/
{
	kprintf("To be implemented!\n");
	//return OK;
	//OS proj 3 modify
	int noofBS_need, temp_hsize;
	int vcreatepid = create(procaddr,ssize,priority,name,nargs,args); //creates a proc id and return proc id
	if(hsize <= 128)
	{
		int freebs_vcreate = get_bsm();//returns free BS id
		if(freebs_vcreate == SYSERR)
		{
			kprintf("\n No free BS available");
			return SYSERR;
		}
		//bsm_map(int pid, int vpno, int store, int npages)
		bsm_map(vcreatepid, 4096, freebs_vcreate, hsize);
		proctab[vcreatepid].store = freebs_vcreate;
		proctab[vcreatepid].vhpno = 4096;
		proctab[vcreatepid].vhpnpages = hsize;
	}
	if(hsize > 128)
	{
		if(hsize % 128 == 0)
		{
			noofBS_need = hsize / 128;
			temp_hsize = 0;
		}
		else
		{
			noofBS_need = (hsize / 128) + 1;
			temp_hsize = hsize - ((noofBS_need - 1) * 128);
		}
		for(i = 0; i < noofBS_need ; ++ i)
		{
			int freebs_vcreate = get_bsm();//returns free BS id
			if(freebs_vcreate == SYSERR)
			{
				kprintf("\n No free BS available");
				return SYSERR;
			}
			//bsm_map(int pid, int vpno, int store, int npages)
			if(i == noofBS_need - 1)
			{
				bsm_map(vcreatepid, 4096, freebs_vcreate, temp_hsize);
			}
			else
			{
				bsm_map(vcreatepid, 4096, freebs_vcreate, 128);
			}
			proctab[vcreatepid].store = freebs_vcreate;
			proctab[vcreatepid].vhpno = 4096 * NBPG >> 12;
			proctab[vcreatepid].vhpnpages = hsize;
			temp_hsize -= hsize;
		}
	}


}

/*------------------------------------------------------------------------
 * newpid  --  obtain a new (free) process id
 *------------------------------------------------------------------------
 */
LOCAL int	newpid()
{
	int	pid;			/* process id to return		*/
	int	i;

	for (i=0 ; i<NPROC ; i++) {	/* check all NPROC slots	*/
		if ( (pid=nextproc--) <= 0)
			nextproc = NPROC-1;
		if (proctab[pid].pstate == PRFREE)
			return(pid);
	}
	return(SYSERR);
}
