#pragma once

#include <stdint.h>

void export_wav(const char* file_path, int16_t channel_count, int32_t sample_rate, int32_t data_size, const float* data);