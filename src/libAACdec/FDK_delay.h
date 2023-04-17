

/**************************** AAC decoder library ******************************

   Author(s):   Matthias Hildenbrand

   Description:

*******************************************************************************/

#ifndef FDK_DELAY_H
#define FDK_DELAY_H
#include "newAACDecoder.h"
#include "aac_rom.h"

/**
 * Structure representing one delay element for multiple channels.
 */
typedef struct {
  int32_t* delay_line; /*!< Pointer which stores allocated delay line. */
  uint16_t delay;        /*!< Delay required in samples (per channel). */
  uint8_t num_channels;  /*!< Number of channels to delay. */
} FDK_SignalDelay;

/**
 * \brief Create delay element for multiple channels with fixed delay value.
 *
 * \param data Pointer delay element structure.
 * \param delay Required delay value in samples per channel.
 * \param num_channels Required number of channels.
 *
 * \return -1 on out of memory, else 0
 */
int32_t FDK_Delay_Create(FDK_SignalDelay* data, const uint16_t delay,
                     const uint8_t num_channels);

/**
 * \brief Apply delay to one channel (non-interleaved storage assumed).
 *
 * \param data Pointer delay element structure.
 * \param time_buffer Pointer to signal to delay.
 * \param frame_length Frame length of input/output signal (needs to be >=
 * delay).
 * \param channel Index of current channel (0 <= channel < num_channels).
 *
 * \return void
 */
void FDK_Delay_Apply(FDK_SignalDelay* data, int32_t* time_buffer,
                     const uint32_t frame_length, const uint8_t channel);

/**
 * \brief Destroy delay element.
 *
 * \param data Pointer delay element structure.
 *
 * \return void
 */
void FDK_Delay_Destroy(FDK_SignalDelay* data);

#endif /* #ifndef FDK_DELAY_H */
