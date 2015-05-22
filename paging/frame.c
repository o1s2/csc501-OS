/* frame.c - manage physical frames */
#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <paging.h>

/*-------------------------------------------------------------------------
 * init_frm - initialize frm_tab
 *-------------------------------------------------------------------------
 */
SYSCALL init_frm()
{
  kprintf("To be implemented!\n");
  int i;
  for(i = 0; i < NFRAMES; ++i)
  {
  	frm_tab[i].fr_status = FRM_UNMAPPED;
  	frm_tab[i].fr_pid = -1;
  	frm_tab[i].fr_vpno = -1;
  	frm_tab[i].fr_refcnt = -1;
  	frm_tab[i].fr_type = -1;
  	frm_tab[i].fr_dirty = 0;
  	frm_tab[i].fr_loadtime = 0;
  }
  return OK;
}

/*-------------------------------------------------------------------------
 * get_frm - get a free frame according page replacement policy
 *-------------------------------------------------------------------------
 */
//SYSCALL get_frm(int* avail)
 SYSCALL int get_frm() //find a free frame according to page replacement policy FIFO or LRU and return frame number
{
  kprintf("To be implemented!\n");
  int i;
  for(i = 0; i < NFRAMES; ++i)
  {
  	if(frm_tab[i].fr_status == FRM_UNMAPPED)
  	{
  		frm_tab[i].fr_status = FRM_MAPPED;
  		return i; 
  	}
  }
  //if there is no free unmapped frame then check the page policy and get a free frame and evict the other page

  //code for FIFO
  if(page_replace_policy == FIFO)
  {

  }

  //code for LRU
  if(page_replace_policy == LRU)
  {
  	
  }

  return OK;
}

/*-------------------------------------------------------------------------
 * free_frm - free a frame 
 *-------------------------------------------------------------------------
 */
SYSCALL free_frm(int i)
{

  kprintf("To be implemented!\n");
  frm_tab[i].fr_status = FRM_UNMAPPED;
  frm_tab[i].fr_pid = -1;
  frm_tab[i].fr_vpno = -1;
  frm_tab[i].fr_refcnt = -1;
  frm_tab[i].fr_type = -1;
  frm_tab[i].fr_dirty = 0; //when free dirty bit is 0 and when filled set to 1
  frm_tab[i].fr_loadtime = 0;
  return OK;
}

//void map_global_tables()
void map_frame_to_proc_virtpage(int frameno, int proc, unsigned long virt_page, int frame_type)
{
	if(frameno < 0 || frameno >= NFRAMES)
	{
		return SYSERR;
	}
	frm_tab[frameno].fr_status = FRM_MAPPED;
	frm_tab[frameno].fr_pid = proc;
	frm_tab[frameno].fr_vpno = (int)virt_page;
	frm_tab[frameno].fr_refcnt += 1;
	frm_tab[frameno].fr_type = frame_type;
	frm_tab[frameno].fr_dirty = 1;
	frm_tab[frameno].fr_loadtime = ctr1000;
}


/*Initialize page directory and return PDBR*/
unsigned long init_pagedir(int proc) //returns unsigned long PDBR which should be saved in proc table
{
	if(proc < 0 || proc >= NPROC)
	{
		return SYSERR;
	}
	unsigned long pdbr_init;
	int freeframe;
	freeframe = get_frm();
	if(freeframe == SYSERR)
	{
		return SYSERR;
	}
	pdbr_init = 0x00400000 + (unsigned long) freeframe * NBPG;//if it is 0th frame, PDBR starts from 0x004
	//map the frame to page directory
	map_frame_to_proc_virtpage(freeframe, proc, pdbr_init >> 12, FR_DIR);
	/*
	frm_tab[freeframe].fr_status = FRM_MAPPED;
	frm_tab[freeframe].fr_pid = proc;
	frm_tab[freeframe].fr_type = FR_DIR;
	frm_tab[freeframe].fr_dirty = 1;
	frm_tab[freeframe].fr_loadtime = ctr1000;
	//map the first 4 page tables to global data
	//map_global_tables(unsigned long pdbr_init);
	*/
	return pdbr_init;
}

void share_global_tables(unsigned long pdbr_proc)
{
	/*Paging.h contains two structures pd_t and pt_t which contains a lot of bit fields. Initially which
fields should be set in a page directory and a page table?
For page directories, set the following bits and make the other bits zero : pd_write always and pd_pres
whenever the corresponding page tables are present in the main memory.
For the four global page tables, set the following bits: pt_pres and pt_write. You can make others zero.*/
	int i,j;
	pd_t * pd = (pd_t *) pdbr_proc;

	for(i = 0; i < 4; ++i)
	{
		pt_t * pt = (pt_t *) global_pagetable_addr[i];
		(pd + i)->pd_pres = 1;
		(pd + i)->pd_write = 1;
		(pd + i)->pd_base = global_pagetable_addr[i] >> 12;
		for(j = 0; j < 1024; ++j)
		{
			(pt + j)->pt_pres = 1;
			(pt + j)->pt_write = 1;
			(pt + j)->pt_base = (i * 1024 * NBPG + j * NBPG) >> 12;
		}
	}
}





