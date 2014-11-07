#pragma once

#include <inttypes.h>

//-------------------------------------------------------------------------
// data_file Data Structure
typedef struct {
	uint8_t  Name[11];     // File name
	uint8_t  Attr;         // File attribute tag
	uint32_t FirstCluster; // First cluster of file data
	uint32_t FileSize;     // Size of the file in bytes
	uint32_t FirstSector;  // First sector of file data
	uint32_t Posn;         // (???) FilePtr byte absolute to the file
} data_file;

// TODO: Remove these that aren't used.
uint32_t FirstSectorofCluster(uint32_t);
uint8_t init_mbr(void);
uint8_t init_bs(void);
void info_bs();
uint32_t FirstSectorofCluster(uint32_t N);
void CalcFATSecAndOffset(uint32_t);
uint8_t isEOF(uint32_t);
void build_cluster_chain(int cc[], uint32_t length, data_file *df);
uint32_t search_for_filetype(uint8_t *extension, data_file *df, int sub_directory,
		int search_root);