#ifndef _SWINAPIFAMILY_H_
#define _SWINAPIFAMILY_H_

#include <winapifamily.h>
/*
*
*此文件描述Windows Api的整体框架
*/


/*
 *                                +-------------------+---+
 *                                |      *Partition*  |   |
 *                                +---+---+---+---+---+---+
 *                                |   |   |   |   |   |   |
 *                                |   |   |   |   |   |   |
 *                                |   |   |   | P |   |   |
 *                                |   |   |   | H |   |   |
 *                                | D |   |   | O |   |   |
 *                                | E |   | P | N | S | S |
 *                                | S |   | C | E | Y | E |
 *                                | K |   | _ | _ | S | R |
 *                                | T | A | A | A | T | V |
 * +-------------------------+----+ O | P | P | P | E | E |
 * |     *Platform/Family*       \| P | P | P | P | M | R |
 * +------------------------------+---+---+---+---+---+---+
 * | WINAPI_FAMILY_DESKTOP_APP    | X | X | X |   |   |   |
 * +------------------------------+---+---+---+---+---+---+
 * |      WINAPI_FAMILY_PC_APP    |   | X | X |   |   |   |
 * +------------------------------+---+---+---+---+---+---+
 * |   WINAPI_FAMILY_PHONE_APP    |   | X |   | X |   |   |
 * +----------------------------- +---+---+---+---+---+---+
 * |      WINAPI_FAMILY_SYSTEM    |   |   |   |   | X |   |
 * +----------------------------- +---+---+---+---+---+---+
 * |      WINAPI_FAMILY_SERVER    |   |   |   |   | X | X |
 * +------------------------------+---+---+---+---+---+---+
*/


/*
------------------
minwindef.h
shlwapi.h
------------------
sysinfoapi.h
processthreadsapi.h


*/


#endif
