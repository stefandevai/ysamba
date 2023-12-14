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

  UTF8Iterator& operator=(const UTF8Iterator& rhs);

  char32_t operator*();

  UTF8Iterator& operator++();

  UTF8Iterator operator++(int);

  UTF8Iterator& operator--();

  UTF8Iterator operator--(int);

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

  uint8_t m_get_code_point() const;
  uint8_t m_get_code_point_size() const;
};

}  // namespace dl
