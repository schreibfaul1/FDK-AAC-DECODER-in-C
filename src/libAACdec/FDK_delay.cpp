

/**************************** AAC decoder library ******************************

   Author(s):   Matthias Hildenbrand

   Description:

*******************************************************************************/

#include "FDK_delay.h"
#include <memory.h>
#include "../libSYS/genericStds.h"

#define MAX_FRAME_LENGTH (1024)

int32_t FDK_Delay_Create(FDK_SignalDelay* data, const uint16_t delay,
                     const uint8_t num_channels) {
  assert(data != NULL);
  assert(num_channels > 0);

  if (delay > 0) {
    data->delay_line =
        (int32_t*)FDKcalloc(num_channels * delay, sizeof(int32_t));
    if (data->delay_line == NULL) {
      return -1;
    }
  } else {
    data->delay_line = NULL;
  }
  data->num_channels = num_channels;
  data->delay = delay;

  return 0;
}

void FDK_Delay_Apply(FDK_SignalDelay* data, int32_t* time_buffer,
                     const uint32_t frame_length, const uint8_t channel) {
  assert(data != NULL);

  if (data->delay > 0) {
    C_ALLOC_SCRATCH_START(tmp, int32_t, MAX_FRAME_LENGTH)
    assert(frame_length <= MAX_FRAME_LENGTH);
    assert(channel < data->num_channels);
    assert(time_buffer != NULL);
    if (frame_length >= data->delay) {
      memcpy(tmp, &time_buffer[frame_length - data->delay],
                data->delay * sizeof(int32_t));
      memmove(&time_buffer[data->delay], &time_buffer[0], (frame_length - data->delay) * sizeof(int32_t));

      memcpy(&time_buffer[0], &data->delay_line[channel * data->delay],
                data->delay * sizeof(int32_t));
      memcpy(&data->delay_line[channel * data->delay], tmp,
                data->delay * sizeof(int32_t));
    } else {
      memcpy(tmp, &time_buffer[0], frame_length * sizeof(int32_t));
      memcpy(&time_buffer[0], &data->delay_line[channel * data->delay],
                frame_length * sizeof(int32_t));
      memcpy(&data->delay_line[channel * data->delay],
                &data->delay_line[channel * data->delay + frame_length],
                (data->delay - frame_length) * sizeof(int32_t));
      memcpy(&data->delay_line[channel * data->delay +
                                  (data->delay - frame_length)],
                tmp, frame_length * sizeof(int32_t));
    }
    C_ALLOC_SCRATCH_END(tmp, int32_t, MAX_FRAME_LENGTH)
  }

  return;
}

void FDK_Delay_Destroy(FDK_SignalDelay* data) {
  if (data->delay_line != NULL) {
    FDKfree(data->delay_line);
  }
  data->delay_line = NULL;
  data->delay = 0;
  data->num_channels = 0;

  return;
}
