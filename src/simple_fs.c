#include <string.h>
#include "at24cx.h"
#include "logging.h"
#include "simple_fs.h"
#include "util.h"



/* This implementation is simple and data locations/sizes are known at compile-time */
static const struct super_block_t g_sb =   
    {
        .magic = SB_MAGIC,
        .lb_count = NUM_DATA_SETS,
        .locs = {
                    (uint16_t)(10*AT24CX_PAGE_SIZE),
                    (uint16_t)(20*AT24CX_PAGE_SIZE)
                }
    };

static const struct local_block_t g_lb =
    {
        .obj_size = sizeof(int),
        .len = 0,
    }; 



/* FIXME: Simple, no sanity checking; does not support run-time sizes */
int sfs_get_superblock(struct super_block_t *sb){
    int i;

    /* Read in magic number and block count first; magic is checked to ensure lb_count is valid data before use */
    at24cx_read((uint16_t)SUPER_BLOCK_LOC, (uint8_t *)sb, sizeof(struct super_block_t));
#ifdef SFS_DEBUG
    dlog("Search for superblock returned magic: 0x%s\r\n", itohs((uint8_t*)&sb->magic, sizeof(((struct super_block_t *)0)->magic))); 
#endif
    
    /* If the expected magic number was at the address, we assume the superblock is present */
    if(0 == memcmp(&(sb->magic), &(g_sb.magic), sizeof(((struct super_block_t *)0)->magic))){
#ifdef SFS_DEBUG
        dlog("MAGIC matched!\r\n");
#endif
        return 0;  
    }else{
#ifdef SFS_DEBUG
        dlog("Initializing superblock at location: %p\r\n", SUPER_BLOCK_LOC);
#endif

        /* Save the new superblock to memory so next boot we know data is stored in memory */
        at24cx_write((uint16_t)SUPER_BLOCK_LOC, (uint8_t *)&g_sb, sizeof(struct super_block_t));

        /* Update the appropriate pages to indicate a new, empty data set */
        for(i=0; i<g_sb.lb_count; i++){
#ifdef SFS_DEBUG
            dlog("Initializing local_block at %p\r\n", g_sb.locs[i]);
#endif
            at24cx_write((uint16_t)g_sb.locs[i], (uint8_t *)&g_lb, sizeof(struct local_block_t));
        }
        
    }
        
    return -1; 
}



int sfs_load(enum trend_data dataset, uint8_t *buf, unsigned int len, unsigned int *memlen){
    struct super_block_t sb;     
    struct local_block_t lb; 

    /* First, check if the superblock is where we expect it to be; -1 means no superblock found */
    if(1 == sfs_get_superblock(&sb))
        return -1; 

    /* If the superblock was found, read the contents of the appropriate local_block_t into memory */
    at24cx_read(sb.locs[dataset],(uint8_t*)&lb, sizeof(struct local_block_t));
    
    /* Check to ensure the data read from memory will fit into the passed buffer */
    if(lb.len > len)
        return -1;

    /* It fits; Read the contents of memory into the passed buffer */
    at24cx_read(sb.locs[dataset] +  sizeof(((struct local_block_t *)0)->obj_size) +
                                    sizeof(((struct local_block_t *)0)->len), buf, lb.len);
    
    
    /* Number of objects read in */
    *memlen = lb.len;
    return 0;
}



/* FIXME: Simple, overwrites memory locations even if they are unchanged */
int sfs_store(enum trend_data dataset, uint8_t *buf, unsigned int len){
    struct local_block_t lb = 
        { 
            .obj_size = sizeof(int), 
            .len = len 
        };

    /* First, write the data localblock */
#ifdef SFS_DEBUG
    dlog("Writing localblock at address 0x%p\r\n", g_sb.locs[dataset]);
#endif
    at24cx_write(g_sb.locs[dataset], (uint8_t*)&lb, sizeof(struct local_block_t)); 

    /* Offset past the localblock and store the contents of data */
#ifdef SFS_DEBUG
    dlog("Storing %d bytes (%d objects) of data 0x%p\r\n", len, len/lb.obj_size, g_sb.locs[dataset]+sizeof(struct local_block_t));
#endif
    at24cx_write(g_sb.locs[dataset]+sizeof(struct local_block_t), buf, len);
    
    return 0;
} 










