#ifndef __DMA_H__
#define __DMA_H__

/**
 * @brief Initialize DMA0 to enable interrupt and
 * TPM0 triggering.
 */
void DMA0_init();

/**
 * @brief testing if the DMA is performing the
 * transactions.
 * @return int: return 1 if running; return 0 if not.
 */
int is_DMA_running();

/**
 * @brief Configure DMA0 based on teh samples and cycles required.
 * @param src: array holding samples
 * @param sample_mnt: the total sample amount
 * @param dma_cycles: the number of DMA cycles required
 */
void Configure_DMA_Playback(uint16_t *src, int sample_mnt, \
		uint32_t dma_cycles);

#endif

