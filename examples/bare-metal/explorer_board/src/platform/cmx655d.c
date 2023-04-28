// Copyright 2020-2021 XMOS LIMITED.
// This Software is subject to the terms of the XMOS Public Licence: Version 1.

/* System headers */
#include <platform.h>
#include <xs1.h>

/* SDK headers */
#include "xcore_utils.h"
#include "i2c.h"

/* App headers */
#include "cmx655d.h"

static i2c_master_t *l_i2c_master_ctx = 0;

/*
 * Writes a value to a register in the CMX655D DAC chip.
 */
static inline int cmx655d_reg_write(uint8_t reg, uint8_t val)
{
	i2c_regop_res_t ret;

	ret = write_reg(l_i2c_master_ctx, CMX655D_I2C_DEVICE_ADDR, reg, val);

	if (ret == I2C_REGOP_SUCCESS) {
		return 0;
	} else {
        debug_printf("failed to write reg 0x%x val 0x%x\n", reg, val);
		return -1;
	}
}

/*
 * Example configuration of the CMX655D DAC using i2c.
 *
 * For details on the CMX655D registers and configuration sequence,
 * see chapter 6 here: https://www.cmlmicro.com/wp-content/uploads/2018/10/CMX655D_ds.pdf
 *
 * Must be called after the RTOS scheduler is started.
 */
int cmx655d_init(i2c_master_t *i2c_master_ctx)
{
    l_i2c_master_ctx = i2c_master_ctx;

    if (l_i2c_master_ctx == 0) {
        return -1;
    }
	//first main clock config
	if (
		cmx655d_reg_write(CMX655D_CLKCTRL_ADDRESS, 0x25) == 0 && //16ksps, main clock source = LPOSC. (Any other valid codec sample rate may be selected.)
		cmx655d_reg_write(CMX655D_COMMAND_ADDRESS, 0x01) //Issue Clock Start
	) {
		// Wait for 2.5 sec for clock to be ready
        hwtimer_t timer = hwtimer_alloc();
        hwtimer_delay(timer, 250000);
        hwtimer_free(timer);

	} else {
        debug_printf("Error during DAC clock set\n");
		return -1;
	}
	//clock reconfig
	if (
		cmx655d_reg_write(CMX655D_COMMAND_ADDRESS, 0x00)  == 0 && //Issue clock stop
		cmx655d_reg_write(CMX655D_CLKCTRL_ADDRESS, 0x20) == 0//16ksps, main clock source = RCLK divided by 1 (so the externally applied RCLK signal must be 24.576MHz)

	) {
		// Wait for 2.5 sec for clock to be ready again
        hwtimer_t timer = hwtimer_alloc();
        hwtimer_delay(timer, 250000);
        hwtimer_free(timer);
	} else {
        debug_printf("Error during DAC clock reset\n");
		return -1;
	}
	//start main clock again
	if (
		cmx655d_reg_write(CMX655D_COMMAND_ADDRESS, 0x01)  == 0//Issue clock start
	) {
		// Wait for 2.5 sec for clock to be ready again again
        hwtimer_t timer = hwtimer_alloc();
        hwtimer_delay(timer, 250000);
        hwtimer_free(timer);
	} else {
        debug_printf("Error during DAC clock restart\n");
		return -1;
	}
	//start actually setting things up
	if (
		cmx655d_reg_write(CMX655D_SYSCTRL_ADDRESS, 0x32) == 0 && //Disable Serial Audio Interface, Lineout, Power Amplifier, Microphone Left Channel, and Microphone Right Channel
		cmx655d_reg_write(CMX655D_SAICTRL_ADDRESS, 0x09) == 0 && //Configure SAI for I2S format • MSTR b7 = 0: SAI is slave and LRCLK/FS and BCLK are digital inputs • WL b6 = 0: moot; applies only if b7 = 1 • MONO b5 = 0: moot; applies only if b0 = 1 • DLY b4 = 1: SDI/SDO are latched/valid on the 2nd BCLK edge following the LRCLK/FS transition • POL b3 = 1: If PCM=0, Left data if LRCLK=0 and right data if LRCLK=1 • BINV b2 = 0: SDI/SDO and LRCLK/FS change on BLCK negative edge, SDI/SDO are latched on BLCK positive edge • b1 = 0: not used • PCM b0 = 0: LRCLK/FS phase indicates Left or Right data channel
		cmx655d_reg_write(CMX655D_COMMAND_ADDRESS, 0x33) == 0 && //Issue Clock Stop command
		cmx655d_reg_write(CMX655D_RDIVHI_ADDRESS, 0x04) == 0 && //Set RDIV b12:8
		cmx655d_reg_write(CMX655D_RDIVLO_ADDRESS, 0x01) == 0 && //Set RDIV b7:0 = 1 so RDIV b12:0 = 0d1
		cmx655d_reg_write(CMX655D_NDIVHI_ADDRESS, 0x06) == 0 && //Set NDIV b12:8
		cmx655d_reg_write(CMX655D_NDIVLO_ADDRESS, 0x00) == 0 && //Set NDIV b7:0 = 0 so NDIV b12:0 = 0x600 = 0d1536 so PLL VCO will stabilize at 1536 * 16 kHz = 24.576 MHz
		cmx655d_reg_write(CMX655D_PLLCTRL_ADDRESS, 0x08) == 0 && //PLLCTRL b7:4 = 0x0 = 17.5 kohm Loop Filter Resistor PLLCTRL b3:0 = 0x3 = 0.1 uA/cycle Charge Pump Current Gain
		cmx655d_reg_write(CMX655D_CLKCTRL_ADDRESS, 0x03) == 0 && //16ksps, main clock source = RCLK divided by 1 (so the externally applied RCLK signal must be 24.576MHz).
		cmx655d_reg_write(CMX655D_RDIVHI_ADDRESS, 0x00) == 0 && //Set RDIV to 0
		cmx655d_reg_write(CMX655D_RDIVLO_ADDRESS, 0x00) == 0//Set RDIV to 0
	) {
		// Wait for 2.5 sec for clock to be ready again again
        hwtimer_t timer = hwtimer_alloc();
        hwtimer_delay(timer, 250000);
        hwtimer_free(timer);
	} else {
        debug_printf("Error during DAC first setup\n");
		return -1;
	}
	if (
		cmx655d_reg_write(CMX655D_SAICTRL_ADDRESS, 0x98) == 0 && //I2S master mode
		cmx655d_reg_write(CMX655D_SAIMUX_ADDRESS, 0x08) == 0 && //Use the mean of left and right input data for playback
		cmx655d_reg_write(CMX655D_LEVEL_ADDRESS, 0xCC) == 0 && //Record level gains set to 0dB
		cmx655d_reg_write(CMX655D_VOLUME_ADDRESS, 0xD5) == 0 //Playback volume gain set to -6dB with smoothing
	) {
		return 1;
	} else {
        debug_printf("Error during DAC final config\n");
		return -1;
	}
}
