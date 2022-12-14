/*
 * Copyright (c) 2015, Nordic Semiconductor
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the Institute nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 */
/**
 * \addtogroup nrf52840
 * @{
 *
 * \file
 *         Hardware specific implementation of putchar() and puts() functions.
 * \author
 *         Wojciech Bober <wojciech.bober@nordicsemi.no>
 *         Alex Stanoev <alex@astanoev.com>
 *
 */
#include "contiki.h"
#include "dev/uart0.h"
#include "usb/usb-serial.h"

/*---------------------------------------------------------------------------*/
#ifndef DBG_CONF_USB
#define DBG_CONF_USB 0
#endif

#if DBG_CONF_USB
#define write_byte(b) usb_serial_writeb(b)
#define flush()       usb_serial_flush()
#else
#define write_byte(b) uart0_writeb(b)
#define flush()
#endif
/*---------------------------------------------------------------------------*/
int
dbg_putchar(int c)
{
  write_byte(c);

  if(c == '\n') {
    flush();
  }

  return c;
}
/*---------------------------------------------------------------------------*/
unsigned int
dbg_send_bytes(const unsigned char *s, unsigned int len)
{
  unsigned int i = 0;

  while(s && *s != 0) {
    if(i >= len) {
      break;
    }
    dbg_putchar(*s++);
    i++;
  }

  flush();

  return i;
}
/*---------------------------------------------------------------------------*/
/**
 * @}
 */
