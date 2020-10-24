#include <cmath>
#include <algorithm>

std::vector<std::vector<ColorInfo>> ColorInfoSorted2D() {
  // Acquire the color information for the palette256.
  std::vector<ColorInfo> info_gray;
  std::vector<ColorInfo> info_color;
  for (int i = 16; i < 256; ++i) {
    ColorInfo info = GetColorInfo(Color::Palette256(i));
    if (info.saturation == 0)
      info_gray.push_back(info);
    else
      info_color.push_back(info);
  }

  // Sort info_color by hue.
  std::sort(
      info_color.begin(), info_color.end(),
      [](const ColorInfo& A, const ColorInfo& B) { return A.hue < B.hue; });

  // Make 8 colums, one gray and seven colored.
  std::vector<std::vector<ColorInfo>> info_columns(8);
  info_columns[0] = info_gray;
  for (size_t i = 0; i < info_color.size(); ++i) {
    info_columns[1 + 7 * i / info_color.size()].push_back(info_color[i]);
  }

  // Minimize discontinuities for every columns.
  for (auto& column : info_columns) {
    std::sort(column.begin(), column.end(),
              [](const ColorInfo& A, const ColorInfo& B) {
                return A.value < B.value;
              });
    for (size_t i = 0; i < column.size() - 1; ++i) {
      int best_index = i + 1;
      int best_distance = 255 * 255 * 3;
      for (size_t j = i + 1; j < column.size(); ++j) {
        int dx = column[i].red - column[j].red;
        int dy = column[i].green - column[j].green;
        int dz = column[i].blue - column[j].blue;
        int distance = dx * dx + dy * dy + dz * dz;
        if (best_distance > distance) {
          best_distance = distance;
          best_index = j;
        }
      }
      std::swap(column[i + 1], column[best_index]);
    }
  }

  return std::move(info_columns);
}
