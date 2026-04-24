#if __cplusplus < 202002L
#error charset support requies C++20
#else
#ifndef _CHARSET_H
#define _CHARSET_H

#include <cstddef>

#ifdef NDEBUG
#define fake_assert(condition) ((void)0)
#else
// fake assert works by basically breaking compilation if condition is false
// ... by the simple fact that the thing usiing it can't be statically compiled
// anymore
#define fake_assert(condition)                                                 \
  do {                                                                         \
    if (!(condition)) {                                                        \
      (*(volatile unsigned char *)(0) = (42));                                 \
    }                                                                          \
  } while (0)
#endif

namespace charset_impl {

  template <size_t N> struct TileString {
    char Str[N]{};

    constexpr TileString(char const (&Src)[N]) {
      for (size_t I = 0, J = 0; I < N; ++I) {
        if (Src[I] == '\\') {
          I++;
          Str[J++] = Src[I];
          continue;
        }
        fake_assert(Src[I] < 0x80);
        auto new_char = TranslateUnicode(Src[I]);
        Str[J++] = new_char;
      }
    }

    constexpr TileString(char16_t const (&Src)[N]) {
      for (size_t I = 0, J = 0; I < N; ++I) {
        fake_assert(Src[I] < 0xD800 || Src[I] > 0xDFFF);
        auto new_char = TranslateUnicode(Src[I]);
        Str[J++] = new_char;
      }
    }

    constexpr TileString(char32_t const (&Src)[N]) {
      for (size_t I = 0, J = 0; I < N; ++I) {
        auto new_char = TranslateUnicode(Src[I]);
        Str[J++] = new_char;
      }
    }

    constexpr char TranslateUnicode(char32_t C) {
      switch (C) {
      default:
        fake_assert(false);
        return 0xff;

      // C0 control codes are uninterpreted.
      case 0x0000 ... 0x001f:
        return (char)C;

      case U' ':
        return 0x3d;
      case U':' ... U'~':
        return (char)(C - 0x20);
      }
    }
  };

} // namespace charset_impl

// Converts strings to tiles
template <charset_impl::TileString S> constexpr auto operator""_ts() {
  return S.Str;
}

#endif // not _CHARSET_H
#endif // __cplusplus >= 202002L
