/*
 * pixy.h
 *
 *  Created on: 8 may 2024
 *      Author: JVillanueva
 */

#ifndef PIXY_H_
#define PIXY_H_

#include "fsl_i2c.h"
#include "fsl_port.h"
#include "fsl_clock.h"
#include "sound_player.h"


void pixy_mod_init(void);

bill_t get_bill(void);



#endif /* PIXY_H_ */
