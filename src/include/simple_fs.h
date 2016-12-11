#ifndef _SIMEPLFS_H
#define _SIMEPLFS_H

#include "stddef.h"


/* Hardware-specific locations */
#define SUPER_BLOCK_LOC 0x0000


/* Enum containing simple labels for data we expect to be in memory */
enum trend_data {
    STOCK_DATA = 0,
    WEATHER_DATA,
    NUM_DATA_SETS
};


#ifdef __cplusplus
extern "C" {
#endif

/*
 * FOREWARD:
 * This is a hopelesly simple, "last evening before this project is due" file system implementation.
 * The idea is that the first page of the eeprom contains a structure pointing to where in later memory
 * the data is stored. At each of those locations is another data structure information where the data begins
 * within that page and how many elements are available
 */


/* Data structure containing information about the what is stored in memory immediately following this structure */
struct local_block_t{
    uint8_t obj_size;   /* Size (in bytes) of the objects contained in each list */
    uint32_t len;       /* Number of objects in "data" */
    uint8_t data[0];    /* Data of length "len" */
} __attribute((packed));


#define SB_MAGIC 0xDEED1E55
/* FIXME: Use struct local_block_t locs[0] for run-time sizes */
struct super_block_t{
    uint32_t magic;                 /* Magic number for super_block_t */
    uint32_t lb_count;              /* Number of local_block_t in the file system */
    uint16_t locs[NUM_DATA_SETS];   /* Array of locations of local_block_t structures in memory */
} __attribute((packed));      



/*
 * Return the conents of the superblock 
 * 
 * @param sb    : Pointer to a super_block_t object to populate
 * 
 * @return      : -1 if no superblock found
 */
int sfs_get_superblock(struct super_block_t *sb);



/* 
 * Load data from memory specified by the "trend_data" enum
 * 
 * @param val       : Specified for which data to load from memory
 * @param buf       : Buffer to load data into
 * @param len       : Length of buf
 * @param memlen    : Length of data read from memory (in bytes)
 * 
 * @return      : -1 if data not available in memory or on failure
 */
int sfs_load(enum trend_data val, uint8_t *buf, unsigned int len, unsigned int *memlen);



/* 
 * Store data to memory specified by the "trend_data" enum
 * 
 * @param val       : Specified for which data to load from memory
 * @param buf       : Data source buffer
 * @param len       : Length of source buffer
 * 
 * @return      : -1 on failure
 */
int sfs_store(enum trend_data dataset, uint8_t *buf, unsigned int len);



#ifdef __cplusplus
}
#endif

#endif /* _SIMEPLFS_H */
