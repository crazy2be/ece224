#include "file_stream.h"

#include "sd.h"
#include <stdlib.h>

void fs_init(struct file_stream *fs, data_file *f) {
	fs->cluster_chain_len = get_file_cluster_count(f);
	fs->cluster_chain = (int*) malloc(fs->cluster_chain_len * sizeof(int));
	build_cluster_chain(fs->cluster_chain, fs->cluster_chain_len, f);

	//fs->buffer = (uint8_t*) malloc(BPB_BytsPerSec * sizeof(uint8_t));
	fs->sector_index = 0;
	fs->file = f;
}

void fs_free(struct file_stream *fs) {
	free(fs->cluster_chain);
}

int fs_read(struct file_stream *fs, uint8_t *buf) {
	uint32_t sector_count = get_file_sector_count(fs->file);
	if (fs->sector_index >= sector_count) {
		return -1; //sector is out of range
	}

	//get sector
	int lba = (fs->sector_index % BPB_SecPerClus) +
			first_sector_of_cluster(fs->cluster_chain[fs->sector_index/ BPB_SecPerClus]);
	SD_read_lba(buf, lba, 1);

	fs->sector_index++;

	return (fs->sector_index == sector_count)
		? fs->file->FileSize % BPB_BytsPerSec
		: BPB_BytsPerSec;
}

bool fs_seek(struct file_stream *fs, uint32_t sector) {
	uint32_t sector_count = get_file_sector_count(fs->file);
	if (sector >= sector_count) {
		return false;
	} else {
		fs->sector_index = sector;
		return true;
	}
}

void fs_seek_rel(struct file_stream *fs, uint32_t rel) {
	uint32_t sector_count = get_file_sector_count(fs->file);
    fs->sector_index = (fs->sector_index + rel) % sector_count;
}
