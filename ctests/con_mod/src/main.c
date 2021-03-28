/*
 * Test the communication between the FPGA and the HPS by reading the
 * controller register and printing the result.
 *
 * Author: Andrew Spaulding
 */

#include <stdio.h>

#include <fp-game/con.h>

int main(void)
{
	printf("Controller state: %x\n", get_con_state());

	return 0;
}
