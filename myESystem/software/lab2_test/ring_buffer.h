#pragma once
// Header file to allow inlining.
struct ring_buffer {
	int cap;
	int len;
	int put;
	int take;
	int16_t data[];
};

struct ring_buffer *ring_buffer_new(int cap) {
	struct ring_buffer *buf = malloc(sizeof(*buf) + cap*sizeof(*buf->data));
	assert(buf);
	*buf = (struct ring_buffer) {
			.cap = cap,
	};
	return buf;
}

static inline int mad(int a, int b) {
	a++;
	if (a >= b) return 0;
	return a;
}
static inline void ring_buffer_put(struct ring_buffer *buf, int16_t sample) {
	//assert(buf->put >= 0 && buf->take < buf->cap);
	//assert(buf->len < buf->cap);
	buf->data[buf->put] = sample;
	buf->put = mad(buf->put, buf->cap);
	buf->len++;
}

static inline int16_t ring_buffer_take(struct ring_buffer *buf) {
	//assert(buf->take >= 0 && buf->take < buf->cap);
	//assert(buf->len > 0);
	int16_t sample = buf->data[buf->take];
	buf->take = mad(buf->take, buf->cap);
	buf->len--;
	return sample;
}

static inline int ring_buffer_len(struct ring_buffer *buf) {
	return buf->len;
}
