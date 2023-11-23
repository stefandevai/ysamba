#include "./family.hpp"

#include <spdlog/spdlog.h>

namespace dl
{
const uint32_t Family::null_member = 0;

uint32_t Family::add_first_member(const Sex sex)
{
  const auto identifier = Family::identifier();
  auto member = std::make_shared<FamilyMember>(identifier, sex);
  m_family[identifier] = member;

  return identifier;
}

uint32_t Family::add_child(const uint32_t parent_id, const Sex sex)
{
  if (!m_family.contains(parent_id))
  {
    spdlog::critical("Family doesn't have a member with id '{}'", parent_id);
    return null_member;
  }

  const auto identifier = Family::identifier();
  auto child = std::make_shared<FamilyMember>(identifier, sex);
  auto& parent = m_family[parent_id];

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

  return identifier;
}

uint32_t Family::add_son(const uint32_t parent_id) { return add_child(parent_id, Sex::Male); }

uint32_t Family::add_daughter(const uint32_t parent_id) { return add_child(parent_id, Sex::Female); }

uint32_t Family::add_spouse(const uint32_t person_id)
{
  if (!m_family.contains(person_id))
  {
    spdlog::critical("Family doesn't have a member with id '{}'", person_id);
    return null_member;
  }

  auto& person = m_family[person_id];

  if (person->spouse != nullptr)
  {
    spdlog::critical("Person with id '{}' already have an spouse with id '{}'", person_id, person->spouse->id);
    return null_member;
  }

  const auto identifier = Family::identifier();
  std::shared_ptr<FamilyMember> spouse;

  if (person->sex == Sex::Female)
  {
    spouse = std::make_shared<FamilyMember>(identifier, Sex::Male);
  }
  else
  {
    spouse = std::make_shared<FamilyMember>(identifier, Sex::Female);
  }

  person->spouse = spouse;
  spouse->spouse = person;

  if (!person->children.empty())
  {
    spouse->children = person->children;
  }

  return identifier;
}

uint32_t Family::add_father(const uint32_t child_id)
{
  if (!m_family.contains(child_id))
  {
    spdlog::critical("Family doesn't have a member with id '{}'", child_id);
    return null_member;
  }

  auto& person = m_family[child_id];

  if (person->father != nullptr)
  {
    spdlog::critical("Person with id '{}' already have a father with id '{}'", child_id, person->father->id);
    return null_member;
  }

  const auto identifier = Family::identifier();
  auto father = std::make_shared<FamilyMember>(identifier, Sex::Male);
  person->father = father;
  father->children[child_id] = person;

  return identifier;
}

uint32_t Family::add_mother(const uint32_t child_id)
{
  if (!m_family.contains(child_id))
  {
    spdlog::critical("Family doesn't have a member with id '{}'", child_id);
    return null_member;
  }

  auto& person = m_family[child_id];

  if (person->mother != nullptr)
  {
    spdlog::critical("Person with id '{}' already have a mother with id '{}'", child_id, person->mother->id);
    return null_member;
  }

  const auto identifier = Family::identifier();
  auto mother = std::make_shared<FamilyMember>(identifier, Sex::Female);
  person->mother = mother;
  mother->children[child_id] = person;

  return identifier;
}

}  // namespace dl
