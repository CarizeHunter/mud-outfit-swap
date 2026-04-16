# Harshlands Outfit Command (Concept)

A quality-of-life command concept for Harshlands (or similar MUD codebases) that allows players to save and quickly swap between clothing sets.

---

## 🧠 Overview

Changing outfits in most MUDs requires multiple repetitive commands per item:

- remove cloak  
- get cloak  
- wear cloak  
- put cloak container  

While functional, this creates enough friction that players often avoid changing attire altogether-even in roleplay-heavy environments.

This concept introduces a simple **outfit system** to streamline that process.

---

## ⚙️ Core Commands
### Examples

---

## 🎭 Intended Behavior

### `outfit save <name> [container]`
- Saves currently worn items as a named outfit
- Optionally stores a preferred container for item storage/retrieval

### `outfit wear <name> [container]`
- Automatically swaps into the saved outfit
- Removes conflicting worn items
- Stores removed items in container or inventory
- Retrieves outfit items from container or inventory
- Continues even if some items are missing

### `outfit list`
- Displays all saved outfits

### `outfit show <name>`
- Displays items in a saved outfit

### `outfit delete <name>`
- Removes the saved outfit reference (not the items)

---

## ✨ Why This Matters

Encourages active use of:

- Travel vs formal clothing
- Seasonal attire
- Disguises / incognito play
- Religious or faction identity
- Armor vs civilian gear
- Social / RP-driven wardrobe changes

Reduces command spam and increases immersion.

---

## 🧩 Implementation Notes

This is a **conceptual mockup** based on observed MUD command patterns.

The implementation assumes:

- A command macro like `ACMD`
- Standard MUD structures (`CHAR_DATA`, `OBJ_DATA`)
- Wear slots (`wear_loc`)
- Inventory and container handling functions

Saved outfit pieces include:
- Wear location
- Item keyword (basic identifier/VNUM)

A production version would ideally include:
- VNUM tracking
- Persistent item identifiers
- More robust matching logic

---

## ⚠️ Disclaimer

This code is **not plug-and-play**

It is a conceptual implementation intended to demonstrate:
- Command flow
- Expected behavior
- Integration style within a typical MUD codebase

Some functions and types (e.g., `CHAR_DATA`, `OBJ_DATA`, `ACMD`, etc.) are assumed to exist in the target codebase.

---

## 🧪 Example Output

This would:
- Remove currently worn item in the slot
- Wear the replacement
- Optionally store the removed item

---

## 💬 Final Thoughts

This feature focuses on improving **player behavior**, not just convenience.

By lowering friction, it encourages:
- More expressive roleplay
- Greater use of clothing systems
- More dynamic character presentation

---

## 👤 Author Carize

Concept and mockup by a Harshlands player exploring quality-of-life improvements for RP system.
