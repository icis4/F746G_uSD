/**
 ******************************************************************************
 * @file   fatfs.c
 * @brief  Code for fatfs applications
 *
 ******************************************************************************
 */

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <malloc.h>
#include "fatfs.h"

// http://elm-chan.org/fsw/ff/doc/sfileinfo.html
// fdate    - The date when the file was modified or the directory was created.
// bit15:9  - Year origin from 1980 (0..127)
// bit8:5   - Month (1..12)
// bit4:0   - Day (1..31)
// ftime    - The time when the file was modified or the directory was created.
// bit15:11 - Hour (0..23)
// bit10:5  - Minute (0..59)
// bit4:0   - Second / 2 (0..29)

typedef union FAT_Date_Time_Type {
	DWORD ftime;
	struct {
		WORD time;
		WORD date;
	};
	struct {
		unsigned int Seconds:5;
		unsigned int Minutes:6;
		unsigned int Hours:5;
		unsigned int Day:5;
		unsigned int Month:4;
		unsigned int Year:7;
	};
} FAT_Date_Time_Type;


/* defined in FATFS/App/fatfs.c */
//uint8_t retSD;    /* Return value for SD */
//char SDPath[4];   /* SD logical drive path */
//FATFS SDFatFS;    /* File system object for SD logical drive */
//FIL SDFile;       /* File object for SD */
void MX_FATFS_Init(void)
{

	/*## FatFS: Link the SD driver ###########################*/
	retSD = FATFS_LinkDriver(&SD_Driver, SDPath);

	/* FR_NO_FILESYSTEM Bug workaround*/
	FATFS *fs;
	fs = malloc(sizeof(FATFS));
	f_mount(fs, "", 0); /* Mount default drive */
	f_mount(NULL, "", 0); /* Unmount default drive */
	free(fs);
	/* FR_NO_FILESYSTEM Bug */

	if (retSD == FR_OK) {
		retSD = f_mount(&SDFatFS, SDPath, 0);
	}
}

/**
 * @brief  Gets Time from RTC
 * @param  None
 * @retval Time in DWORD
 */
DWORD get_fattime(void)
{
	return 0;
}

int diskfree(char *path, DWORD *total_kb, DWORD *free_kb) {
	FATFS *fs;
	DWORD fre_clust, fre_sect, tot_sect;
	FRESULT result;

	/* Get volume information and free clusters of drive 0:/ */
//	if (strcmp(path, SRAMDISKPath) == 0 || strncasecmp(path, "ram:", 4) == 0) {
//		fs = &SRAMDISKFatFS;
//		result = f_getfree(SRAMDISKPath, &fre_clust, &fs);
//
//		/* Get total sectors and free sectors */
//		tot_sect = (fs->n_fatent - 2) * fs->csize;
//		fre_sect = fre_clust * fs->csize;
//
//		/* Print the free space (assuming 512 bytes/sector) */
//		*total_kb = tot_sect / 2;
//		*free_kb = fre_sect / 2;
//
//		/* Get volume information and free clusters of drive 1:/ */
//	} else if (strcmp(path, USERPath) == 0
//			|| strncasecmp(path, "flash:", 6) == 0) {
//		fs = &USERFatFS;
//		result = f_getfree(USERPath, &fre_clust, &fs);
//
//		/* Get total sectors and free sectors */
//		tot_sect = (fs->n_fatent - 2) * fs->csize;
//		fre_sect = fre_clust * fs->csize;
//
//		/* Print the free space (assuming 4096 bytes/sector) */
//		*total_kb = tot_sect * 4;
//		*free_kb = fre_sect * 4;
//
//		/* Get volume information and free clusters of drive 2:/ */
//	} else
//
		if (strcmp(path, SDPath) == 0
			|| strncasecmp(path, "sd:", 3) == 0) {
		fs = &SDFatFS;
		result = f_getfree(SDPath, &fre_clust, &fs);

		/* Get total sectors and free sectors */
		tot_sect = (fs->n_fatent - 2) * fs->csize;
		fre_sect = fre_clust * fs->csize;

		/* Print the free space (assuming 512 bytes/sector) */
		*total_kb = tot_sect / 2;
		*free_kb = fre_sect / 2;

	} else {
		*total_kb = -1;
		*free_kb = -1;
		errno = __ELASTERROR + FR_EXIST;
		return -1;
	}

	if (result != FR_OK) {
		errno = __ELASTERROR + result;
		return -1;
	}

	return 0;
}

FRESULT scan_files(FILE* fout, char* path) {
	FRESULT res;
	DIR dir;
	static FILINFO fno;
	FAT_Date_Time_Type dt;

	res = f_opendir(&dir, path); /* Open the directory */
	if (res == FR_OK) {
		// fprintf(fout, "\nDIR %s\n", path);
		for (;;) {
			res = f_readdir(&dir, &fno); /* Read a directory item */
			if (res != FR_OK || fno.fname[0] == 0)
				break; /* Break on error or end of dir */
			dt.date = fno.fdate;
			dt.time = fno.ftime;
			if (fno.fattrib & AM_DIR) { /* It is a directory */
				fprintf(fout,
						"%-16s <DIR> %04d-%02d-%02d %02d:%02d:%02d\n", fno.fname,
						dt.Year, dt.Month, dt.Day, dt.Hours, dt.Minutes, dt.Seconds);
			} else { /* It is a file. */
				fprintf(fout, "%-16s %ld%s %04d-%02d-%02d %02d:%02d:%02d\n", fno.fname,
						(fno.fsize > 5000) ? fno.fsize / 1024 : fno.fsize,
						(fno.fsize > 5000) ? "KB" : "B",
						dt.Year, dt.Month, dt.Day, dt.Hours, dt.Minutes, dt.Seconds);
			}
		}
		f_closedir(&dir);
		fputs("", fout);
	}

	return res;
}

