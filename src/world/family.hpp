#pragma once

#include <cstdint>
#include <unordered_map>

#include "./sex.hpp"

namespace dl
{
struct FamilyMember;

using FamilyMemberContainer = std::unordered_map<uint32_t, std::shared_ptr<FamilyMember>>;

struct FamilyMember
{
  FamilyMember(const uint32_t id, const Sex sex) : id(id), sex(sex) {}

  uint32_t id;
  Sex sex;
  std::shared_ptr<FamilyMember> father;
  std::shared_ptr<FamilyMember> mother;
  std::shared_ptr<FamilyMember> spouse;
  FamilyMemberContainer children;
};

class Family
{
 public:
  static const uint32_t null_member;

  Family() {}

  [[nodiscard]] uint32_t add_first_member(const Sex sex);
  [[nodiscard]] uint32_t add_child(const uint32_t parent_id, const Sex sex);
  [[nodiscard]] uint32_t add_son(const uint32_t parent_id);
  [[nodiscard]] uint32_t add_daughter(const uint32_t parent_id);
  [[nodiscard]] uint32_t add_spouse(const uint32_t person_id);
  [[nodiscard]] uint32_t add_father(const uint32_t child_id);
  [[nodiscard]] uint32_t add_mother(const uint32_t child_id);

 private:
  FamilyMemberContainer m_family;

  static uint32_t identifier() noexcept
  {
    static uint32_t value = 1;
    return value++;
  }
};
}  // namespace dl
