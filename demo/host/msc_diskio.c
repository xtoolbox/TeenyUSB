/*       
 *         _______                    _    _  _____ ____  
 *        |__   __|                  | |  | |/ ____|  _ \ 
 *           | | ___  ___ _ __  _   _| |  | | (___ | |_) |
 *           | |/ _ \/ _ \ '_ \| | | | |  | |\___ \|  _ < 
 *           | |  __/  __/ | | | |_| | |__| |____) | |_) |
 *           |_|\___|\___|_| |_|\__, |\____/|_____/|____/ 
 *                               __/ |                    
 *                              |___/                     
 *
 * TeenyUSB - light weight usb stack for STM32 micro controllers
 * 
 * Copyright (c) 2019 XToolBox  - admin@xtoolbox.org
 *                         www.tusb.org
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "ff.h"			/* Obtains integer types */
#include "diskio.h"		/* Declarations of disk functions */
#include "tusbh_msc.h"
#include <string.h>

#define MAX_FF  4
static tusbh_interface_t* mount_itf[MAX_FF] = {0};
static FATFS   mount_ff[MAX_FF];

int msc_ff_mount(tusbh_interface_t* interface, int max_lun, const tusbh_block_info_t* blocks)
{
    for(int i=0;i<MAX_FF;i++){
        if(mount_itf[i] == 0){
            mount_itf[i] = interface;
            char mo[4] = "0:";
            mo[0]+= i;
            FRESULT r = f_mount(&mount_ff[i], mo, 0);
            printf("MSC mount [%s] r = %d\n", mo, r);
            return 0;
        }
    }
    printf("MSC mount fail, no space\n");
    return 0;
}

int msc_ff_unmount(tusbh_interface_t* interface)
{
    for(int i=0;i<MAX_FF;i++){
        if(mount_itf[i] == interface){
            char mo[4] = "0:";
            mo[0]+= i;
            FRESULT r = f_mount(0, mo, 0);
            printf("MSC unmount [%s] r = %d\n", mo, r);
            mount_itf[i] = 0;
            return 0;
        }
    }
    printf("MSC unmount fail, not found\n");
    return 0;
}

FRESULT scan_files (
    char* path
)
{
    FRESULT res;
    DIR dir;
    UINT i;
    static FILINFO fno;
    res = f_opendir(&dir, path);
    if (res == FR_OK) {
        for (;;) {
            res = f_readdir(&dir, &fno);                   /* Read a directory item */
            if (res != FR_OK || fno.fname[0] == 0) break;  /* Break on error or end of dir */
            if (fno.fattrib & AM_DIR) {                    /* It is a directory */
                i = strlen(path);
                sprintf(&path[i], "/%s", fno.fname);
                res = scan_files(path);                    /* Enter the directory */
                if (res != FR_OK) break;
                path[i] = 0;
            } else {                                       /* It is a file. */
                printf("%s/%s\n", path, fno.fname);
            }
        }
        f_closedir(&dir);
    }else{
        printf("Fail to open dir %s, res = %d\n", path, res);
    }
    return res;
}

void cmd_ls(char* argv[], int argc)
{
    if(argc<2){
        printf("ls <dir>\n");
        return;
    }
    scan_files(argv[1]);
}

void cmd_mkdir(char* argv[], int argc)
{
    if(argc<2){
        printf("mkdir <dir>\n");
        return;
    }
    FRESULT res = f_mkdir(argv[1]);
    if(res != FR_OK){
        printf("Fail to execute f_mkdir %d\n", res);
    }
}

void cmd_mv(char* argv[], int argc)
{
    if(argc<3){
        printf("mv <from> <to>\n");
        return;
    }
    FRESULT res = f_rename(argv[1], argv[2]);
    if(res != FR_OK){
        printf("Fail to execute f_rename %d\n", res);
    }
}

void cmd_cat(char* argv[], int argc)
{
    if(argc<2){
        printf("cat <file>\n");
        return;
    }
    FIL fp;
    FRESULT res = f_open(&fp, argv[1], FA_READ);
    if(res != FR_OK){
        printf("Fail to open %s, res = %d\n",argv[1], res);
    }
    char line[64];
    while (f_gets(line, sizeof(line), &fp)) {
        printf(line);
    }
    printf("\n");
    f_close(&fp);
}

void cmd_cp(char* argv[], int argc)
{
    if(argc<3){
        printf("cp <source> <dest>\n");
        return;
    }
    
    FIL fsrc,fdst;
    FRESULT res = f_open(&fsrc, argv[1], FA_READ);
    if(res != FR_OK){
        printf("Fail to open %s, res = %d\n",argv[1], res);
        return;
    }
    
    res = f_open(&fdst, argv[2], FA_WRITE | FA_CREATE_ALWAYS);
    if(res != FR_OK){
        printf("Fail to open %s, res = %d\n",argv[2], res);
        f_close(&fsrc);
        return;
    }
    
    UINT br, bw;
    char buffer[64];
    for (;;) {
        res = f_read(&fsrc, buffer, sizeof(buffer), &br);
        if (res || br == 0) break; /* error or eof */
        res = f_write(&fdst, buffer, br, &bw);
        if (res || bw < br) break; /* error or disk full */
    }
    
    f_close(&fsrc);
    f_close(&fdst);
}

void cmd_rm(char* argv[], int argc)
{
    if(argc<2){
        printf("rm <target>\n");
        return;
    }
    FRESULT res = f_unlink(argv[1]);
    if(res != FR_OK){
        printf("Fail to execute f_unlink %d\n", res);
    }
}

void cmd_append(char* argv[], int argc)
{
    if(argc<3){
        printf("append <file> <txt1> [txt2 txt3 ...]\n");
        return;
    }
    FIL fp;
    FRESULT res = f_open(&fp, argv[1], FA_OPEN_APPEND | FA_WRITE | FA_READ);
    if(res != FR_OK){
        printf("Fail to open %s, res = %d\n",argv[1], res);
        return;
    }
    for(int i=2;i<argc;i++){
        f_puts(argv[i], &fp);
    }
    f_puts("\n", &fp);
    f_close(&fp);
}

DSTATUS disk_status (
	BYTE pdrv		/* Physical drive nmuber to identify the drive */
)
{
    if(pdrv < MAX_FF){
        if(mount_itf[pdrv]){
            return 0;
        }
    }
    return STA_NODISK;
}



/*-----------------------------------------------------------------------*/
/* Inidialize a Drive                                                    */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize (
	BYTE pdrv				/* Physical drive nmuber to identify the drive */
)
{
    if(pdrv < MAX_FF){
        if(mount_itf[pdrv]){
            if(tusbh_msc_is_unit_ready(mount_itf[pdrv], 0)){
                return 0;
            }
            return STA_NOINIT;
        }
    }
    return STA_NODISK;
}



/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/

DRESULT disk_read (
	BYTE pdrv,		/* Physical drive nmuber to identify the drive */
	BYTE *buff,		/* Data buffer to store read data */
	LBA_t sector,	/* Start sector in LBA */
	UINT count		/* Number of sectors to read */
)
{
    if(pdrv < MAX_FF){
        tusbh_interface_t* itf = mount_itf[pdrv];
        if(itf){
            int res = tusbh_msc_block_read(itf, 0, sector, count, buff);
            if(res<0){
                return RES_ERROR;
            }
            return RES_OK;
        }
    }
	return RES_PARERR;
}



/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/

#if FF_FS_READONLY == 0

DRESULT disk_write (
	BYTE pdrv,			/* Physical drive nmuber to identify the drive */
	const BYTE *buff,	/* Data to be written */
	LBA_t sector,		/* Start sector in LBA */
	UINT count			/* Number of sectors to write */
)
{
    if(pdrv < MAX_FF){
        tusbh_interface_t* itf = mount_itf[pdrv];
        if(itf){
            int res = tusbh_msc_block_write(itf, 0, sector, count, (void*)buff);
            if(res<0){
                return RES_ERROR;
            }
            return RES_OK;
        }
    }
	return RES_PARERR;
}

#endif


/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/

DRESULT disk_ioctl (
	BYTE pdrv,		/* Physical drive nmuber (0..) */
	BYTE cmd,		/* Control code */
	void *buff		/* Buffer to send/receive control data */
)
{
    if(pdrv >= MAX_FF) return RES_PARERR;
    tusbh_interface_t* interface = mount_itf[pdrv];
    if(!interface)return RES_NOTRDY;
    tusbh_msc_info_t* info = tusbh_get_info(interface, tusbh_msc_info_t);
    switch(cmd){
        case CTRL_SYNC:
            return RES_OK;
        case GET_SECTOR_COUNT:
            *(LBA_t*)buff = info->blocks[0].block_count;
            return RES_OK;
        case GET_SECTOR_SIZE:
            *(DWORD*)buff = info->blocks[0].block_size;
            return RES_OK;
        case GET_BLOCK_SIZE:
            *(DWORD*)buff = info->blocks[0].block_size;
            return RES_OK;
    }
    return RES_PARERR;
}


