/*
 * Copyright (c) 2015, Benoît Thébaudeau <benoit.thebaudeau.dev@gmail.com>
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
 *
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE
 * COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 */
/**
 * \addtogroup cc2538-examples
 * @{
 *
 * \defgroup cc2538-ctr-test cc2538d AES-CTR Test Project
 *
 *   AES-CTR access example for CC2538-based platforms
 *
 *   This example shows how AES-CTR should be used. The example also verifies
 *   the AES-CTR functionality.
 *
 * @{
 *
 * \file
 *     Example demonstrating AES-CTR
 */
#include "contiki.h"
#include "sys/rtimer.h"
#include "dev/rom-util.h"
#include "dev/ctr.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
/*---------------------------------------------------------------------------*/
#define NONCE_MAX_LEN   0
#define ICTR_MAX_LEN    16
#define MDATA_MAX_LEN   64
/*---------------------------------------------------------------------------*/
PROCESS(ctr_test_process, "ctr test process");
AUTOSTART_PROCESSES(&ctr_test_process);
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(ctr_test_process, ev, data)
{
  static const char *const str_res[] = {
    "success",
    "invalid param",
    "NULL error",
    "resource in use",
    "DMA bus error",
    "keystore read error",
    "keystore write error",
    "authentication failed"
  };
  static const uint8_t keys128[][128 / 8] = {
    { 0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6,
      0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c }
  };
  static const uint8_t keys192[][192 / 8] = {
    { 0x8e, 0x73, 0xb0, 0xf7, 0xda, 0x0e, 0x64, 0x52,
      0xc8, 0x10, 0xf3, 0x2b, 0x80, 0x90, 0x79, 0xe5,
      0x62, 0xf8, 0xea, 0xd2, 0x52, 0x2c, 0x6b, 0x7b }
  };
  static const uint8_t keys256[][256 / 8] = {
    { 0x60, 0x3d, 0xeb, 0x10, 0x15, 0xca, 0x71, 0xbe,
      0x2b, 0x73, 0xae, 0xf0, 0x85, 0x7d, 0x77, 0x81,
      0x1f, 0x35, 0x2c, 0x07, 0x3b, 0x61, 0x08, 0xd7,
      0x2d, 0x98, 0x10, 0xa3, 0x09, 0x14, 0xdf, 0xf4 }
  };
  static const struct {
    const void *keys;
    uint8_t key_size;
    uint8_t count;
  } keys[] = {
    { keys128, AES_KEY_STORE_SIZE_KEY_SIZE_128,
      sizeof(keys128) / sizeof(keys128[0]) },
    { keys192, AES_KEY_STORE_SIZE_KEY_SIZE_192,
      sizeof(keys192) / sizeof(keys192[0]) },
    { keys256, AES_KEY_STORE_SIZE_KEY_SIZE_256,
      sizeof(keys256) / sizeof(keys256[0]) }
  };
  static const struct {
    bool encrypt;
    uint8_t key_size_index;
    uint8_t key_area;
    uint8_t nonce[NONCE_MAX_LEN];
    uint8_t ictr[ICTR_MAX_LEN];
    uint8_t ctr_len;
    uint8_t mdata[MDATA_MAX_LEN];
    uint16_t mdata_len;
    uint8_t expected[MDATA_MAX_LEN];
  } vectors[] = {
    {
      true, /* encrypt */
      0, /* key_size_index */
      0, /* key_area */
      {}, /* nonce */
      { 0xf0, 0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7,
        0xf8, 0xf9, 0xfa, 0xfb, 0xfc, 0xfd, 0xfe, 0xff }, /* ictr */
      16, /* ctr_len */
      { 0x6b, 0xc1, 0xbe, 0xe2, 0x2e, 0x40, 0x9f, 0x96,
        0xe9, 0x3d, 0x7e, 0x11, 0x73, 0x93, 0x17, 0x2a,
        0xae, 0x2d, 0x8a, 0x57, 0x1e, 0x03, 0xac, 0x9c,
        0x9e, 0xb7, 0x6f, 0xac, 0x45, 0xaf, 0x8e, 0x51,
        0x30, 0xc8, 0x1c, 0x46, 0xa3, 0x5c, 0xe4, 0x11,
        0xe5, 0xfb, 0xc1, 0x19, 0x1a, 0x0a, 0x52, 0xef,
        0xf6, 0x9f, 0x24, 0x45, 0xdf, 0x4f, 0x9b, 0x17,
        0xad, 0x2b, 0x41, 0x7b, 0xe6, 0x6c, 0x37, 0x10 }, /* mdata */
      64, /* mdata_len */
      { 0x87, 0x4d, 0x61, 0x91, 0xb6, 0x20, 0xe3, 0x26,
        0x1b, 0xef, 0x68, 0x64, 0x99, 0x0d, 0xb6, 0xce,
        0x98, 0x06, 0xf6, 0x6b, 0x79, 0x70, 0xfd, 0xff,
        0x86, 0x17, 0x18, 0x7b, 0xb9, 0xff, 0xfd, 0xff,
        0x5a, 0xe4, 0xdf, 0x3e, 0xdb, 0xd5, 0xd3, 0x5e,
        0x5b, 0x4f, 0x09, 0x02, 0x0d, 0xb0, 0x3e, 0xab,
        0x1e, 0x03, 0x1d, 0xda, 0x2f, 0xbe, 0x03, 0xd1,
        0x79, 0x21, 0x70, 0xa0, 0xf3, 0x00, 0x9c, 0xee } /* expected */
    }, {
      false, /* encrypt */
      0, /* key_size_index */
      0, /* key_area */
      {}, /* nonce */
      { 0xf0, 0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7,
        0xf8, 0xf9, 0xfa, 0xfb, 0xfc, 0xfd, 0xfe, 0xff }, /* ictr */
      16, /* ctr_len */
      { 0x87, 0x4d, 0x61, 0x91, 0xb6, 0x20, 0xe3, 0x26,
        0x1b, 0xef, 0x68, 0x64, 0x99, 0x0d, 0xb6, 0xce,
        0x98, 0x06, 0xf6, 0x6b, 0x79, 0x70, 0xfd, 0xff,
        0x86, 0x17, 0x18, 0x7b, 0xb9, 0xff, 0xfd, 0xff,
        0x5a, 0xe4, 0xdf, 0x3e, 0xdb, 0xd5, 0xd3, 0x5e,
        0x5b, 0x4f, 0x09, 0x02, 0x0d, 0xb0, 0x3e, 0xab,
        0x1e, 0x03, 0x1d, 0xda, 0x2f, 0xbe, 0x03, 0xd1,
        0x79, 0x21, 0x70, 0xa0, 0xf3, 0x00, 0x9c, 0xee }, /* mdata */
      64, /* mdata_len */
      { 0x6b, 0xc1, 0xbe, 0xe2, 0x2e, 0x40, 0x9f, 0x96,
        0xe9, 0x3d, 0x7e, 0x11, 0x73, 0x93, 0x17, 0x2a,
        0xae, 0x2d, 0x8a, 0x57, 0x1e, 0x03, 0xac, 0x9c,
        0x9e, 0xb7, 0x6f, 0xac, 0x45, 0xaf, 0x8e, 0x51,
        0x30, 0xc8, 0x1c, 0x46, 0xa3, 0x5c, 0xe4, 0x11,
        0xe5, 0xfb, 0xc1, 0x19, 0x1a, 0x0a, 0x52, 0xef,
        0xf6, 0x9f, 0x24, 0x45, 0xdf, 0x4f, 0x9b, 0x17,
        0xad, 0x2b, 0x41, 0x7b, 0xe6, 0x6c, 0x37, 0x10 } /* expected */
    }, {
      true, /* encrypt */
      1, /* key_size_index */
      0, /* key_area */
      {}, /* nonce */
      { 0xf0, 0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7,
        0xf8, 0xf9, 0xfa, 0xfb, 0xfc, 0xfd, 0xfe, 0xff }, /* ictr */
      16, /* ctr_len */
      { 0x6b, 0xc1, 0xbe, 0xe2, 0x2e, 0x40, 0x9f, 0x96,
        0xe9, 0x3d, 0x7e, 0x11, 0x73, 0x93, 0x17, 0x2a,
        0xae, 0x2d, 0x8a, 0x57, 0x1e, 0x03, 0xac, 0x9c,
        0x9e, 0xb7, 0x6f, 0xac, 0x45, 0xaf, 0x8e, 0x51,
        0x30, 0xc8, 0x1c, 0x46, 0xa3, 0x5c, 0xe4, 0x11,
        0xe5, 0xfb, 0xc1, 0x19, 0x1a, 0x0a, 0x52, 0xef,
        0xf6, 0x9f, 0x24, 0x45, 0xdf, 0x4f, 0x9b, 0x17,
        0xad, 0x2b, 0x41, 0x7b, 0xe6, 0x6c, 0x37, 0x10 }, /* mdata */
      64, /* mdata_len */
      { 0x1a, 0xbc, 0x93, 0x24, 0x17, 0x52, 0x1c, 0xa2,
        0x4f, 0x2b, 0x04, 0x59, 0xfe, 0x7e, 0x6e, 0x0b,
        0x09, 0x03, 0x39, 0xec, 0x0a, 0xa6, 0xfa, 0xef,
        0xd5, 0xcc, 0xc2, 0xc6, 0xf4, 0xce, 0x8e, 0x94,
        0x1e, 0x36, 0xb2, 0x6b, 0xd1, 0xeb, 0xc6, 0x70,
        0xd1, 0xbd, 0x1d, 0x66, 0x56, 0x20, 0xab, 0xf7,
        0x4f, 0x78, 0xa7, 0xf6, 0xd2, 0x98, 0x09, 0x58,
        0x5a, 0x97, 0xda, 0xec, 0x58, 0xc6, 0xb0, 0x50 } /* expected */
    }, {
      false, /* encrypt */
      1, /* key_size_index */
      0, /* key_area */
      {}, /* nonce */
      { 0xf0, 0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7,
        0xf8, 0xf9, 0xfa, 0xfb, 0xfc, 0xfd, 0xfe, 0xff }, /* ictr */
      16, /* ctr_len */
      { 0x1a, 0xbc, 0x93, 0x24, 0x17, 0x52, 0x1c, 0xa2,
        0x4f, 0x2b, 0x04, 0x59, 0xfe, 0x7e, 0x6e, 0x0b,
        0x09, 0x03, 0x39, 0xec, 0x0a, 0xa6, 0xfa, 0xef,
        0xd5, 0xcc, 0xc2, 0xc6, 0xf4, 0xce, 0x8e, 0x94,
        0x1e, 0x36, 0xb2, 0x6b, 0xd1, 0xeb, 0xc6, 0x70,
        0xd1, 0xbd, 0x1d, 0x66, 0x56, 0x20, 0xab, 0xf7,
        0x4f, 0x78, 0xa7, 0xf6, 0xd2, 0x98, 0x09, 0x58,
        0x5a, 0x97, 0xda, 0xec, 0x58, 0xc6, 0xb0, 0x50 }, /* mdata */
      64, /* mdata_len */
      { 0x6b, 0xc1, 0xbe, 0xe2, 0x2e, 0x40, 0x9f, 0x96,
        0xe9, 0x3d, 0x7e, 0x11, 0x73, 0x93, 0x17, 0x2a,
        0xae, 0x2d, 0x8a, 0x57, 0x1e, 0x03, 0xac, 0x9c,
        0x9e, 0xb7, 0x6f, 0xac, 0x45, 0xaf, 0x8e, 0x51,
        0x30, 0xc8, 0x1c, 0x46, 0xa3, 0x5c, 0xe4, 0x11,
        0xe5, 0xfb, 0xc1, 0x19, 0x1a, 0x0a, 0x52, 0xef,
        0xf6, 0x9f, 0x24, 0x45, 0xdf, 0x4f, 0x9b, 0x17,
        0xad, 0x2b, 0x41, 0x7b, 0xe6, 0x6c, 0x37, 0x10 } /* expected */
    }, {
      true, /* encrypt */
      2, /* key_size_index */
      0, /* key_area */
      {}, /* nonce */
      { 0xf0, 0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7,
        0xf8, 0xf9, 0xfa, 0xfb, 0xfc, 0xfd, 0xfe, 0xff }, /* ictr */
      16, /* ctr_len */
      { 0x6b, 0xc1, 0xbe, 0xe2, 0x2e, 0x40, 0x9f, 0x96,
        0xe9, 0x3d, 0x7e, 0x11, 0x73, 0x93, 0x17, 0x2a,
        0xae, 0x2d, 0x8a, 0x57, 0x1e, 0x03, 0xac, 0x9c,
        0x9e, 0xb7, 0x6f, 0xac, 0x45, 0xaf, 0x8e, 0x51,
        0x30, 0xc8, 0x1c, 0x46, 0xa3, 0x5c, 0xe4, 0x11,
        0xe5, 0xfb, 0xc1, 0x19, 0x1a, 0x0a, 0x52, 0xef,
        0xf6, 0x9f, 0x24, 0x45, 0xdf, 0x4f, 0x9b, 0x17,
        0xad, 0x2b, 0x41, 0x7b, 0xe6, 0x6c, 0x37, 0x10 }, /* mdata */
      64, /* mdata_len */
      { 0x60, 0x1e, 0xc3, 0x13, 0x77, 0x57, 0x89, 0xa5,
        0xb7, 0xa7, 0xf5, 0x04, 0xbb, 0xf3, 0xd2, 0x28,
        0xf4, 0x43, 0xe3, 0xca, 0x4d, 0x62, 0xb5, 0x9a,
        0xca, 0x84, 0xe9, 0x90, 0xca, 0xca, 0xf5, 0xc5,
        0x2b, 0x09, 0x30, 0xda, 0xa2, 0x3d, 0xe9, 0x4c,
        0xe8, 0x70, 0x17, 0xba, 0x2d, 0x84, 0x98, 0x8d,
        0xdf, 0xc9, 0xc5, 0x8d, 0xb6, 0x7a, 0xad, 0xa6,
        0x13, 0xc2, 0xdd, 0x08, 0x45, 0x79, 0x41, 0xa6 } /* expected */
    }, {
      false, /* encrypt */
      2, /* key_size_index */
      0, /* key_area */
      {}, /* nonce */
      { 0xf0, 0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7,
        0xf8, 0xf9, 0xfa, 0xfb, 0xfc, 0xfd, 0xfe, 0xff }, /* ictr */
      16, /* ctr_len */
      { 0x60, 0x1e, 0xc3, 0x13, 0x77, 0x57, 0x89, 0xa5,
        0xb7, 0xa7, 0xf5, 0x04, 0xbb, 0xf3, 0xd2, 0x28,
        0xf4, 0x43, 0xe3, 0xca, 0x4d, 0x62, 0xb5, 0x9a,
        0xca, 0x84, 0xe9, 0x90, 0xca, 0xca, 0xf5, 0xc5,
        0x2b, 0x09, 0x30, 0xda, 0xa2, 0x3d, 0xe9, 0x4c,
        0xe8, 0x70, 0x17, 0xba, 0x2d, 0x84, 0x98, 0x8d,
        0xdf, 0xc9, 0xc5, 0x8d, 0xb6, 0x7a, 0xad, 0xa6,
        0x13, 0xc2, 0xdd, 0x08, 0x45, 0x79, 0x41, 0xa6 }, /* mdata */
      64, /* mdata_len */
      { 0x6b, 0xc1, 0xbe, 0xe2, 0x2e, 0x40, 0x9f, 0x96,
        0xe9, 0x3d, 0x7e, 0x11, 0x73, 0x93, 0x17, 0x2a,
        0xae, 0x2d, 0x8a, 0x57, 0x1e, 0x03, 0xac, 0x9c,
        0x9e, 0xb7, 0x6f, 0xac, 0x45, 0xaf, 0x8e, 0x51,
        0x30, 0xc8, 0x1c, 0x46, 0xa3, 0x5c, 0xe4, 0x11,
        0xe5, 0xfb, 0xc1, 0x19, 0x1a, 0x0a, 0x52, 0xef,
        0xf6, 0x9f, 0x24, 0x45, 0xdf, 0x4f, 0x9b, 0x17,
        0xad, 0x2b, 0x41, 0x7b, 0xe6, 0x6c, 0x37, 0x10 } /* expected */
    }
  };
  static uint8_t mdata[MDATA_MAX_LEN];
  static int i;
  static uint8_t key_size_index = -1, ret;
  static int8_t res;
  static rtimer_clock_t time, time2, total_time;

  PROCESS_BEGIN();

  puts("-----------------------------------------\n"
       "Initializing cryptoprocessor...");
  crypto_init();

  for(i = 0; i < sizeof(vectors) / sizeof(vectors[0]); i++) {
    if(key_size_index != vectors[i].key_size_index) {
      key_size_index = vectors[i].key_size_index;
      printf("-----------------------------------------\n"
             "Filling %d-bit key store...\n", 128 + (key_size_index << 6));
      time = RTIMER_NOW();
      ret = aes_load_keys(keys[key_size_index].keys,
              keys[key_size_index].key_size, keys[key_size_index].count, 0);
      time = RTIMER_NOW() - time;
      printf("aes_load_keys(): %s, %" PRIu32 " us\n", str_res[ret],
             (uint32_t)((uint64_t)time * 1000000 / RTIMER_SECOND));
      PROCESS_PAUSE();
      if(ret != CRYPTO_SUCCESS) {
        break;
      }
    }

    printf("-----------------------------------------\n"
           "Test vector #%d: %s\n"
           "key_area=%d ctr_len=%d mdata_len=%d\n",
           i, vectors[i].encrypt ? "encrypt" : "decrypt",
           vectors[i].key_area, vectors[i].ctr_len, vectors[i].mdata_len);

    /* mdata has to be in SRAM. */
    rom_util_memcpy(mdata, vectors[i].mdata, vectors[i].mdata_len);

    time = RTIMER_NOW();
    ret = ctr_crypt_start(vectors[i].encrypt, vectors[i].key_area,
                          vectors[i].nonce, vectors[i].ictr, vectors[i].ctr_len,
                          mdata, mdata, vectors[i].mdata_len,
                          &ctr_test_process);
    time2 = RTIMER_NOW();
    time = time2 - time;
    total_time = time;
    if(ret == CRYPTO_SUCCESS) {
      PROCESS_WAIT_EVENT_UNTIL((res = ctr_crypt_check_status()) !=
                               CRYPTO_PENDING);
      time2 = RTIMER_NOW() - time2;
      total_time += time2;
    }
    printf("ctr_crypt_start(): %s, %" PRIu32 " us\n", str_res[ret],
           (uint32_t)((uint64_t)time * 1000000 / RTIMER_SECOND));
    if(ret != CRYPTO_SUCCESS) {
      PROCESS_PAUSE();
      continue;
    }
    printf("ctr_crypt_check_status() wait: %s, %" PRIu32 " us\n", str_res[res],
           (uint32_t)((uint64_t)time2 * 1000000 / RTIMER_SECOND));
    PROCESS_PAUSE();
    if(res != CRYPTO_SUCCESS) {
      continue;
    }

    if(rom_util_memcmp(mdata, vectors[i].expected, vectors[i].mdata_len)) {
      puts("Output message does not match expected one");
    } else {
      puts("Output message OK");
    }

    printf("Total duration: %" PRIu32 " us\n",
           (uint32_t)((uint64_t)total_time * 1000000 / RTIMER_SECOND));
  }

  puts("-----------------------------------------\n"
       "Disabling cryptoprocessor...");
  crypto_disable();

  puts("Done!");

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
/**
 * @}
 * @}
 */
