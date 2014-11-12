#include "fat.h"

#include <math.h>
#include "sd.h"
#include <stdio.h>
#include <string.h>
#include <assert.h>

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
uint32_t FATClusEntryVal;


//http://stackoverflow.com/questions/17944/how-to-round-up-the-result-of-integer-division
static inline uint32_t ceil_div(uint32_t a, uint32_t b) {
	// doesn't handle overflow, positive numbers only please
	return (a + b - 1) / b;
}
static uint16_t read_uint16(uint8_t *buf) {
	return buf[0] + (buf[1] << 8);
}
static uint32_t read_uint32(uint8_t *buf) {
	return buf[0] + (buf[1] << 8) + (buf[2] << 16) + (buf[3] << 24);
}
static void copy(uint8_t *dst, uint8_t *src, int len) {
	for (int i = 0; i < len; i++) {
		dst[i] = src[i];
	}
}

typedef enum {
	TypeINVALID = 0,
	TypeFAT12 = 1,
	TypeFAT16 = 2,
	TypeFAT32 = 3,
} FilesystemType;
static const char *filesystem_type_to_string(FilesystemType type) {
	static const char strs[][6] = {"???", "FAT12", "FAT16", "FAT32"};
	if (type > TypeFAT32) type = TypeINVALID;
	return strs[(int)type];
}
static FilesystemType filesystem_type(uint32_t CountofClusters) {
	if (CountofClusters < 4085) return TypeFAT12;
	else if (CountofClusters < 65525) return TypeFAT16;
	else return TypeFAT32;
}


// https://en.wikipedia.org/wiki/Master_boot_record
typedef struct __attribute__((packed)) {
	uint8_t BootableCode[446];
	struct {
		uint8_t Status;
		uint8_t CHSFirstAddress[3];
		uint8_t PartitionType;
		uint8_t CHSLastAddress[3];
		uint32_t LBAFirstAddress;
		uint32_t LBALength;
	} Partitions[4];
	uint8_t Magic1; // Must be 0x55
	uint8_t Magic2; // Must be 0xAA
} MasterBootRecord;
_Static_assert(sizeof(MasterBootRecord) == 512, "Blocks are 512 bytes!");

int read_mbr(MasterBootRecord *mbr) {
	SD_read_lba((uint8_t*)mbr, 0);
	if (mbr->Magic1 != 0x55 || mbr->Magic2 != 0xAA) {
		return -1;
	}

	MBR_BS_Location = mbr->Partitions[0].LBAFirstAddress;
	MBR_Partition_Len = mbr->Partitions[0].LBALength;
	return 0;
}


// https://en.wikipedia.org/wiki/Design_of_the_FAT_file_system#Layout
// FAT: File Allocation Table. Space at the beginning of the filesystem to
// store a singly linked list of all the entries in a file, starting at a
// given sector.
//////////////////////////////////////////////////////////////////////////////
//  Boot  / FAT32 Info /  Reserved  / FAT /   FAT #2    /   Root    /  Data  /
// Sector /   Sector   / Sector(0+) /  #1 / (sometimes) / Directory / Region /
//////////////////////////////////////////////////////////////////////////////
#define packed __attribute__((packed))
typedef struct packed {
	struct packed {
		uint8_t  JumpInstruction[3];
		uint8_t  OEMName[8];

		// DOS 2.0 BPB Entries
		uint16_t BytesPerSector;
		uint8_t  SectorsPerCluster;
		uint16_t ReservedSectorCount;
		uint8_t  NumFATs;
		// Maximum number of root directory entries. These are stored
		// immediately following the last FAT. This is zero in FAT32.
		// (see fat32.RootCluster in that case)
		uint16_t RootEntryCount;
		uint16_t TotalSectors16;
		uint8_t  MediaDescriptor;
		uint16_t SectorsPerFAT;

		// DOS 3.3.1 BPB Entries
		uint16_t SectorsPerTrack;
		uint16_t NumHeads;
		uint32_t HiddenSectorCount;
		uint32_t TotalSectors32;
	} fat12;
	union packed {
		struct packed {
			uint8_t  DriveNum;
			uint8_t  Reserved;
			uint8_t  ExtendedBootSignature;
			uint32_t VolumeID;
			uint8_t  VolumeLabel[11];
			uint8_t  FilesystemType[8];
		} fat16;
		struct packed {
			uint32_t SectorsPerFAT;
			uint16_t MirroringFlags;
			uint16_t Version;
			uint32_t RootCluster;
			uint16_t FSInfoSector;
			uint16_t BackupBootSector;
			// Wikipedia says there should be 12 bytes of reserved space
			// here, but the original code didn't think there was. I'm
			// trusting Wikipedia, but we may want to verify.
			uint8_t  Reserved2[12];
			uint8_t  DriveNum;
			uint8_t  Reserved;
			uint8_t  ExtendedBootSignature;
			uint32_t VolumeID;
			uint8_t  VolumeLabel[11];
			uint8_t  FilesystemType[8];
		} fat32;
	};
	uint8_t BootableCode[419];
	uint8_t DriveNum; // From FAT12/FAT16
	uint8_t Magic1; // Must be 0x55
	uint8_t Magic2; // Must be 0xAA
} BootSector;
_Static_assert(sizeof(BootSector) == 512, "Blocks are 512 bytes!");

void print_boot_sector(BootSector *bs) {
	printf("OEMName: %.*s\n", sizeof(bs->fat12.OEMName), bs->fat12.OEMName);
	printf("SectorsPerCluster: %d\n", bs->fat12.SectorsPerCluster);
	printf("ReservedSectors: %d\n", bs->fat12.ReservedSectorCount);
	printf("SectorsPerFAT: %d\n", bs->fat32.SectorsPerFAT);
	printf("RootCluster: %d\n", bs->fat32.RootCluster);
	printf("VolumeLabel: %.*s\n", sizeof(bs->fat32.VolumeLabel), bs->fat32.VolumeLabel);
	printf("FilesystemType: %.*s\n", sizeof(bs->fat32.FilesystemType), bs->fat32.FilesystemType);
	printf("Magic: %x %x\n", bs->Magic1, bs->Magic2);
}
int read_boot_sector(BootSector *bs, int lba) {
	SD_read_lba((uint8_t*)bs, lba);
	print_boot_sector(bs);
}

typedef struct {
	FilesystemType Type;
	uint32_t       NumSectors;
	uint16_t       BytesPerSector;
	uint8_t        SectorsPerCluster;

	// RootCluster should be used for FAT32, RootDirectorySectors for FAT12/16.
	uint32_t RootCluster;
	uint32_t RootDirectoryStartSector;
	uint32_t RootDirectoryNumSectors;

	uint32_t DataStartSector;
	uint32_t DataNumClusters;
} FSInfo;
int calc_fs_info(FSInfo *info, const BootSector *bs) {
	uint32_t num_sectors = bs->fat12.TotalSectors16 ?
			bs->fat12.TotalSectors16 : bs->fat12.TotalSectors32;
	uint32_t sectors_per_fat = bs->fat12.SectorsPerFAT ?
			bs->fat12.SectorsPerFAT : bs->fat32.SectorsPerFAT;

	// According to the FAT spec, we shouldn't have to ceil divide here, but
	// we might as well just to be safe.
	uint32_t root_dir_start = bs->fat12.ReservedSectorCount
									+ bs->fat12.NumFATs*sectors_per_fat;
	uint32_t root_dir_sectors = ceil_div(bs->fat12.RootEntryCount * 32,
										 bs->fat12.BytesPerSector);


	uint32_t num_data_sectors = num_sectors - (bs->fat12.ReservedSectorCount
									+ bs->fat12.NumFATs*sectors_per_fat
									+ root_dir_sectors);
	uint32_t num_data_clusters = num_data_sectors / bs->fat12.SectorsPerCluster;

	FilesystemType tp = filesystem_type(num_data_clusters);
	*info = (FSInfo) {
		.Type = tp,
		.NumSectors = num_sectors,
		.BytesPerSector = bs->fat12.BytesPerSector,
		.SectorsPerCluster = bs->fat12.SectorsPerCluster,

		.RootCluster = tp == TypeFAT32 ? bs->fat32.RootCluster : -1,
		.RootDirectoryStartSector = tp == TypeFAT32 ? -1 : root_dir_start,
		.RootDirectoryNumSectors = tp = TypeFAT32 ? 0 : root_dir_sectors,

		.DataStartSector = root_dir_start + root_dir_sectors,
		.DataNumClusters = num_data_clusters,
	};
	return 0;
}

uint8_t read_boot_sector_original(void);
// Todo: Probably just s_fs_info should be global.
static MasterBootRecord s_mbr;
static BootSector s_boot_sector;
static FSInfo s_fs_info;
int fat_init(void) {
	int status = 0;
	if ((status = read_mbr(&s_mbr)) < 0) {
		return status;
	}
	uint32_t partition_start = s_mbr.Partitions[0].LBAFirstAddress;
	if ((status = read_boot_sector(&s_boot_sector, partition_start)) < 0) {
		return status;
	}
	if ((status = calc_fs_info(&s_fs_info, &s_boot_sector)) < 0) {
		return status;
	}
	printf("Type: %s (%d)\n", filesystem_type_to_string(s_fs_info.Type), s_fs_info.Type);
	printf("Num sectors: %d\n", s_fs_info.NumSectors);
	printf("Bytes per sector: %d\n", s_fs_info.BytesPerSector);
	printf("Root cluster: %d\n", s_fs_info.RootCluster);
	printf("Root directory start sector: %d\n", s_fs_info.RootDirectoryStartSector);
	printf("Data start sector: %d\n", s_fs_info.DataStartSector);
	printf("Data num clusters: %d\n", s_fs_info.DataNumClusters);
	printf("------THEIR CODE-------");
	read_boot_sector_original();
	info_bs();
	return status;
}

// Todo: Remove this garbage.
uint8_t read_boot_sector_original(void) {
	uint8_t buf[512] = { 0 };

	SD_read_lba(buf, MBR_BS_Location);

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
	// printf("\nFile System: %s", FATType);
	return 0;
}






// Initialize the Boot Sector

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
// The FAT is a linked list of clusters. Given a cluster, this function finds
// the next cluster in the list.
uint32_t next_cluster(uint32_t cur_cluster) {
	uint32_t fat_offset = -1;
	switch (filesystem_type(CountofClusters)) {
	// FAT12 cluster chain entries are 12 bits, or 1.5 bytes.
	case TypeFAT12: fat_offset = cur_cluster + cur_cluster/2; break;
	case TypeFAT16: fat_offset = cur_cluster*2; break;
	case TypeFAT32: fat_offset = cur_cluster*4; break;
	}

	uint32_t sector = BPB_RsvdSecCnt + (fat_offset / BPB_BytsPerSec);
	uint32_t entry_offset = fat_offset % BPB_BytsPerSec;

	uint8_t buf[512] = { 0 };
	SD_read_lba(buf, MBR_BS_Location + sector);

	switch (filesystem_type(CountofClusters)) {
	case TypeFAT12:
		uint16_t low = buf[entry_offset];
		uint16_t high = buf[(entry_offset + 1) % BPB_BytsPerSec];
		if (entry_offset == BPB_BytsPerSec - 1) {
			SD_read_lba(buf, MBR_BS_Location + sector + 1);
			high = buf[0]; // FAT12 entries can span multiple sectors.
		}
		uint16_t next = low | (high << 8);
		if (cur_cluster & 0x0001) return next >> 4;
		else return next & 0x0FFF;
	case TypeFAT16:
		return read_uint16(&buf[entry_offset]);
	case TypeFAT32:
		return read_uint32(&buf[entry_offset]) & 0x0FFFFFFF;
	}
}
// Returns true if the Cluster is the last cluster in the file's cluster chain
bool isEOF(uint32_t FATContent) {
	switch (filesystem_type(CountofClusters)) {
	case TypeFAT12: return FATContent >= 0x0FF8;
	case TypeFAT16: return FATContent >= 0xFFF8;
	case TypeFAT32: return FATContent >= 0x0FFFFFF8;
	}
}
// Buffers the cluster chain of a file so that it can be streamed
void build_cluster_chain(int cc[], uint32_t length, data_file *df) {
	cc[0] = df->FirstCluster;
	for (int i = 1; i < length; i++) {
		cc[i] = next_cluster(cc[i - 1]);
	}
	// assert(isEOF(cc[length - 1]));
}

// Calculates the First Sector of Cluster number 'N'
static uint32_t first_sector_of_cluster(uint32_t N) {
	return (((N - 2) * BPB_SecPerClus) + FirstDataSector + MBR_BS_Location);
}
// Searches for a particular file extension specified by "extension"
// To browse from the start of the file system use
// search_for_filetye("extension",0,1);
uint32_t search_for_filetype(char *extension, data_file *df, int sub_directory,
		bool search_root) {
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
		SD_read_lba(buf, directory);
	} else {
		//Search the sub directory
		SD_read_lba(buf, sub_directory);
		directory = sub_directory;
		// start from entry number 2 to skip over the
		// ./ Current directory Entry and the ../ Parent directory entry
		entry_num = 2;
	}

	//Browse while there are still entries to browse
	while ((buf[entry_num*32] != 0x00)) {
		uint8_t *entry = buf + entry_num*32;
		ATTR_LONG_NAME_MASK = entry[attribute_offset] & 0x3F;
		ATTR_LONG_NAME = entry[attribute_offset] & 0x0F;

		//Determine if the entry contains a long file name
		if (((entry[attribute_offset] & ATTR_LONG_NAME_MASK)
				== ATTR_LONG_NAME)
				&& (entry[attribute_offset] != 0x08)
				&& (entry[0] != 0xE5)) //long filename
		{
			//longname_blocks is the amount of entries that contain the long filename
			int longname_blocks = (entry[0] & 0xBF);

			//read the file name from the buffer and store it into longname[]
			while (longname_blocks > 0) {
				longname_blocks--;
				entry_num++;

				//if the entry number spans beyond the current sector, grab the next one
				if (entry_num*32 >= BPB_BytsPerSec) {
					root_sector_count++;
					SD_read_lba(buf, directory + root_sector_count);
					entry_num = 0;
				}
			}
		}

		entry = buf + entry_num*32;

		//Read the attribute tag of the current entry
		//0x00 Indicates the end of the FAT entries
		//0x08 Indicates Volume ID
		//0xE5 Indicates and empty entry
		//0x10 Indicates a Directory
		//anything else indicates a file

		attribute = entry[attribute_offset];

		if ((attribute & 0x08) || (entry[0] == 0xE5)) {
			//0x08 Indicates Volume ID
			//0xE5 Indicates and empty entry
			//Either case increment to next entry
		} else if (attribute & 0x10) {
			//Indicates a Directory, search the directory
			sub_directory = (entry[FstClusLo_offset])
					+ (entry[FstClusLo_offset + 1] << 8)
					+ (entry[FstClusHi_offset])
					+ (entry[FstClusHi_offset + 1] << 8);
			sub_directory = first_sector_of_cluster(sub_directory);
			if (!search_for_filetype(extension, df, sub_directory, 0)) {
				return 0;
			}
		} else {
			//Indicates a file
			for (int i = 0; i < 11; i++) {
				filename[i] = buf[entry_num*32 + i];
			}
			filename[11] = '\0';

			//Grab the current entry numbers file extension
			fileext[0] = entry[8];
			fileext[1] = entry[9];
			fileext[2] = entry[10];
			fileext[3] = '\0';

			//printf("found file: %s.%s\n", filename, fileext);

			//compare the current file's file extension to the extension to search for
			if (!strcmp(extension, fileext)) {
				if (file_count == file_number) {
					memcpy(df->Name, entry, 11 * sizeof(char));
					df->Name[11] = '\0';
					df->Attr = attribute;

					// TODO: is this wrong?
					df->FirstCluster =
						(entry[FstClusLo_offset]) +
						(entry[FstClusLo_offset + 1] << 8) +
						(entry[FstClusHi_offset]) +
						(entry[FstClusHi_offset + 1] << 8);


					df->FileSize =
						(entry[FileSize_offset]) |
						(entry[FileSize_offset + 1] << 8) |
						(entry[FileSize_offset + 2] << 16) |
						(entry[FileSize_offset + 3] << 24);
					df->FirstSector = first_sector_of_cluster(df->FirstCluster);
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
		if (entry_num*32 >= BPB_BytsPerSec) {
			root_sector_count++;
			SD_read_lba(buf, directory + root_sector_count);
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


inline uint32_t get_file_cluster_count(data_file *df) {
	return ceil_div(df->FileSize, BPB_BytsPerSec * BPB_SecPerClus);
}

inline uint32_t get_file_sector_count(data_file *df) {
	return ceil_div(df->FileSize, BPB_BytsPerSec);
}
