#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <paging.h>

int get_bs(bsd_t bs_id, unsigned int npages) {

  /* requests a new mapping of npages with ID map_id */
	//OS proj 3 modify
	/*This call requests from the page server a new
backing store with id store of size npages (in pages, not bytes). If the page server is able to create
the new backingstore, or a backingstore with this ID already exists, the size of the new or existing
backingstore is returned. This size is in pages. If a size of 0 is requested, or the pageserver
encounters an error, SYSERR is returned.
Also for practical reasons, the npages will be no more than 128.
*/
	if((bs_id < 1) || (bs_id >= MAX_ID) || (npages < 1) || (npages > 128))
	{
		return SYSERR;
	}
	/*
	//if npages requested for already mapped backing store, should we check no of free pages and give those pages back ?
	if((bsm_tab[bs_id].bs_status == BSM_MAPPED) && ((128 - bsm_tab[bs_id].bs_npages) > npages))
	{
		return npages;
	}
	*/

	//if backing store is free and npages requested is < 128 return them else if it is mapped and full return error
	if(bsm_tab[bs_id].bs_status == BSM_UNMAPPED)
	{
		return npages;
	}
	else
	{
		return SYSERR;
	}
    //return npages;


}


