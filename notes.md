# World creation workflow
1. Generate terrain
2. Generate societies
3. Let player choose a predefined society
4. Let player choose starting location

# Society types
They change according to the geographical location
- Savants (a lot of old people)
- Warriors

Nomadic - worse equipment at start but better skills overall
Sedentary - better equipment but worse skills

# Gameplay
- Player's society can conquer others by force or by diplomacy
- Nearby societies have a more similar language and culture than those far away
- Investigate dummy language generation

# Gameplay loop
1. World creation
2. Society creation
3. Society management
  - Enslave someone
  - Free someone
  - Gather wood
  - Gather food
  - Gather water
  - Hunt
  - Cook
  - Build tools
  - Dig pit
  - Light fire
  - Resources dry out
  - Move to another location (nomadic)
  - Prepare soil to plant
4. Automate routine actions:
  - 1x day, 1x week, 1x month
5. Society can collapse if not well structured
  - Too much working hours
  - Not enough cohersive power (weapons strength)
  - Illegitimacy of rulers
  - Quantity of slaves
6. Increase of number of ppl leads to inventions of new technology
7. Collapse leads to spending materials, loosing ppl and modifying the society towards an equilibrium

TODO:
  - Consider cases when coord is on negative x or y

# Game loop
- Start as a nomad society
- Build stuff and eat while things start to run out
- You have to move to another location, you can't take everything
- Locations are pretty different
- Possibility of finding other societies that are hostile or friendly
- Each new location you can accumulate stuff until you're able to settle
- If someone dies you can bury them
- The game ends when all the members of your society died

- When you settle, you can farm and tame animals
- You can craft more advanced things
- You can slave or befriend people from other societies
- Sometimes people from other societies come to attack you

## Animals
- They can approach and attack
- Can be scared
- Can attack without killing

## Different ppl
- Old people are knowledgable but weak
- Children and pregnant women need protection
- Skills improve automatically when doing jobs and faster than in other games

## Night
- Some society memebers must remain awake during night and sleep in the morning

## Key points
- Gathering material is really hard, there are low quantities and recipes require a lot of things
- Effects of hunger and thirst are harsh in order to force the player to migrate

## Pieces of UI
- Society inventory:
  - Some things are available in place
  - But you can't carry everything when you migrate
- Society overview
  - Details about each member

## Ideas
- Procedural religions
- Rituals
- Be able to draw paintings on the cave walls
- Draw patterns in stone
- Pick magic mushrooms
- Danse
- Big fireplace
- Be able to jump and break branches in half
- Carry animal body on the back
- Use multiple ppl to carry heavy stuff
- Hunt with multiple ppl
- Scary an animal that is eating another dead animal to take the food
- Honey
- Weird rock with spiral shapes (fossil)
- Learn recipies with other societies

# Inventory
- CarriedItems and WearedItems components to attach to people
- They contain a vector of entt::entities
- Carried and Weared components added to the items
- The pickup system will use item data (where can it be weared or carried?), and slot data (what kind of body parts the creature has to carry or wear?),
  and person data (does the person have all the arms?) to evaluate if an item can be carried or weared

- hammer_entity
  - item_component
    - id: hammer_id
  - stored_component
    - container_entity: bag_entity

- bag_entity
  - item_component
    - id: bag_id
  - storage_component
    - items: [hammer_entity, ...]
    - weight_capacity: 10kg
    - current_weight: 1kg

- hat_entity
  - item_component
    - id: hat_id
  - wearable_component
    - slots: [head_slot_id]
  - weared_component
    - slot_id: head_slot_id

- sword_entity
  - item_component
    - id: sword_id
  - wielded_component
    - slot_id: right_hand_id

- person_entity
  - biology_component
    - body_parts: [left_hand_id, torso_id, leg_id, ...]
    - wield_slots: [left_hand_id, right_hand_id, ...],
  - wielded_items:
    - items: [sword_entity]
  - weared_items
    - items: [hat_entity, bag_entity, ...]
  - carried_items
    - items: [hammer_entity, ...]

slots:
[
  {
    "id": 2,
    "name": "left hand"
    "can_wield": true,
  },
  {
    "id": 3,
    "name": "torso"
    "can_wield": false,
  },
  ...
]

items:
[
  {
    "id": 0,
    "name": "hat"
    "flags": ["PICKABLE", "WIELDABLE"],
    "wearable_slots": [2, ...],
  },
  {
    "id": 1,
    "name": "sword"
    "flags": ["PICKABLE", "WIELDABLE", "TWO_HANDED"],
  },
  ...
]

### Wear system
1. Get item entity and agent entity
2. Get Item component and Biology component
3. Check item_data.wearable_parts if it's wearable and if biology has corresponding body parts that match the wearable parts
4. Wear item and compute encumbrance for body part

### Pickup system
1. Get item entity and agent entity
2. Check agent entity WearedItems component and get those with Storage component
  - if current_weight lt weight_capacity ? push_item : get child container items
  - Push item to CarriedItems component or StorageComponent or both?
  - Decision: push to StorageComponent only for simplicity, CarriedItems look like a cache and can be implemented later
3. Add item weight to total weight in biology component

### Wield system
1. Get item entity and agent entity
2. Check if item has WIELDABLE flag
3. Check if agent has free wield slots
4. Add wielded entt::tag to item entity

## Society inventory
- Define a storage area and all items in this area appear in the society inventory
- Also get all Selectable, CarriedItems and WearedItems components to be able to display all items


