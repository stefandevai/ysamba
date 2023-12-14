#pragma once

#include <cstdint>
#include <iterator>
#include <string>

namespace dl
{
class UTF8Iterator
{
 public:
  using iterator_category = std::bidirectional_iterator_tag;
  using difference_type = std::string::difference_type;
  using value_type = char32_t;
  using pointer = const char32_t*;
  using reference = const char32_t&;

  UTF8Iterator(std::string::const_iterator it) : m_string_iterator(it) {}

  UTF8Iterator(std::string::iterator it) : m_string_iterator(it) {}

  UTF8Iterator(const UTF8Iterator& other) : m_string_iterator(other.m_string_iterator) {}

  UTF8Iterator& operator=(const UTF8Iterator& rhs)
  {
    m_string_iterator = rhs.m_string_iterator;
    return *this;
  }

  char32_t operator*()
  {
    if (!m_dirty)
    {
      return m_code_point;
    }

    const auto code_point = m_get_code_point();
    m_code_point = code_point;
    m_dirty = false;

    return code_point;
  }

  UTF8Iterator& operator++()
  {
    m_dirty = true;
    m_string_iterator += m_get_codepoint_size();
    return *this;
  }

  UTF8Iterator operator++(int)
  {
    m_dirty = true;
    UTF8Iterator temp = *this;
    ++(*this);
    return temp;
  }

  UTF8Iterator& operator--()
  {
    m_dirty = true;

    --m_string_iterator;

    if (*m_string_iterator & m_first_bit_mask)
    {
      --m_string_iterator;

      if ((*m_string_iterator & m_second_bit_mask) == 0)
      {
        --m_string_iterator;

        if ((*m_string_iterator & m_third_bit_mask) == 0)
        {
          --m_string_iterator;
        }
      }
    }
    return *this;
  }

  UTF8Iterator operator--(int)
  {
    m_dirty = true;
    UTF8Iterator temp = *this;
    --(*this);
    return temp;
  }

  bool operator==(const UTF8Iterator& rhs) const { return m_string_iterator == rhs.m_string_iterator; }

  bool operator!=(const UTF8Iterator& rhs) const { return m_string_iterator != rhs.m_string_iterator; }

  bool operator==(std::string::iterator rhs) const { return m_string_iterator == rhs; }

  bool operator==(std::string::const_iterator rhs) const { return m_string_iterator == rhs; }

  bool operator!=(std::string::iterator rhs) const { return m_string_iterator != rhs; }

  bool operator!=(std::string::const_iterator rhs) const { return m_string_iterator != rhs; }

 private:
  std::string::const_iterator m_string_iterator;
  static constexpr uint8_t m_first_bit_mask = 0x80;
  static constexpr uint8_t m_second_bit_mask = 0x40;
  static constexpr uint8_t m_third_bit_mask = 0x20;
  static constexpr uint8_t m_fourth_bit_mask = 0x10;
  bool m_dirty = true;
  uint32_t m_code_point = 0;

  uint8_t m_get_code_point()
  {
    char8_t first_byte = *m_string_iterator;
    char32_t code_point = first_byte;

    const auto size = m_get_codepoint_size();

    switch (size)
    {
    case 2:
    {
      code_point = (first_byte & 0x1f) << 6;
      const char8_t second_byte = *(m_string_iterator + 1);
      code_point += (second_byte & 0x3f);
      break;
    }
    case 3:
    {
      code_point = (first_byte & 0x0f) << 12;
      const char8_t second_byte = *(m_string_iterator + 1);
      code_point += (second_byte & 0x3f) << 6;
      const char8_t third_byte = *(m_string_iterator + 2);
      code_point += (third_byte & 0x3f);
      break;
    }
    case 4:
    {
      code_point = (first_byte & 0x07) << 18;
      const char8_t second_byte = *(m_string_iterator + 1);
      code_point += (second_byte & 0x3F) << 12;
      const char8_t third_byte = *(m_string_iterator + 2);
      code_point += (third_byte & 0x3F) << 6;
      ;
      const char8_t fourth_byte = *(m_string_iterator + 3);
      code_point += (fourth_byte & 0x3F);
    }
    break;
    default:
      break;
    }

    return code_point;
  }

  uint8_t m_get_codepoint_size() const
  {
    uint8_t size = 1;
    char8_t first_byte = *m_string_iterator;

    if (first_byte & m_first_bit_mask)
    {
      if (first_byte & m_third_bit_mask)
      {
        if (first_byte & m_fourth_bit_mask)
        {
          size = 4;
        }
        else
        {
          size = 3;
        }
      }
      else
      {
        size = 2;
      }
    }

    return size;
  }
};

}  // namespace dl
