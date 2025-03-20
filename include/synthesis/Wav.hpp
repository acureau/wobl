#pragma once

#include <cstdint>
#include <string>
#include <vector>

void export_wav(const std::string& file_path, int16_t channel_count, int32_t sample_rate, const std::vector<float>& data);