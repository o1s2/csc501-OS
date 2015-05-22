/* pfintr.S - pfintr */


    	   .text
pferrcode: .long 0
           .globl  pfintr,pferrcode 
pfintr:
	popl pferrcode 	/*store error*/
	pushfl		/*store or save flag_regis*/
	cli			/*disable or clear interrupt*/
	pushal		/*store or save all regis*/
	call pfint		/*call handler*/
	popal			/*restore all regis*/
	popfl			/*restore flag regis*/
	iret


