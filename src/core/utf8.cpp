#include "./utf8.hpp"

namespace dl
{
UTF8Iterator& UTF8Iterator::operator=(const UTF8Iterator& rhs)
{
  string_iterator = rhs.string_iterator;
  return *this;
}

char32_t UTF8Iterator::operator*()
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

UTF8Iterator& UTF8Iterator::operator++()
{
  m_dirty = true;

  const auto size = m_get_code_point_size();

  for (int i = 0; i < size; ++i)
  {
    ++string_iterator;
  }

  return *this;
}

UTF8Iterator UTF8Iterator::operator++(int)
{
  m_dirty = true;
  UTF8Iterator temp = *this;
  ++(*this);
  return temp;
}

UTF8Iterator UTF8Iterator::operator+=(int quantity)
{
  m_dirty = true;
  UTF8Iterator temp = *this;

  if (quantity > 0)
  {
    for (int i = 0; i < quantity; ++i)
    {
      ++(*this);
    }
  }
  else
  {
    for (int i = 0; i < -quantity; ++i)
    {
      --(*this);
    }
  }

  return temp;
}

UTF8Iterator& UTF8Iterator::operator--()
{
  m_dirty = true;

  --string_iterator;

  if (*string_iterator & m_first_bit_mask)
  {
    --string_iterator;

    if ((*string_iterator & m_second_bit_mask) == 0)
    {
      --string_iterator;

      if ((*string_iterator & m_third_bit_mask) == 0)
      {
        --string_iterator;
      }
    }
  }
  return *this;
}

UTF8Iterator UTF8Iterator::operator--(int)
{
  m_dirty = true;
  UTF8Iterator temp = *this;
  --(*this);
  return temp;
}

UTF8Iterator UTF8Iterator::operator-=(int quantity) { return operator+=(-quantity); }

uint8_t UTF8Iterator::m_get_code_point() const
{
  char8_t first_byte = *string_iterator;
  char32_t code_point = first_byte;

  const auto size = m_get_code_point_size();

  switch (size)
  {
  case 2:
  {
    code_point = (first_byte & 0x1f) << 0x6;
    const char8_t second_byte = *(string_iterator + 1);
    code_point += (second_byte & 0x3f);
    break;
  }
  case 3:
  {
    code_point = (first_byte & 0x0f) << 0xC;
    const char8_t second_byte = *(string_iterator + 1);
    code_point += (second_byte & 0x3f) << 0x6;
    const char8_t third_byte = *(string_iterator + 2);
    code_point += (third_byte & 0x3f);
    break;
  }
  case 4:
  {
    code_point = (first_byte & 0x07) << 0x12;
    const char8_t second_byte = *(string_iterator + 1);
    code_point += (second_byte & 0x3F) << 0xC;
    const char8_t third_byte = *(string_iterator + 2);
    code_point += (third_byte & 0x3F) << 0x6;
    const char8_t fourth_byte = *(string_iterator + 3);
    code_point += (fourth_byte & 0x3F);
  }
  break;
  default:
    break;
  }

  return code_point;
}

uint8_t UTF8Iterator::m_get_code_point_size() const
{
  uint8_t size = 1;
  char8_t first_byte = *string_iterator;

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
}  // namespace dl
