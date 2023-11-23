#pragma once

#include <string>
#include <unordered_map>

#include "./sex.hpp"

namespace dl
{
enum Predominance
{
  Patriarchy,
  Matriarchy,
};

enum SocietyType
{
  Nomadic,
  Sedentary,
};

enum ModeOfProduction
{
  PrimitiveCommunism,
  Slavery,
};

struct RelationshipNode;

using RelationshipNodeContainer = std::unordered_map<uint32_t, std::shared_ptr<RelationshipNode>>;

struct RelationshipNode
{
  RelationshipNode(const uint32_t id, const Sex sex) : id(id), sex(sex) {}

  uint32_t id;
  Sex sex;
  std::shared_ptr<RelationshipNode> father;
  std::shared_ptr<RelationshipNode> mother;
  std::shared_ptr<RelationshipNode> spouse;
  RelationshipNodeContainer children;
};

class Society
{
 public:
  static const uint32_t null_member;

  Society() {}

  std::string id;
  std::string name;
  unsigned int age;
  Predominance predominance;
  SocietyType type;
  ModeOfProduction mode_of_production;

  [[nodiscard]] uint32_t add_first_member(const Sex sex);
  [[nodiscard]] uint32_t add_child(const uint32_t parent_id, const Sex sex);
  [[nodiscard]] uint32_t add_son(const uint32_t parent_id);
  [[nodiscard]] uint32_t add_daughter(const uint32_t parent_id);
  [[nodiscard]] uint32_t add_spouse(const uint32_t member_id);
  [[nodiscard]] uint32_t add_father(const uint32_t child_id);
  [[nodiscard]] uint32_t add_mother(const uint32_t child_id);

  [[nodiscard]] std::shared_ptr<RelationshipNode> get_member(const uint32_t member_id) const
  {
    return m_relationship.at(member_id);
  }

  template <class Archive>
  void serialize(Archive& archive)
  {
    archive(id, name, age, predominance, type, mode_of_production);
  }

 private:
  RelationshipNodeContainer m_relationship;

  static uint32_t identifier() noexcept
  {
    static uint32_t value = 1;
    return value++;
  }
};
}  // namespace dl
