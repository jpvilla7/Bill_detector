/*
 * sound_player.h
 *
 *  Created on: 7 may 2024
 *      Author: JVillanueva
 */

#ifndef SOUND_PLAYER_H_
#define SOUND_PLAYER_H_


#include "stdbool.h"

#define TWENTY_ADDR   0x10000
#define FIFTY_ADDR    0x74000
#define HUNDRED_ADDR  0xD8000
#define TWO_HUN_ADDR  0x13C000
#define FIVE_HUN_ADDR 0x1A0000

#define GLOBAL_LEN    33074U

#define DAC_ADDR (void*)(0x400CC000)


typedef enum {
	twenty = 1,
	fifty = 2,
	hundred = 3,
	two_hundred = 4,
	five_hundred = 5,
} bill_t;

void load_sound(bill_t bill_value);

void Init_peri(void);

void play_sound(void);

bool detection_finished(void);


#endif /* SOUND_PLAYER_H_ */
