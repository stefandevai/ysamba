namespace dl::ui
{
constexpr std::array<char, 52> alphabet{
    'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r',
    's', 't', 'u', 'v', 'x', 'y', 'w', 'z', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J',
    'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'X', 'Y', 'W', 'Z',
};

constexpr char get_character_by_index(const int index)
{
  assert(index >= 0 && index < 52 && "Index out of bounds");
  return alphabet[index];
}
}  // namespace dl::ui
