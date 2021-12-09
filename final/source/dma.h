#ifndef __DMA_H__
#define __DMA_H__

/**
 * @brief Initialize DMA0 to enable interrupt and
 * TPM0 triggering.
 */
void DMA0_init();

void Configure_DMA_Playback(uint16_t *src, int len);

#endif

