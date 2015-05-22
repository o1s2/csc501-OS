/* xm.c = xmmap xmunmap */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <paging.h>


/*-------------------------------------------------------------------------
 * xmmap - xmmap
 *-------------------------------------------------------------------------
 */
SYSCALL xmmap(int virtpage, bsd_t source, int npages)
{
  /* sanity check ! */
  // it will map a source "file" ("backing store" here) of size npages pages to the virtual page virtpage. 
  //A process may call this multiple times to map data structures,code, etc.
  // bsd_t is a typedef for unsigned int in paging.h
  STATWORD ps; 
  int i;
  disable(ps);
  if ( (virtpage < 4096) || ( source < 0 ) || ( source >= MAX_ID) ||(npages < 1) || ( npages >128)){ //OS proj 3 modified npages to 128 source here is the backing store id and >= is added as MAX_ID is changed to 16 but highest backing store id is 15
	kprintf("xmmap call error: parameter error! \n");
  restore(ps);
	return SYSERR;
  }

  if((bsm_tab[source].bs_status == BSM_MAPPED) && (bsm_tab[source].bs_pid == currpid))
  {
    kprintf("xmmap call error: process not mapped to already mapped BS! \n");
    return SYSERR;
  }

  kprintf("xmmap - to be implemented!\n");
  //OS proj 3 modify
  //is get_bs needed here?
  if((get_bs(source, (unsigned int)npages) == SYSERR) && (bsm_tab[source].bs_status == BSM_MAPPED)) // should return npages if available if syserr is returned then backing store is mapped and requested pages are not free
  {
    restore(ps);
    return SYSERR;
  }


  if(bsm_tab[source].bs_status == BSM_UNMAPPED)
  {
    bsm_tab[source].bs_status = BSM_MAPPED;
    bsm_tab[source].bs_pid = currpid;
    bsm_tab[source].bs_vpno = virtpage;
    bsm_tab[source].bs_npages = npages;
  }
  else
  {
    if(bsm_tab[source].bs_status == BSM_MAPPED) 
    {
      if(bsm_tab[source].bs_pid == currpid) 
      {
        if((bsm_tab[source].bs_vpno == virtpage) || ((bsm_tab[source].bs_vpno < virtpage) && (virtpage < (bsm_tab[source].bs_vpno + bsm_tab[source].bs_npages)))) 
        {
          restore(ps);
          return SYSERR;//if the called param virtpage is asked to map again in the same process id and already mapped BS then it is wrong as u r replacing valid pages for pages are unmapped only by xmunmap
        }
        else
        {
          if((128 - bsm_tab[source].bs_npages > npages) && (bsm_tab[source].bs_vpno < virtpage) && (virtpage < bsm_tab[source].bs_vpno + 128))
          {
            for(i = bsm_tab[source].bs_npages; i < (virtpage - bsm_tab[source].bs_vpno - 1) ; ++ i)
            {
              bsm_tab[source].bs_page_validbit[i] = 1; //this says there are empty pages in middle this info might be useful later
            }
            bsm_tab[source].bs_npages += npages;//if the virtpage doesnt fall into already mapped pages but still within 128 then allow BS to grow
          }
          //bsm_tab[source].bs_npages += npages;
          //bsm_tab[source].bs_vpno = virtpage;
          //bsm_tab[source].bs_npages = npages;
        }
        //bsm_tab[source].bs_vpno = virtpage;
        //bsm_tab[source].bs_npages = npages;
      }
      else
      {
        if(bsm_tab[source].bs_pid != currpid)
        {
          restore(ps);
          return SYSERR;
        }
      }
    }
  }
  
  restore(ps);
  return OK; //if BS is unmapped it maps and this OK will return
  //return SYSERR;
}



/*-------------------------------------------------------------------------
 * xmunmap - xmunmap
 *-------------------------------------------------------------------------
 */
SYSCALL xmunmap(int virtpage )
{
  /* sanity check ! */
  int i, j, k;
  //unsigned long PDI, PTI ; 
  //unsigned long PDBRv, PTBR, FBR;
  STATWORD ps; 
  disable(ps);
  if ( (virtpage < 4096) ){ 
	kprintf("xmummap call error: virtpage (%d) invalid! \n", virtpage);
  restore(ps);
	return SYSERR;
  }

  kprintf("To be implemented!");
  //OS proj 3 modify
  /*This call, like munmap, should remove a virtual memory mapping. See man munmap for the details of
the Unix call. if process calls for unmap then the corresponding frames i.e. its dirty bits should be cleared and check if pdbr belongs 
to the virtpage that is getting cleared and move the pdbr down accordingly, remember pdbr has left most 22-31 bits*/

  for(i = 0; i < MAX_ID ; ++i)
  {
    if((bsm_tab[i].bs_vpno <= virtpage) || (virtpage < bsm_tab[i].bs_vpno + bsm_tab[i].bs_npages))//if the given virtpage is a part of any BS
    {
      //before unmapping get the 1024 frames(10bits=offset) related to each virtpage and unmap them and make the dirty bit 0
      //for each virtpage is of 20 bits, virtpage>>10= PDI(page dir index), virtpage & 0x3FF= PTI(page table index)
      //PDBR = page dir base register, PTBR = page table base register, FBR = frame base register
      // *(PDI * 4 + (unsigned long)PDBR) = PTBR;  *(PTI * 4 + (unsigned long)PTBR) = FBR;
      // FBR till FBR + 1024 means 2^10 frames for unknown 10 bits of the single virtpage should be freed/unmapped, make the frm status=unmapped and frm dirty bit=0
      // write the code to free the frame in frame.c
      //PDBRv = proctab[bsm_tab[i].bs_pid].pdbr; //gives the BS's pid's pdbr which is already unsigned long
      for(j = virtpage ; j < bsm_tab[i].bs_vpno + bsm_tab[i].bs_npages ; ++j)//from virtpage till end of the BS
      {
        for(k = 0 ; k < 1024 ; ++k)
        {
          if((frm_tab[k].fr_pid == bsm_tab[i].bs_pid) && (frm_tab[k].fr_vpno == j) && (frm_tab[k].fr_type == FR_PAGE))
          {
            free_frm(k);
          }
        }
      }
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

  restore(ps);
  return SYSERR;
}

