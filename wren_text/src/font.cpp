#include "font.hpp"

#include <fontconfig/fontconfig.h>
#include <spdlog/spdlog.h>

#include <filesystem>
#include <tl/expected.hpp>

namespace wren::text {

auto load_default_font() -> wren::expected<std::filesystem::path> {
  FcConfig* config = FcInitLoadConfigAndFonts();

  std::optional<std::filesystem::path> font_path;

  for (auto const& font : DEFAULT_FONTS) {
    FcPattern* pat = FcNameParse((FcChar8* const)(font.data()));
    FcConfigSubstitute(config, pat, FcMatchPattern);
    FcDefaultSubstitute(pat);

    FcResult res{};
    FcPattern* f = FcFontMatch(config, pat, &res);
    if (f) {
      FcChar8* file = nullptr;
      if (FcPatternGetString(f, FC_FILE, 0, &file) == FcResultMatch) {
        font_path = std::filesystem::path{(char*)file};
      }
    }
  }

  if (font_path.has_value()) {
    return *font_path;
  }

  return tl::make_unexpected(
      std::make_error_code(std::errc::no_such_file_or_directory));
}

}  // namespace wren::text
