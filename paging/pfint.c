/* pfint.c - pfint */

#include <conf.h>
#include <kernel.h>
#include <paging.h>


/*-------------------------------------------------------------------------
 * pfint - paging fault ISR
 *-------------------------------------------------------------------------
 */
SYSCALL pfint()
{

  kprintf("To be implemented!\n");
  //OS proj 3 modify
  unsigned long pagefault_addr = read_cr2();
  int i, bsmap_check = 0, backingstore = -1;
  
  if((pagefault_addr >> 12) < 4096)
  {
    kill(currpid);//if it illegal address that is accessed kill the process
    return SYSERR;
  }

  for( i =0; i < MAX_ID; ++i)
  {
    if((bsm_tab[i].bs_status == BSM_MAPPED) && (bsm_tab[i].bs_pid == currpid) && (bsm_tab[i].bs_vpno <= (pagefault_addr >> 12)) && ((pagefault_addr >> 12) <= bsm_tab[i].bs_vpno + bsm_tab[i].bs_npages))//if curr process has a BS mapped, if it does not have a BS mapped give syserr
    {
      bsmap_check = 1;
      break;
    }
  }
  if(bsmap_check == 0) //if there are no BS at all this variable will be 0 or if the virtpage is not mapped into any of the BS that process has this var= 0
  {
    kill(currpid);//if it illegal address that is accessed kill the process
    return SYSERR;
  }
  unsigned long current_pdbr = proctab[currpid].pdbr;

  unsigned int pd_extract = pagefault_addr >> 22; //gives the page directory index
  unsigned int pt_extract = (pagefault_addr >> 12) & 0x03FF; //gives the middle 10 bits of page table index
  unsigned int pageoffset_extract = pagefault_addr & 0x0FFF; //gives the last 12 bits offset

  int free_frame_pf;

  //for each virtpage is of 20 bits, virtpage>>10= PDI(page dir index), virtpage & 0x3FF= PTI(page table index)
  //PDBR = page dir base register, PTBR = page table base register, FBR = frame base register
  // *(PDI * 4 + (unsigned long)PDBR) = PTBR;  *(PTI * 4 + (unsigned long)PTBR) = FBR;
  // FBR till FBR + 1024 means 2^10 frames for unknown 10 bits of the single virtpage should be freed/unmapped, make the frm status=unmapped and frm dirty bit=0
  pd_t * pd_entry = (pd_t *) (current_pdbr + pd_extract);

  if(pd_entry->pd_pres == 0) //page directory is not present
  {
  	free_frame_pf = get_frm(); //gives a free frame number
  	if(free_frame_pf == SYSERR || free_frame_pf < 0 || free_frame_pf > 1023)
  	{
  		return SYSERR;
  	}
  	//void map_frame_to_proc_virtpage(int frameno, int proc, unsigned long virt_page, int frame_type)
  	map_frame_to_proc_virtpage(free_frame_pf, currpid, pagefault_addr >> 12, FR_TBL);
  	pd_entry->pd_pres = 1;
  	pd_entry->pd_write = 1;
  	pd_entry->pd_base = (0x00400000 + free_frame_pf * NBPG) >> 12; //1024 frame number = address translation is 1024 * NBPG(4096 bytes) = 0x00400000
  }
  else
  {
  	if(pd_entry->pd_pres == 1) //page directory is present, now check for page table
    {
      pt_t * pt_entry = (pt_t *) ((pd_entry->pd_base >> 12) + pt_extract) //gives the pt struct at the page table index
      if(pt_entry->pt_pres == 0)
      {
        free_frame_pf = get_frm(); //gives a free frame number
        if(free_frame_pf == SYSERR || free_frame_pf < 0 || free_frame_pf > 1023)
        {
          return SYSERR;
        }
        map_frame_to_proc_virtpage(free_frame_pf, currpid, pagefault_addr >> 12, FR_PAGE);
        pt_entry->pt_pres = 1;
        pt_entry->pt_write = 1;
        pt_entry->pt_base = (0x00400000 + free_frame_pf * NBPG) >> 12;
        backingstore = ispagefaultaadr_mapped_bsm_lookup(currpid, pagefault_addr >> 12);
        if(backingstore == SYSERR)
        {
          kill(currpid);
          return SYSERR;
        }
        //read_bs(char *dst, bsd_t bs_id, int page)
        read_bs((char *)(0x00400000 + free_frame_pf * NBPG), backingstore, pagefault_addr >> 12 - bsm_tab[backingstore].bs_vpno);
      }
    }
  }

  return OK; 
}


