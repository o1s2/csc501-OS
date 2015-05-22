/* policy.c = srpolicy*/

#include <conf.h>
#include <kernel.h>
#include <paging.h>


extern int page_replace_policy;
/*-------------------------------------------------------------------------
 * srpolicy - set page replace policy 
 *-------------------------------------------------------------------------
 */
SYSCALL srpolicy(int policy)
{
  /* sanity check ! */

  kprintf("To be implemented!\n");

  if(policy == FIFO)
  {
  	page_replace_policy = FIFO;
  }

  if(policy == LRU)
  {
  	page_replace_policy = LRU;
  }

  if((policy == FIFO) || (policy == LRU))
  {
  	return SYSERR;
  }

  return OK;
}

/*-------------------------------------------------------------------------
 * grpolicy - get page replace policy 
 *-------------------------------------------------------------------------
 */
SYSCALL grpolicy()
{
  return page_replace_policy;
}
