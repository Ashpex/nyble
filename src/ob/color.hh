/*
                                    88888888
                                  888888888888
                                 88888888888888
                                8888888888888888
                               888888888888888888
                              888888  8888  888888
                              88888    88    88888
                              888888  8888  888888
                              88888888888888888888
                              88888888888888888888
                             8888888888888888888888
                          8888888888888888888888888888
                        88888888888888888888888888888888
                              88888888888888888888
                            888888888888888888888888
                           888888  8888888888  888888
                           888     8888  8888     888
                                   888    888

                                   OCTOBANANA

Licensed under the MIT License

Copyright (c) 2019 Brett Robinson <https://octobanana.com/>

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#ifndef OB_COLOR_HH
#define OB_COLOR_HH

#include "ob/string.hh"
#include "ob/term.hh"
namespace aec = OB::Term::ANSI_Escape_Codes;

#include <cmath>
#include <cstdio>
#include <cstddef>

#include <regex>
#include <string>
#include <random>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <unordered_map>

namespace OB
{

class Color
{
  inline static const std::unordered_map<std::string, std::string> color_fg {
    {"black", "\x1b[30m"},
    {"black bright", "\x1b[90m"},
    {"red", "\x1b[31m"},
    {"red bright", "\x1b[91m"},
    {"green", "\x1b[32m"},
    {"green bright", "\x1b[92m"},
    {"yellow", "\x1b[33m"},
    {"yellow bright", "\x1b[93m"},
    {"blue", "\x1b[34m"},
    {"blue bright", "\x1b[94m"},
    {"magenta", "\x1b[35m"},
    {"magenta bright", "\x1b[95m"},
    {"cyan", "\x1b[36m"},
    {"cyan bright", "\x1b[96m"},
    {"white", "\x1b[37m"},
    {"white bright", "\x1b[97m"},
  };

  inline static const std::unordered_map<std::string, std::string> color_bg {
    {"black", "\x1b[40m"},
    {"black bright", "\x1b[100m"},
    {"red", "\x1b[41m"},
    {"red bright", "\x1b[101m"},
    {"green", "\x1b[42m"},
    {"green bright", "\x1b[102m"},
    {"yellow", "\x1b[43m"},
    {"yellow bright", "\x1b[103m"},
    {"blue", "\x1b[44m"},
    {"blue bright", "\x1b[104m"},
    {"magenta", "\x1b[45m"},
    {"magenta bright", "\x1b[105m"},
    {"cyan", "\x1b[46m"},
    {"cyan bright", "\x1b[106m"},
    {"white", "\x1b[47m"},
    {"white bright", "\x1b[107m"},
  };

public:

  struct Mode
  {
    enum Type
    {
      null = 0,
      rainbow,
      candy,
      party
    };
  };

  struct RGB
  {
    // range [0-255]
    double r {0};
    double g {0};
    double b {0};
  };

  struct HSL
  {
    // range [0-1]
    double h {0};
    double s {0};
    double l {0};
  };

  struct Type
  {
    enum value
    {
      bg = 0,
      fg
    };
  };

  Color() = default;

  Color(Type::value const fg) noexcept :
    _fg {static_cast<bool>(fg)}
  {
  }

  Color(std::string const& k, Type::value const fg = Type::value::fg) :
    _fg {static_cast<bool>(fg)}
  {
    key(k);
  }

  Color(Color&&) = default;
  Color(Color const&) = default;
  ~Color() = default;

  Color& operator=(Color&& rhs) = default;
  Color& operator=(Color const&) = default;

  Color& operator=(std::string const& k)
  {
    clear();
    key(k);

    return *this;
  }

  operator bool() const
  {
    return _valid;
  }

  operator std::string() const
  {
    return _key;
  }

  friend std::ostream& operator<<(std::ostream& os, Color const& obj)
  {
    os << obj.value();

    return os;
  }

  friend std::string& operator+=(std::string& str, Color const& obj)
  {
    return str += obj.value();
  }

  bool is_valid() const
  {
    return _valid;
  }

  Color& clear()
  {
    _key = "clear";
    _value.clear();
    _hsl = HSL {50, 50, 50};
    _valid = false;

    return *this;
  }

  Color& fg()
  {
    if (! _fg)
    {
      _fg = true;
      key(_key);
    }

    return *this;
  }

  Color& bg()
  {
    if (_fg)
    {
      _fg = false;
      key(_key);
    }

    return *this;
  }

  bool is_fg() const
  {
    return _fg;
  }

  bool is_bg() const
  {
    return ! _fg;
  }

  Mode::Type mode() const
  {
    return _mode;
  }

  Color& step(double const val = 0)
  {
    if (val != 0)
    {
      _hsl.h += val;

      if (_hsl.h < 0)
      {
        _hsl.h += 100;
      }
      else if (_hsl.h > 100)
      {
        _hsl.h -= 100;
      }

      _value = _fg ? aec::fg_true(hsl_to_hex(_hsl)) :
        aec::bg_true(hsl_to_hex(_hsl));

      return *this;
    }

    switch(_mode)
    {
      case Mode::rainbow:
      {
        _hsl.h = _hsl.h < 100 ? _hsl.h + 0.2 : _hsl.h + 0.2 - 100;
        _value = _fg ? aec::fg_true(hsl_to_hex(_hsl)) :
          aec::bg_true(hsl_to_hex(_hsl));

        break;
      }

      case Mode::candy:
      {
        _hsl.h = _hsl.h < 100 ? _hsl.h + 0.4 : _hsl.h + 0.4 - 100;
        _value = _fg ? aec::fg_true(hsl_to_hex(_hsl)) :
          aec::bg_true(hsl_to_hex(_hsl));

        break;
      }

      case Mode::party:
      {
        _hsl.h = random(0, 100);
        _value = _fg ? aec::fg_true(hsl_to_hex(_hsl)) :
          aec::bg_true(hsl_to_hex(_hsl));

        break;
      }

      case Mode::null:
      default:
      {
        break;
      }
    }

    return *this;
  }

  double hue() const
  {
    return _hsl.h;
  }

  Color& hue(double const val)
  {
    if (val < 0)
    {
      _hsl.h = 100;
    }
    else if (val > 100)
    {
      _hsl.h = 0;
    }
    else
    {
      _hsl.h = val;
    }

    _key = hsl_to_hex(_hsl);
    _value = _fg ? aec::fg_true(_key) : aec::bg_true(_key);

    return *this;
  }

  double sat() const
  {
    return _hsl.s;
  }

  Color& sat(double const val)
  {
    if (val < 0)
    {
      _hsl.s = 100;
    }
    else if (val > 100)
    {
      _hsl.s = 0;
    }
    else
    {
      _hsl.s = val;
    }

    _key = hsl_to_hex(_hsl);
    _value = _fg ? aec::fg_true(_key) : aec::bg_true(_key);

    return *this;
  }

  double lum() const
  {
    return _hsl.l;
  }

  Color& lum(double const val)
  {
    if (val < 0)
    {
      _hsl.l = 100;
    }
    else if (val > 100)
    {
      _hsl.l = 0;
    }
    else
    {
      _hsl.l = val;
    }

    _key = hsl_to_hex(_hsl);
    _value = _fg ? aec::fg_true(_key) : aec::bg_true(_key);

    return *this;
  }

  std::string key() const
  {
    return _key;
  }

  bool key(std::string const& k)
  {
    if (! k.empty())
    {
      if (k == "clear")
      {
        // clear
        _key = k;
        _value = "";
        _valid = true;
      }
      else if (k == "reverse")
      {
        // reverse
        _key = k;
        _value = "\x1b[7m";
        _valid = true;
      }
      else if (k == "rainbow" || k == "candy" || k == "party")
      {
        switch(k.at(0))
        {
          case 'r':
          {
            _mode = Mode::rainbow;

            break;
          }

          case 'c':
          {
            _mode = Mode::candy;

            break;
          }

          case 'p':
          {
            _mode = Mode::party;

            break;
          }

          default:
          {
            break;
          }
        }

        _key = k;
        _hsl = HSL {50, 50, 50};
        _hsl.h = random(0, 100);
        _value = _fg ? aec::fg_true(hsl_to_hex(_hsl)) :
          aec::bg_true(hsl_to_hex(_hsl));
        _valid = true;
      }
      else
      {
        // 21-bit color
        if (k.at(0) == '#' && OB::String::assert_rx(k,
          std::regex("^#[0-9a-fA-F]{3}(?:[0-9a-fA-F]{3})?$")))
        {
          _key = k;
          _value = _fg ? aec::fg_true(k) : aec::bg_true(k);
          _hsl = hex_to_hsl(k);
          _valid = true;
        }

        // 8-bit color
        else if (OB::String::assert_rx(k, std::regex("^[0-9]{1,3}$")) &&
          std::stoi(k) >= 0 && std::stoi(k) <= 255)
        {
          _key = k;
          _value = _fg ? aec::fg_256(k) : aec::bg_256(k);
          _valid = true;
        }

        // 4-bit color
        else if (color_fg.find(k) != color_fg.end())
        {
          _key = k;

          if (_fg)
          {
            _value = color_fg.at(k);
          }
          else
          {
            _value = color_bg.at(k);
          }

          _valid = true;
        }
      }
    }

    return _valid;
  }

  std::string value() const
  {
    return _value;
  }

  std::string hex() const
  {
    return hsl_to_hex(_hsl);
  }

  RGB rgb() const
  {
    return hsl_to_rgb(_hsl);
  }

  HSL hsl() const
  {
    return _hsl;
  }

  static HSL hex_to_hsl(std::string const& hex)
  {
    return rgb_to_hsl(hex_to_rgb(hex));
  }

  static std::string hsl_to_hex(HSL hsl)
  {
    return rgb_to_hex(hsl_to_rgb(hsl));
  }

  static std::string hex_encode(char const c)
  {
    char s[3];

    if (c & 0x80)
    {
      std::snprintf(&s[0], 3, "%02x",
        static_cast<unsigned int>(c & 0xff)
      );
    }
    else
    {
      std::snprintf(&s[0], 3, "%02x",
        static_cast<unsigned int>(c)
      );
    }

    return std::string(s);
  }

  static std::size_t hex_decode(std::string const& str_)
  {
    std::stringstream ss;
    ss << str_;
    std::size_t val;
    ss >> std::hex >> val;

    return val;
  }

  static bool valid_hstr(std::string& str)
  {
    std::smatch m;
    std::regex rx {"^#?((?:[0-9a-fA-F]{3}){1,2})$"};

    if (std::regex_match(str, m, rx, std::regex_constants::match_not_null))
    {
      std::string hstr {m[1]};

      if (hstr.size() == 3)
      {
        std::stringstream ss;
        ss << hstr[0] << hstr[0] << hstr[1] << hstr[1] << hstr[2] << hstr[2];
        hstr = ss.str();
      }

      str = hstr;

      return true;
    }

    return false;
  }

  static RGB hex_to_rgb(std::string str)
  {
    if (! valid_hstr(str))
    {
      return {};
    }

    return RGB {static_cast<double>(hex_decode(str.substr(0, 2))), static_cast<double>(hex_decode(str.substr(2, 2))), static_cast<double>(hex_decode(str.substr(4, 2)))};
  }

  static std::string rgb_to_hex(RGB rgb)
  {
    std::ostringstream os; os
    << "#"
    << hex_encode(static_cast<char>(rgb.r))
    << hex_encode(static_cast<char>(rgb.g))
    << hex_encode(static_cast<char>(rgb.b));

    return os.str();
  }

  static double hue_to_rgb(double j, double i, double h)
  {
    double r;

    if (h < 0)
    {
      h += 1;
    }

    if (h > 1)
    {
      h -= 1;
    }

    if (h < 1 / 6.0)
    {
      r = j + (i - j) * 6 * h;
    }
    else if (h < 1 / 2.0)
    {
      r = i;
    }
    else if (h < 2 / 3.0)
    {
      r = j + (i - j) * (2 / 3.0 - h) * 6;
    }
    else
    {
      r = j;
    }

    return r;
  }

  static RGB hsl_to_rgb(HSL hsl)
  {
    RGB rgb;

    hsl.h /= 100;
    hsl.s /= 100;
    hsl.l /= 100;

    if (hsl.s <= 0.0001)
    {
      rgb.r = hsl.l;
      rgb.g = hsl.l;
      rgb.b = hsl.l;
    }
    else
    {
      double const i {(hsl.l < 0.5) ? (hsl.l * (1 + hsl.s)) : ((hsl.l + hsl.s) - (hsl.l * hsl.s))};
      double const j {2 * hsl.l - i};

      rgb.r = hue_to_rgb(j, i, (hsl.h + 1 / 3.0));
      rgb.g = hue_to_rgb(j, i, hsl.h);
      rgb.b = hue_to_rgb(j, i, (hsl.h - 1 / 3.0));
    }

    rgb.r *= 255;
    rgb.g *= 255;
    rgb.b *= 255;

    return rgb;
  }

  static HSL rgb_to_hsl(RGB rgb)
  {
    rgb.r /= 255;
    rgb.g /= 255;
    rgb.b /= 255;

    auto const min = std::min(rgb.r, std::min(rgb.g, rgb.b));
    auto const max = std::max(rgb.r, std::max(rgb.g, rgb.b));
    auto const init {(max + min) / 2.0};

    HSL hsl {init, init, init};

    if (max == min)
    {
      hsl.h = 0;
      hsl.s = 0;
    }
    else
    {
      auto const v {max - min};
      hsl.s = (hsl.l > 0.5) ? v / (2 - (max - min)) : v / (max + min);

      if (max == rgb.r)
      {
        hsl.h = (rgb.g - rgb.b) / v + (rgb.g < rgb.b ? 6 : 0);
      }
      else if (max == rgb.g)
      {
        hsl.h = (rgb.b - rgb.r) / v + 2;
      }
      else
      {
        hsl.h = (rgb.r - rgb.g) / v + 4;
      }

      hsl.h /= 6.0;
    }

    hsl.h *= 100;
    hsl.s *= 100;
    hsl.l *= 100;

    return hsl;
  }

private:

  std::size_t random(std::size_t min, std::size_t max) const
  {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<std::size_t> distr(min, max);

    return distr(gen);
  }

  bool _valid {false};
  bool _fg {true};
  std::string _key {"clear"};
  std::string _value;

  Mode::Type _mode {Mode::null};
  HSL _hsl {50, 50, 50};
}; // Color

} // namespace OB

#endif // OB_COLOR_HH
