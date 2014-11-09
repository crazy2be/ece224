#include "sd.h"
#include "fat.h"

#include <stdio.h>
#include <string.h>
#include <assert.h>

static FILE *s_test_fs = NULL;
int SD_read_lba(uint8_t *buf, uint32_t lba) {
	printf("Reading %d\n", lba);
	assert(fseek(s_test_fs, lba*512, SEEK_SET) == 0);
	int res = fread(buf, 512, 1, s_test_fs);
	printf("Res: %d\n", res);
	assert(res == 1);
	return 0;
}

int main() {
	s_test_fs = fopen("testfs", "r");
	printf("%d\n", fat_init());
	return 0;
}
