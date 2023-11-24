#include "./society_blueprint.hpp"

#include <spdlog/spdlog.h>

namespace dl
{
const uint32_t SocietyBlueprint::null_member = 0;

uint32_t SocietyBlueprint::add_first_member(const Sex sex)
{
  const auto identifier = SocietyBlueprint::identifier();
  auto member = std::make_shared<RelationshipNode>(identifier, sex);
  m_relationship[identifier] = member;

  return identifier;
}

uint32_t SocietyBlueprint::add_child(const uint32_t parent_id, const Sex sex)
{
  if (!m_relationship.contains(parent_id))
  {
    spdlog::critical("Society doesn't have a member with id '{}'", parent_id);
    return null_member;
  }

  const auto identifier = SocietyBlueprint::identifier();
  auto child = std::make_shared<RelationshipNode>(identifier, sex);
  auto& parent = m_relationship[parent_id];

  parent->children[identifier] = child;

  if (parent->sex == Sex::Female)
  {
    child->mother = parent;
  }
  else
  {
    child->father = parent;
  }

  if (parent->spouse != nullptr)
  {
    parent->spouse->children[identifier] = child;
  }

  m_relationship[identifier] = child;

  return identifier;
}

uint32_t SocietyBlueprint::add_son(const uint32_t parent_id) { return add_child(parent_id, Sex::Male); }

uint32_t SocietyBlueprint::add_daughter(const uint32_t parent_id) { return add_child(parent_id, Sex::Female); }

uint32_t SocietyBlueprint::add_spouse(const uint32_t member_id)
{
  if (!m_relationship.contains(member_id))
  {
    spdlog::critical("Society doesn't have a member with id '{}'", member_id);
    return null_member;
  }

  auto& member = m_relationship[member_id];

  if (member->spouse != nullptr)
  {
    spdlog::critical("Person with id '{}' already have an spouse with id '{}'", member_id, member->spouse->id);
    return null_member;
  }

  const auto identifier = SocietyBlueprint::identifier();
  std::shared_ptr<RelationshipNode> spouse;

  if (member->sex == Sex::Female)
  {
    spouse = std::make_shared<RelationshipNode>(identifier, Sex::Male);
  }
  else
  {
    spouse = std::make_shared<RelationshipNode>(identifier, Sex::Female);
  }

  member->spouse = spouse;
  spouse->spouse = member;

  if (!member->children.empty())
  {
    spouse->children = member->children;
  }

  m_relationship[identifier] = spouse;

  return identifier;
}

uint32_t SocietyBlueprint::add_father(const uint32_t child_id)
{
  if (!m_relationship.contains(child_id))
  {
    spdlog::critical("Society doesn't have a member with id '{}'", child_id);
    return null_member;
  }

  auto& member = m_relationship[child_id];

  if (member->father != nullptr)
  {
    spdlog::critical("Person with id '{}' already have a father with id '{}'", child_id, member->father->id);
    return null_member;
  }

  const auto identifier = SocietyBlueprint::identifier();
  auto father = std::make_shared<RelationshipNode>(identifier, Sex::Male);
  member->father = father;
  father->children[child_id] = member;

  m_relationship[identifier] = father;

  return identifier;
}

uint32_t SocietyBlueprint::add_mother(const uint32_t child_id)
{
  if (!m_relationship.contains(child_id))
  {
    spdlog::critical("Society doesn't have a member with id '{}'", child_id);
    return null_member;
  }

  auto& member = m_relationship[child_id];

  if (member->mother != nullptr)
  {
    spdlog::critical("Person with id '{}' already have a mother with id '{}'", child_id, member->mother->id);
    return null_member;
  }

  const auto identifier = SocietyBlueprint::identifier();
  auto mother = std::make_shared<RelationshipNode>(identifier, Sex::Female);
  member->mother = mother;
  mother->children[child_id] = member;

  m_relationship[identifier] = mother;

  return identifier;
}

}  // namespace dl
