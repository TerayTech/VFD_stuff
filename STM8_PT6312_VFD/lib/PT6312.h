#ifndef __PT6312_H
#define __PT6312_H

#include "stm8s.h"

void VFD_Init(void);
void vfd_cmd (u8 vfd_cmd0);
void vfd_wrdata (u8 vfd_wrdata0);               /* VFD  写数据子程序  */
void vfd_init(void);
#endif