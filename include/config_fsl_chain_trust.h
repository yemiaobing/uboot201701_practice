/*
 * Copyright 2015 Freescale Semiconductor, Inc.
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#ifndef __CONFIG_FSL_CHAIN_TRUST_H
#define __CONFIG_FSL_CHAIN_TRUST_H

/* For secure boot, since ENVIRONMENT in flash/external memories is
 * not verified, undef CONFIG_ENV_xxx and set default env
 * (CONFIG_ENV_IS_NOWHERE)
 */
#ifdef CONFIG_SECURE_BOOT

#undef CONFIG_ENV_IS_IN_EEPROM
#undef CONFIG_ENV_IS_IN_NAND
#undef CONFIG_ENV_IS_IN_MMC
#undef CONFIG_ENV_IS_IN_SPI_FLASH
#undef CONFIG_ENV_IS_IN_FLASH

#define CONFIG_ENV_IS_NOWHERE

#endif

#ifdef CONFIG_CHAIN_OF_TRUST

#ifndef CONFIG_EXTRA_ENV
#define CONFIG_EXTRA_ENV	""
#endif

/*
 * Control should not reach back to uboot after validation of images
 * for secure boot flow and therefore bootscript should have
 * the bootm command. If control reaches back to uboot anyhow
 * after validating images, core should just spin.
 */

/*
 * Define the key hash for boot script here if public/private key pair used to
 * sign bootscript are different from the SRK hash put in the fuse
 * Example of defining KEY_HASH is
 * #define CONFIG_BOOTSCRIPT_KEY_HASH \
 *	 "41066b564c6ffcef40ccbc1e0a5d0d519604000c785d97bbefd25e4d288d1c8b"
 */

#ifdef CONFIG_BOOTARGS
#define CONFIG_SET_BOOTARGS	"setenv bootargs \'" CONFIG_BOOTARGS" \';"
#else
#define CONFIG_SET_BOOTARGS	"setenv bootargs \'root=/dev/ram "	\
				"rw console=ttyS0,115200 ramdisk_size=600000\';"
#endif


#ifdef CONFIG_BOOTSCRIPT_KEY_HASH
#define CONFIG_SECBOOT \
	"setenv bs_hdraddr " __stringify(CONFIG_BOOTSCRIPT_HDR_ADDR)";" \
	CONFIG_SET_BOOTARGS	\
	CONFIG_EXTRA_ENV	\
	"esbc_validate $bs_hdraddr " \
	  __stringify(CONFIG_BOOTSCRIPT_KEY_HASH)";" \
	"source $img_addr;"	\
	"esbc_halt\0"
#else
#define CONFIG_SECBOOT \
	"setenv bs_hdraddr " __stringify(CONFIG_BOOTSCRIPT_HDR_ADDR)";" \
	CONFIG_SET_BOOTARGS	\
	CONFIG_EXTRA_ENV	\
	"esbc_validate $bs_hdraddr;" \
	"source $img_addr;"	\
	"esbc_halt\0"
#endif

#ifdef CONFIG_BOOTSCRIPT_COPY_RAM
#define CONFIG_BS_COPY_ENV \
	"setenv bs_hdr_ram " __stringify(CONFIG_BS_HDR_ADDR_RAM)";" \
	"setenv bs_hdr_device " __stringify(CONFIG_BS_HDR_ADDR_DEVICE)";" \
	"setenv bs_hdr_size " __stringify(CONFIG_BS_HDR_SIZE)";" \
	"setenv bs_ram " __stringify(CONFIG_BS_ADDR_RAM)";" \
	"setenv bs_device " __stringify(CONFIG_BS_ADDR_DEVICE)";" \
	"setenv bs_size " __stringify(CONFIG_BS_SIZE)";"

/* For secure boot flow, default environment used will be used */
#if defined(CONFIG_SYS_RAMBOOT)
#if defined(CONFIG_RAMBOOT_NAND)
#define CONFIG_BS_COPY_CMD \
	"nand read $bs_hdr_ram $bs_hdr_device $bs_hdr_size ;" \
	"nand read $bs_ram $bs_device $bs_size ;"
#endif /* CONFIG_RAMBOOT_NAND */
#elif defined(CONFIG_SD_BOOT)
#define CONFIG_BS_COPY_CMD \
	"mmc read $bs_hdr_ram $bs_hdr_device $bs_hdr_size ;" \
	"mmc read $bs_ram $bs_device $bs_size ;"
#else /* CONFIG_SD_BOOT */
#define CONFIG_BS_COPY_CMD \
	"cp.b $bs_hdr_device $bs_hdr_ram  $bs_hdr_size ;" \
	"cp.b $bs_device $bs_ram  $bs_size ;"
#endif
#endif /* CONFIG_BOOTSCRIPT_COPY_RAM */

#ifndef CONFIG_BS_COPY_ENV
#define CONFIG_BS_COPY_ENV
#endif

#ifndef CONFIG_BS_COPY_CMD
#define CONFIG_BS_COPY_CMD
#endif

#define CONFIG_CHAIN_BOOT_CMD	CONFIG_BS_COPY_ENV \
				CONFIG_BS_COPY_CMD \
				CONFIG_SECBOOT

#endif
#endif
