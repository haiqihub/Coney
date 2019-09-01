#include "type.h"
#include "stdio.h"
#include "const.h"
#include "protect.h"
#include "string.h"
#include "fs.h"
#include "proc.h"
#include "tty.h"
#include "console.h"
#include "global.h"
#include "proto.h"

/******************************************************************/

// animation to start the OS.

/******************************************************************/

PUBLIC void animation()
{
	printf("----------------------------------------------\n");
	milli_delay(500);
	printf("|--------------------------------------------|\n");
	milli_delay(500);
	printf("|-------//-----------------------------------|\n");
	milli_delay(500);
	printf("|----//   //--////--------//----////    //---|\n");
	milli_delay(500);
	printf("|---//     //-//   //--//   //--// //   //---|\n");
	milli_delay(500);
	printf("|----//   //--////----/////-----//  //  //---|\n");
	milli_delay(500);
	printf("|------//-----//------//   -----//   // //---|\n");
	milli_delay(500);
	printf("|-------------//--------//------//    ////---|\n");
	milli_delay(500);
	printf("|--------------------------------------------|\n");
	milli_delay(500);
	printf("|--------------------------------------------|\n");
	milli_delay(500);
	printf("|--------------------------------------------|\n");
	milli_delay(500);
	printf("|--------------------------------------------|\n");
	milli_delay(500);
	printf("----------------------------------------------\n");
	return;
}
