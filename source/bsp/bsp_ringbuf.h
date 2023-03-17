#ifndef __BSP_RINGBUF_H__
#define __BSP_RINGBUF_H__

#include <rtthread.h>
#include <string.h>

#define rt_ringbuffer_space_len(rb) ((rb)->buffer_size - rt_ringbuffer_data_len(rb))

struct rt_ringbuffer
{
    rt_uint8_t *buffer_ptr;

    rt_uint16_t read_mirror : 1;
    rt_uint16_t read_index : 15;
    rt_uint16_t write_mirror : 1;
    rt_uint16_t write_index : 15;

    rt_int16_t buffer_size;
};

enum rt_ringbuffer_state
{
    RT_RINGBUFFER_EMPTY,
    RT_RINGBUFFER_FULL,
    /* half full is neither full nor empty */
    RT_RINGBUFFER_HALFFULL,
};

rt_inline enum rt_ringbuffer_state rt_ringbuffer_status(struct rt_ringbuffer *rb);
rt_size_t rt_ringbuffer_data_len(struct rt_ringbuffer *rb);
void rt_ringbuffer_init(struct rt_ringbuffer *rb,
                        rt_uint8_t           *pool,
                        rt_int16_t            size);
rt_size_t rt_ringbuffer_putchar(struct rt_ringbuffer *rb, const rt_uint8_t ch);
rt_size_t rt_ringbuffer_getchar(struct rt_ringbuffer *rb, rt_uint8_t *ch);

#endif
