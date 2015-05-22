/* bsm.c - manage the backing store mapping*/

#include <conf.h>
#include <kernel.h>
#include <paging.h>
#include <proc.h>

/*-------------------------------------------------------------------------
 * init_bsm- initialize bsm_tab
 *-------------------------------------------------------------------------
 */
SYSCALL init_bsm()
{
	int i,j;
	for(i = 0; i < MAX_ID; ++i)
	{
		bsm_tab[i].bs_status = BSM_UNMAPPED;
		bsm_tab[i].bs_pid = -1;
		bsm_tab[i].bs_vpno = -1;
		bsm_tab[i].bs_npages = -1;
		for(j = 0; j < 128 ; ++j)
		{
		  bs_page_validbit[j] = 0;
		}
	}
}

/*-------------------------------------------------------------------------
 * get_bsm - get a free entry from bsm_tab 
 *-------------------------------------------------------------------------
 */
SYSCALL int get_bsm()
{
	int i;
	for(i = 0; i < MAX_ID; ++i)
	{
		if(bsm_tab[i].bs_status == BSM_UNMAPPED)
		{
			bsm_tab[i].bs_status == BSM_MAPPED;
			return i;
		}
	}
	return SYSERR;
}


/*-------------------------------------------------------------------------
 * free_bsm - free an entry from bsm_tab 
 *-------------------------------------------------------------------------
 */
SYSCALL free_bsm(int i)
{
}

/*-------------------------------------------------------------------------
 * bsm_lookup - lookup bsm_tab and find the corresponding entry
 *-------------------------------------------------------------------------
 */
//SYSCALL bsm_lookup(int pid, long vaddr, int* store, int* pageth)
 SYSCALL int ispagefaultaadr_mapped_bsm_lookup(int pid, long vaddr)
{
	int i;
	for(i = 0; i < MAX_ID; ++i)
	{
		if((bsm_tab[i].bs_status == BSM_MAPPED) && (bsm_tab[i].bs_pid == pid) && (bsm_tab[i].bs_vpno <= vaddr) && (vaddr <= bsm_tab[i].bs_vpno + bsm_tab[i].bs_npages))
		{
			return i;
		}
	}
	return SYSERR;//if the for loop did not work then syserr is returned
}


/*-------------------------------------------------------------------------
 * bsm_map - add an mapping into bsm_tab 
 *-------------------------------------------------------------------------
 */
SYSCALL bsm_map(int pid, int vpno, int store, int npages) //this is different from xmmap as the current process does not 
{
	if(npages < 1 || npages > 128 || pid < 0 || pid >= NPROC || store < 0 || store >= MAX_ID)
	{
		return SYSERR;
	}
	bsm_tab[store].bs_status = BSM_MAPPED;
	bsm_tab[store].bs_pid = pid;
	bsm_tab[store].bs_vpno = vpno;
	bsm_tab[store].bs_npages = npages;
	return OK;
}



/*-------------------------------------------------------------------------
 * bsm_unmap - delete an mapping from bsm_tab
 *-------------------------------------------------------------------------
 */
SYSCALL bsm_unmap(int pid, int vpno, int flag)
{
}


