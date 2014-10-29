#include "SD_Card.h"
#include "fat.h"
#include "stdio.h"

int main(void) {
	data_file df;
	search_for_file_extension("wav", &df, 0, 1);

	printf("%s\n", df.Name);
}

