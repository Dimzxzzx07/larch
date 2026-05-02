#include <stdint.h>

#define HDA_GCTL 0x08
#define HDA_STATESTS 0x0E
#define HDA_WAKEEN 0x0C

typedef struct {
    uint32_t gcap;
    uint32_t gctl;
    uint32_t wakeen;
    uint32_t states;
    uint32_t gsts;
} hda_regs_t;

static volatile hda_regs_t* hda = (volatile hda_regs_t*)0xFEBF0000;

void hda_init(void) {
    if (!hda) return;
    
    uint32_t gcap = hda->gcap;
    int num_streams = (gcap >> 8) & 0x0F;
    int num_bidir = (gcap >> 12) & 0x0F;
    
    hda->gctl = (hda->gctl & ~1) | 1;
    
    for (int i = 0; i < 10000; i++) {
        if (hda->gctl & 1) break;
    }
}

void hda_play_sample(uint16_t* buffer, uint32_t length, int stream_id) {
    volatile uint32_t* stream = (volatile uint32_t*)(0xFEBF1000 + stream_id * 0x80);
    stream[0] = 0x80000000;
    stream[2] = (uint32_t)buffer;
    stream[3] = length / 4;
    stream[0] = 0x80000001;
}

void hda_stop_stream(int stream_id) {
    volatile uint32_t* stream = (volatile uint32_t*)(0xFEBF1000 + stream_id * 0x80);
    stream[0] = 0x80000000;
}