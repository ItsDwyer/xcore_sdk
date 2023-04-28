#ifndef CMX655D_H_
#define CMX655D_H_

// CMX655D Device I2C Address
#define CMX655D_I2C_DEVICE_ADDR 0x54 //Address is correct :/

// Interrupt and Status
#define CMX655D_ISR_ADDRESS 0x00 // Interrupt Status Register
#define CMX655D_ISM_ADDRESS 0x01 // Interrupt Status Mask
#define CMX655D_ISE_ADDRESS 0x02 // Interrupt Status Enable

// Clock & PLL
#define CMX655D_CLKCTRL_ADDRESS 0x03 // Clock Control
#define CMX655D_RDIVHI_ADDRESS 0x04 // R-Divider High Byte
#define CMX655D_RDIVLO_ADDRESS 0x05 // R-Divider Low Byte
#define CMX655D_NDIVHI_ADDRESS 0x06 // N-Divider High Byte
#define CMX655D_NDIVLO_ADDRESS 0x07 // N-Divider Low Byte
#define CMX655D_PLLCTRL_ADDRESS 0x08 // PLL Control

// Serial Audio Interface
#define CMX655D_SAICTRL_ADDRESS 0x09 // Serial Audio Interface Control
#define CMX655D_SAIMUX_ADDRESS 0x0A // Serial Audio Interface Mux

// Record
#define CMX655D_RVF_ADDRESS 0x0C // Record Voice Filters
#define CMX655D_LDCTRL_ADDRESS 0x0D // Left Channel Detection Control
#define CMX655D_RDCTRL_ADDRESS 0x0E // Right Channel Detection Control
#define CMX655D_LEVEL_ADDRESS 0x0F // Record Level
#define CMX655D_NGCTRL_ADDRESS 0x1C // Noise Gate Control
#define CMX655D_NGTIME_ADDRESS 0x1D // Noise Gate Time
#define CMX655D_NGLSTAT_ADDRESS 0x1E // Noise Gate Left Channel Status
#define CMX655D_NGRSTAT_ADDRESS 0x1F // Noise Gate Right Channel Status

// Playback
#define CMX655D_PVF_ADDRESS 0x28 // Playback Voice Filters
#define CMX655D_PREAMP_ADDRESS 0x29 // Playback Preamp Gain
#define CMX655D_VOLUME_ADDRESS 0x2A // Playback Volume
#define CMX655D_ALCCTRL_ADDRESS 0x2B // ALC Control
#define CMX655D_ALCTIME_ADDRESS 0x2C // ALC Time
#define CMX655D_ALCGAIN_ADDRESS 0x2D // ALC Make-up Gain
#define CMX655D_ALCSTAT_ADDRESS 0x2E // ALC Status
#define CMX655D_DST_ADDRESS 0x2F // Digital Side Tone Control
#define CMX655D_CPR_ADDRESS 0x30 // Click-and-Pop Reduction

// General System
#define CMX655D_SYSCTRL_ADDRESS 0x32 // System Control
#define CMX655D_COMMAND_ADDRESS 0x33 // Command Register

// Reserved
#define CMX655D_RESERVED1_ADDRESS 0x34 // Not for customer use.
#define CMX655D_RESERVED2_ADDRESS 0x35 // Not for customer use.
#define CMX655D_RESERVED3_ADDRESS 0x36 // Not for customer use.
#define CMX655D_RESERVED4_ADDRESS 0x37 // Not for customer use.

#endif // CMX655D_H_
