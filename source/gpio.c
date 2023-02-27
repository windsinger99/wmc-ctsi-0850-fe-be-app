/*
 * gpio.c
 *
 *  Created on: 2020. 3. 31.
 *      Author: lispect-shj
 */

#include "gpio.h"

void Scan_GPIO_PortSet(GPIO_Type *base, uint32_t pin, uint32_t mask)
{
	base->DR ^= (base->DR ^ pin) & mask;
}
