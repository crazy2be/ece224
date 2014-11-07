#include "fat.h"

#include <math.h>
#include "sd.h"
#include <stdio.h>
#include <string.h>

//-------------------------------------------------------------------------
uint16_t file_count = 0;
uint16_t file_number = 0;

//-------------------------------------------------------------------------
//MASTER BOOT RECORD
// TODO: Make these into nice structs.
// uint8_t MBR_Bootable;
// uint8_t MBR_Start_Sector[3];
// uint8_t MBR_End_End[3];
// uint8_t MBR_Partition_Type;
uint32_t MBR_BS_Location = -1;
uint32_t MBR_Partition_Len = -1;

//FAT12, FAT16, FAT32
uint8_t BS_JmpBoot[3];
uint8_t BS_OEMName[8];
uint16_t BPB_BytsPerSec;
uint8_t BPB_SecPerClus;
uint16_t BPB_RsvdSecCnt;
uint8_t BPB_NumFATs;
uint16_t BPB_RootEntCnt;
uint16_t BPB_TotSec16;
uint8_t BPB_Media;
uint16_t BPB_FATSz16;
uint16_t BPB_SecPerTrk;
uint16_t BPB_NumHeads;
uint32_t BPB_HiddSec;
uint32_t BPB_TotSec32;

//FAT12, FAT16
uint8_t BS_DrvNum_16;
uint8_t BS_BootSig_16;
uint32_t BS_VOLID_16;
uint8_t BS_VOLLab_16[11];
uint8_t BS_FilSysType_16[8];

//FAT32
uint32_t BPB_FATSz32;
uint16_t BPB_ExtFlags;
uint16_t BPB_FSVer;
uint32_t BPB_RootClus;
uint16_t BPB_FSInfo;
uint16_t BPB_BkBootSec;
uint8_t BS_DrvNum_32;
uint8_t BS_BootSig_32;
uint32_t BS_VOLID_32;
uint8_t BS_VOLLab_32[11];
uint8_t BS_FilSysType_32[8];

//Calculated Values
uint16_t RootDirSectors;
uint32_t FATSz;
uint32_t FirstDataSector;
uint32_t TotSec;
uint32_t DataSec;
uint32_t CountofClusters;
uint32_t FirstRootDirSecNum;
char FATType[6];
uint32_t FATOffset;
uint32_t ThisFATSecNum;
uint32_t ThisFATEntOffset;
uint16_t FAT12ClusEntryVal;
uint16_t FAT16ClusEntryVal;
uint32_t FAT32ClusEntryVal;
uint32_t FATClusEntryVal;

static uint16_t read_uint16(uint8_t *buf) {
	return buf[0] + (buf[1] << 8);
}
static uint32_t read_uint32(uint8_t *buf) {
	return buf[0] + (buf[1] << 8) + (buf[2] << 16) + (buf[3] << 24);
}

// Initialize the Master Boot Record
uint8_t init_mbr(void) {
	uint8_t buf[512] = { 0 };

	SD_read_lba(buf, 0, 1); // Store master boot record in buf

	// Check the last 2 bytes of the buffer to ensure it is tagged as the MBR
	// (magic bytes)
	if (buf[510] != 0x55 || buf[511] != 0xAA) {
		return -1;
	}

// 	MBR_Bootable = buf[446];
// 	MBR_Start_Sector[0] = buf[447];
// 	MBR_Start_Sector[1] = buf[448];
// 	MBR_Start_Sector[2] = buf[449];
// 	MBR_Partition_Type = buf[450];
// 	MBR_End_End[0] = buf[451];
// 	MBR_End_End[1] = buf[452];
// 	MBR_End_End[2] = buf[453];

	// Read boot sector and partition length for first partition. Other
	// partitions are ignored.
	MBR_BS_Location = read_uint32(&buf[454]);
	MBR_Partition_Len = read_uint32(&buf[458]);
	return 0;
}
//-------------------------------------------------------------------------
// Initialize the Boot Sector
uint8_t init_bs(void) {
	uint8_t buf[512] = { 0 };

	SD_read_lba(buf, MBR_BS_Location, 1); //Store boot sector in buf

	if (buf[510] != 0x55 || buf[511] != 0xAA) {
		return -1;
	}

	BS_JmpBoot[0]  = buf[0];
	BS_JmpBoot[1]  = buf[1];
	BS_JmpBoot[2]  = buf[2];
	BS_OEMName[0]  = buf[3];
	BS_OEMName[1]  = buf[4];
	BS_OEMName[2]  = buf[5];
	BS_OEMName[3]  = buf[6];
	BS_OEMName[4]  = buf[7];
	BS_OEMName[5]  = buf[8];
	BS_OEMName[6]  = buf[9];
	BS_OEMName[7]  = buf[10];
	BPB_BytsPerSec = read_uint16(&buf[11]);
	BPB_SecPerClus = buf[13];
	BPB_RsvdSecCnt = read_uint16(&buf[14]);
	BPB_NumFATs    = buf[16];
	BPB_RootEntCnt = read_uint16(&buf[17]);
	BPB_TotSec16   = read_uint16(&buf[19]);
	BPB_Media      = buf[21];
	BPB_FATSz16    = read_uint16(&buf[22]);
	BPB_SecPerTrk  = read_uint16(&buf[24]);
	BPB_NumHeads   = read_uint16(&buf[26]);
	BPB_HiddSec    = read_uint32(&buf[28]);
	BPB_TotSec32   = read_uint32(&buf[32]);

	//count of sectors occupied by the root directory
	RootDirSectors = ((BPB_RootEntCnt * 32) + (BPB_BytsPerSec - 1)) / BPB_BytsPerSec;

	if (BPB_FATSz16 != 0) {
		FATSz = BPB_FATSz16;

		BS_DrvNum_16        = buf[36];
		BS_BootSig_16       = buf[38];
		BS_VOLID_16         = read_uint32(&buf[39]);
		BS_VOLLab_16[0]     = buf[43];
		BS_VOLLab_16[1]     = buf[44];
		BS_VOLLab_16[2]     = buf[45];
		BS_VOLLab_16[3]     = buf[46];
		BS_VOLLab_16[4]     = buf[47];
		BS_VOLLab_16[5]     = buf[48];
		BS_VOLLab_16[6]     = buf[49];
		BS_VOLLab_16[7]     = buf[50];
		BS_VOLLab_16[8]     = buf[51];
		BS_VOLLab_16[9]     = buf[52];
		BS_VOLLab_16[10]    = buf[53];
		BS_FilSysType_16[0] = buf[54];
		BS_FilSysType_16[1] = buf[55];
		BS_FilSysType_16[2] = buf[56];
		BS_FilSysType_16[3] = buf[57];
		BS_FilSysType_16[4] = buf[58];
		BS_FilSysType_16[5] = buf[59];
		BS_FilSysType_16[6] = buf[60];
		BS_FilSysType_16[6] = buf[61];

		//root directory LBA = FirstRootDirSecNum + MBR_BS_Location
		FirstRootDirSecNum = BPB_RsvdSecCnt + (BPB_NumFATs * BPB_FATSz16);
		FirstDataSector = BPB_RsvdSecCnt + (BPB_NumFATs * BPB_FATSz16) + RootDirSectors;
	} else {
		FATSz = BPB_FATSz32;

		BPB_FATSz32         = read_uint32(&buf[36]);
		BPB_ExtFlags        = read_uint16(&buf[40]);
		BPB_FSVer           = read_uint16(&buf[42]);
		BPB_RootClus        = read_uint32(&buf[44]);
		BPB_FSInfo          = read_uint16(&buf[48]);
		BPB_BkBootSec       = read_uint16(&buf[50]);
		BS_DrvNum_32        = buf[64];
		BS_BootSig_32       = buf[66];
		BS_VOLID_32         = read_uint32(&buf[67]);
		BS_VOLLab_32[0]     = buf[71];
		BS_VOLLab_32[1]     = buf[72];
		BS_VOLLab_32[2]     = buf[73];
		BS_VOLLab_32[3]     = buf[74];
		BS_VOLLab_32[4]     = buf[75];
		BS_VOLLab_32[5]     = buf[76];
		BS_VOLLab_32[6]     = buf[77];
		BS_VOLLab_32[7]     = buf[78];
		BS_VOLLab_32[8]     = buf[79];
		BS_VOLLab_32[9]     = buf[80];
		BS_VOLLab_32[10]    = buf[81];
		BS_FilSysType_32[0] = buf[82];
		BS_FilSysType_32[1] = buf[83];
		BS_FilSysType_32[2] = buf[84];
		BS_FilSysType_32[3] = buf[85];
		BS_FilSysType_32[4] = buf[86];
		BS_FilSysType_32[5] = buf[87];
		BS_FilSysType_32[6] = buf[88];
		BS_FilSysType_32[7] = buf[89];

		//root directory LBA = FirstRootDirSecNum + MBR_BS_Location
		FirstRootDirSecNum = BPB_RsvdSecCnt + (BPB_NumFATs * BPB_FATSz32);
		FirstDataSector = BPB_RsvdSecCnt + (BPB_NumFATs * BPB_FATSz32)
				+ RootDirSectors;
	}
	//The start of the data region, the first sector of cluster 2


	if (BPB_TotSec16 != 0) {
		TotSec = BPB_TotSec16;
	} else {
		TotSec = BPB_TotSec32;
	}
	DataSec = TotSec - (BPB_RsvdSecCnt + (BPB_NumFATs * FATSz) + RootDirSectors);

	CountofClusters = DataSec / BPB_SecPerClus;
	if (CountofClusters < 4085) {
		strcpy(FATType, "FAT12");
	} else if (CountofClusters < 65525) {
		strcpy(FATType, "FAT16");
	} else {
		strcpy(FATType, "FAT32");
	}
	// printf("\nFile System: %s",FATType);
	return 0;
}
//-------------------------------------------------------------------------
// Prints Boot Sector information
void info_bs() {
	//Stored in Boot Sector
	printf("\n\nBoot Sector Data Structure Summary:");
	printf("\nBS_JmpBoot: 0x%02X 0x%02X 0x%02X", BS_JmpBoot[0], BS_JmpBoot[1],
			BS_JmpBoot[2]);
	printf("\nBS_OEMName: %s", BS_OEMName);
	printf("\nBPB_BytsPerSec: %d", BPB_BytsPerSec);
	printf("\nBPB_SecPerClus: %d", BPB_SecPerClus);
	printf("\nBPB_RsvdSecCnt: %d", BPB_RsvdSecCnt);
	printf("\nBPB_NumFATs: %d", BPB_NumFATs);
	printf("\nBPB_RootEntCnt: %d", BPB_RootEntCnt);
	printf("\nBPB_TotSec16: %d", BPB_TotSec16);
	printf("\nBPB_Media: %d", BPB_Media);
	printf("\nBPB_FATSz16: %d", BPB_FATSz16);
	printf("\nBPB_SecPerTrk: %d", BPB_SecPerTrk);
	printf("\nBPB_NumHeads: %d", BPB_NumHeads);
	printf("\nBPB_HiddSec: %ud", BPB_HiddSec);
	printf("\nBPB_TotSec32: %ud", BPB_TotSec32);

	//Calculated based on Boot Sector Values
	printf("\nRootDirSectors: 0x%04X (%d)10", RootDirSectors, RootDirSectors);
	printf("\nFATSz: 0x%04X (%d)10", FATSz, FATSz);
	printf("\nFirstDataSector: 0x%04X (%d)10", FirstDataSector, FirstDataSector);
	printf("\nTotSec: 0x%04X (%d)10", TotSec, TotSec);
	printf("\nDataSec: 0x%04X (%d)10", DataSec, DataSec);
	printf("\nCountofClusters: 0x%04X (%d)10", CountofClusters, CountofClusters);
	printf("\nFirstRootDirSecNum: 0x%04X (%d)10", FirstRootDirSecNum,
			FirstRootDirSecNum);
}
//-------------------------------------------------------------------------
// Calculates the First Sector of Cluster number 'N'
uint32_t FirstSectorofCluster(uint32_t N) {
	return (((N - 2) * BPB_SecPerClus) + FirstDataSector + MBR_BS_Location);
}

//-------------------------------------------------------------------------
// Calculates the Next Cluster after cluster 'N'
// Finds the cluster 'N' in the File Allocation Table
// Cluster 'N's data contains the Next Cluster number 
void CalcFATSecAndOffset(uint32_t N) {
	uint8_t buf[512] = { 0 };
	//Calculate The absolute FATOffset based on which File System is in use
	//Difference between FAT12, FAT16, FAT32 is based only on CountofClusters
	if (CountofClusters < 4085) {
		//FAT12
		// Multiply by 1.5 without using floating point, the divide by 2 rounds DOWN
		FATOffset = N + (N / 2);
	} else if (CountofClusters < 65525) {
		//FAT16
		FATOffset = N * 2;
	} else {
		//FAT32
		FATOffset = N * 4;
	}

	//FAT Sector
	ThisFATSecNum = BPB_RsvdSecCnt + (FATOffset / BPB_BytsPerSec);
	//FAT Offset
	ThisFATEntOffset = FATOffset % BPB_BytsPerSec;

	//Store the FAT Sector into buf[512]
	SD_read_lba(buf, MBR_BS_Location + ThisFATSecNum, 1);

	//FATClusEntryVal is the next cluster for cluster 'N'
	if (CountofClusters < 4085) {
		//FAT12
		if (ThisFATEntOffset != 511) {
			if (N & 0x0001) {
				// Cluster number is ODD
				FAT12ClusEntryVal = (((buf[ThisFATEntOffset] & 0xF0)
						| (buf[ThisFATEntOffset + 1] << 8)) >> 4);
			} else {
				// Cluster number is EVEN
				FAT12ClusEntryVal = ((buf[ThisFATEntOffset]
						| (buf[ThisFATEntOffset + 1] << 8)) & 0x0FFF);
			}
		} else {
			FAT12ClusEntryVal = (buf[511] & 0xFF);
			SD_read_lba(buf, MBR_BS_Location + ThisFATSecNum + 1, 1);
			FAT12ClusEntryVal = (FAT12ClusEntryVal | ((buf[0] & 0x0F) << 8));
		}
		FATClusEntryVal = FAT12ClusEntryVal;
	} else if (CountofClusters < 65525) {
		//FAT16
		FAT16ClusEntryVal = (buf[ThisFATEntOffset]
				| (buf[ThisFATEntOffset + 1] << 8));
		FATClusEntryVal = FAT16ClusEntryVal;
	} else {
		//FAT32
		FAT32ClusEntryVal = (buf[ThisFATEntOffset]
				| (buf[ThisFATEntOffset + 1] << 8)
				| (buf[ThisFATEntOffset + 2] << 16)
				| (buf[ThisFATEntOffset + 3] << 24)) & 0x0FFFFFFF;
		FATClusEntryVal = FAT32ClusEntryVal;
	}

}

//-------------------------------------------------------------------------
// Determines if the Cluster is the last cluster in the file's cluster chain
// Returns 1 if FATContent is the last cluster in the cluster chain
// Returns 0 if there are more clusters
uint8_t isEOF(uint32_t FATContent) {
	if (CountofClusters < 4085) return FATContent >= 0x0FF8; // FAT12
	else if (CountofClusters < 65525) return FATContent >= 0xFFF8; // FAT16
	else return FATContent >= 0x0FFFFFF8; // FAT32
}
//-------------------------------------------------------------------------
// Buffers the cluster chain of a file so that it can be streamed
void build_cluster_chain(int cc[], uint32_t length, data_file *df) {
	cc[0] = df->FirstCluster;

	// TODO: this uses global variables in a frankly disgusting way
	//       it should be refactored as soon as possible
	CalcFATSecAndOffset(df->FirstCluster);

	for (int i = 1; i < length; i++) {
		cc[i] = FATClusEntryVal;
		if (!isEOF(FATClusEntryVal)) {
			CalcFATSecAndOffset(FATClusEntryVal);
		}
	}
}
//-------------------------------------------------------------------------
// Searches for a particular file extension specified by "extension"
// To browse from the start of the file system use
// search_for_filetye("extension",0,1);
uint32_t search_for_filetype(char *extension, data_file *df, int sub_directory,
		int search_root) {
	uint16_t directory;
	uint8_t buf[512] = { 0 };
	uint8_t attribute_offset = 11; //first attribute offset
	uint8_t FstClusHi_offset = 20; //first cluster high offset
	uint8_t FstClusLo_offset = 26; //first cluster offset
	uint8_t FileSize_offset = 28; //file size offset
	uint8_t attribute;
	uint8_t entry_num = 0;
	char filename[12];
	char fileext[4];
	char longname[255] = { 0 };
	int root_sector_count = 0, ATTR_LONG_NAME, ATTR_LONG_NAME_MASK;

	if (search_root) {
		//Search the root directory
		directory = (MBR_BS_Location + FirstRootDirSecNum);
		SD_read_lba(buf, directory, 1);
	} else {
		//Search the sub directory
		SD_read_lba(buf, sub_directory, 1);
		directory = sub_directory;
		// start from entry number 2 to skip over the
		// ./ Current directory Entry and the ../ Parent directory entry
		entry_num = 2;
	}

	//Browse while there are still entries to browse
	while ((buf[entry_num * 32] != 0x00)) {
		ATTR_LONG_NAME_MASK = buf[entry_num * 32 + attribute_offset] & 0x3F;
		ATTR_LONG_NAME = buf[entry_num * 32 + attribute_offset] & 0x0F;

		//Determine if the entry contains a long file name
		if (((buf[entry_num * 32 + attribute_offset] & ATTR_LONG_NAME_MASK)
				== ATTR_LONG_NAME)
				&& (buf[entry_num * 32 + attribute_offset] != 0x08)
				&& (buf[entry_num * 32] != 0xE5)) //long filename
		{
			//longname_blocks is the amount of entrys that contain the long filename
			int longname_blocks = (buf[entry_num * 32] & 0xBF);
			if (longname_blocks < 20) {
				longname[(longname_blocks - 1) * 13 + 13] = '\0';
			}

			//read the file name from the buffer and store it into longname[]
			while (longname_blocks > 0) {
				longname[(longname_blocks - 1)*13 + 0] = buf[entry_num*32 + 1];
				longname[(longname_blocks - 1)*13 + 1] = buf[entry_num*32 + 3];
				longname[(longname_blocks - 1)*13 + 2] = buf[entry_num*32 + 5];
				longname[(longname_blocks - 1)*13 + 3] = buf[entry_num*32 + 7];
				longname[(longname_blocks - 1)*13 + 4] = buf[entry_num*32 + 9];
				longname[(longname_blocks - 1)*13 + 5] = buf[entry_num*32 + 14];
				longname[(longname_blocks - 1)*13 + 6] = buf[entry_num*32 + 16];
				longname[(longname_blocks - 1)*13 + 7] = buf[entry_num*32 + 18];
				longname[(longname_blocks - 1)*13 + 8] = buf[entry_num*32 + 20];
				longname[(longname_blocks - 1)*13 + 9] = buf[entry_num*32 + 22];
				longname[(longname_blocks - 1)*13 + 10] = buf[entry_num*32 + 24];
				longname[(longname_blocks - 1)*13 + 11] = buf[entry_num*32 + 28];
				longname[(longname_blocks - 1)*13 + 12] = buf[entry_num*32 + 30];

				longname_blocks--;
				entry_num++;

				//if the entry number spans beyond the current sector, grab the next one
				if (entry_num*32 >= BPB_BytsPerSec) {
					root_sector_count++;
					SD_read_lba(buf, directory + root_sector_count, 1);
					entry_num = 0;
				}
			}
		}

		//Read the attribute tag of the current entry
		//0x00 Indicates the end of the FAT entries
		//0x08 Indicates Volume ID
		//0xE5 Indicates and empty entry
		//0x10 Indicates a Directory
		//anything else indicates a file

		attribute = buf[entry_num * 32 + attribute_offset];

		if ((attribute & 0x08) || (buf[entry_num * 32] == 0xE5)) {
			//0x08 Indicates Volume ID
			//0xE5 Indicates and empty entry
			//Either case increment to next entry
		} else if (attribute & 0x10) {
			//Indicates a Directory, search the directory
			sub_directory = (buf[entry_num * 32 + FstClusLo_offset])
					+ (buf[entry_num * 32 + FstClusLo_offset + 1] << 8)
					+ (buf[entry_num * 32 + FstClusHi_offset])
					+ (buf[entry_num * 32 + FstClusHi_offset + 1] << 8);
			sub_directory = FirstSectorofCluster(sub_directory);
			if (!search_for_filetype(extension, df, sub_directory, 0)) {
				return 0;
			}
		} else {
			//Indicates a file
			for (int i = 0; i < 11; i++) {
				filename[i] = buf[entry_num * 32 + i];
			}
			filename[11] = '\0';

			//Grab the current entry numbers file extension
			fileext[0] = buf[entry_num * 32 + 8];
			fileext[1] = buf[entry_num * 32 + 9];
			fileext[2] = buf[entry_num * 32 + 10];
			fileext[3] = '\0';

			//printf("found file: %s.%s\n", filename, fileext);

			//compare the current file's file extension to the extension to search for
			if (!strcmp(extension, fileext)) {
				if (file_count == file_number) {
					strcpy(df->Name, filename);
					df->Attr = attribute;
					df->FirstCluster = (buf[entry_num * 32 + FstClusLo_offset])
							+ (buf[entry_num * 32 + FstClusLo_offset + 1]
									<< 8) + (buf[entry_num * 32
							+ FstClusHi_offset]) + (buf[entry_num * 32
							+ FstClusHi_offset + 1] << 8);
					df->FileSize = (buf[entry_num * 32 + FileSize_offset])
							| (buf[entry_num * 32 + FileSize_offset + 1]
									<< 8) | (buf[entry_num * 32
							+ FileSize_offset + 2] << 16) | (buf[entry_num
							* 32 + FileSize_offset + 3] << 24);
					df->FirstSector = FirstSectorofCluster(df->FirstCluster);
					df->Posn = 0;
					file_count = 0;
					file_number = file_number + 1;
					return 0; //file found
				}
				file_count++;
			}
		}
		entry_num++;
		//if the entry number spans beyond the current sector, grab the next one
		if (entry_num * 32 >= BPB_BytsPerSec) {
			root_sector_count++;
			SD_read_lba(buf, directory + root_sector_count, 1);
			entry_num = 0;
		}
	}

	//The buf[entry_num*32] is 0x00
	if (search_root) {
		//The entire volume has been searched
		if (file_number == 0) {
			//No files matching the file extension have been found
			printf("\nFile Extension %s not found", extension);
		} else {
			//Wrap around and find the first file
			file_number = 0;
			file_count = 0;
			search_for_filetype(extension, df, 0, 1);
		}
	} else {
		//The subdirectory doesn't contain any more entries
		return 1;//entry not found
	}
	return 0;
}

static inline uint32_t ceil_div(uint32_t a, uint32_t b) {
	// doesn't handle overflow
	return (a + b - 1) / b;
}


inline uint32_t get_file_cluster_count(data_file *df) {
	return ceil_div(df->FileSize, BPB_BytsPerSec * BPB_SecPerClus);
}

inline uint32_t get_file_sector_count(data_file *df) {
	return ceil_div(df->FileSize, BPB_SecPerClus);
}
