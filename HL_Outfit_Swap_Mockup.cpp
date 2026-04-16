</> C++ mockup implementation of an 'outfit' command for Harshlands MUD game, allowing players to save and manage sets of equipped items as 'outfits'.

//NOTE: This command is not intended to be used by builders or immortals, so it does not have any special checks for those roles. It is purely a player convenience command.
//This is a conceptual mockup and may require adjustments to fit into the actual codebase, especially regarding data structures and utility functions.
//Some types and functions (like CHAR_DATA, OBJ_DATA, get_obj_in_list_vis, equip_char, unequip_char, etc.) are assumed to be defined elsewhere in the codebase.
//will differ from the actual implementation based on how the rest of the codebase is structured, especially regarding how characters, objects, and commands are handled.

void do_outfit(CHAR_DATA *character, char *argument);

ACMD(do_outfit)
{
    CHAR_DATA *ch = character;
    char cmd[MAX_INPUT_LENGTH];
    char name[MAX_INPUT_LENGTH];
    char container_name[MAX_INPUT_LENGTH];
    OBJ_DATA *obj = 0;
    OBJ_DATA *worn = 0;
    OBJ_DATA *container = 0;

    argument = one_argument(argument, cmd);
    argument = one_argument(argument, name);
    argument = one_argument(argument, container_name);

    if (!*cmd) {
        ch->sendln("Syntax:");
        ch->sendln("  outfit list");
        ch->sendln("  outfit show <name>");
        ch->sendln("  outfit save <name> [container]");
        ch->sendln("  outfit wear <name> [container]");
        ch->sendln("  outfit delete <name>");
        return;
    }

    if (!str_cmp(cmd, "list")) {
        if (ch->pcdata->outfits.empty()) {
            ch->sendln("You have no saved outfits.");
            return;
        }

        ch->sendln("Saved outfits:");
        for (OutfitMap::iterator it = ch->pcdata->outfits.begin(); it != ch->pcdata->outfits.end(); ++it)
            ch->sendf("  %s (%zu pieces)\r\n", it->second.name.c_str(), it->second.pieces.size());
        return;
    }

    if (!str_cmp(cmd, "show")) {
        if (!*name) {
            ch->sendln("Show which outfit?");
            return;
        }

        OutfitMap::iterator it = ch->pcdata->outfits.find(name);
        if (it == ch->pcdata->outfits.end()) {
            ch->sendln("You have no outfit by that name.");
            return;
        }

        ch->sendf("Outfit '%s':\r\n", it->second.name.c_str());
        if (!it->second.preferred_container.empty())
            ch->sendf("  Container: %s\r\n", it->second.preferred_container.c_str());

        for (size_t i = 0; i < it->second.pieces.size(); ++i)
            ch->sendf("  [%d] %s\r\n",
                      it->second.pieces[i].wear_loc,
                      it->second.pieces[i].item_keyword.c_str());
        return;
    }

    if (!str_cmp(cmd, "delete")) {
        if (!*name) {
            ch->sendln("Delete which outfit?");
            return;
        }

        OutfitMap::iterator it = ch->pcdata->outfits.find(name);
        if (it == ch->pcdata->outfits.end()) {
            ch->sendln("You have no outfit by that name.");
            return;
        }

        ch->pcdata->outfits.erase(it);
        ch->sendf("You delete the outfit '%s'.\r\n", name);
        return;
    }

    if (!str_cmp(cmd, "save")) {
        if (!*name) {
            ch->sendln("Save which outfit?");
            return;
        }

        OutfitSet set;
        set.name = name;
        set.preferred_container = container_name;

        for (obj = ch->carrying; obj; obj = obj->next_content) {
            if (obj->wear_loc == WEAR_NONE)
                continue;

            OutfitPiece piece;
            piece.wear_loc = obj->wear_loc;
            piece.item_keyword = fname(obj->name);
            set.pieces.push_back(piece);
        }

        ch->pcdata->outfits[name] = set;
        ch->sendf("You save your current attire as outfit '%s'.\r\n", name);
        return;
    }

    if (!str_cmp(cmd, "wear")) {
        if (!*name) {
            ch->sendln("Wear which outfit?");
            return;
        }

        OutfitMap::iterator it = ch->pcdata->outfits.find(name);
        if (it == ch->pcdata->outfits.end()) {
            ch->sendln("You have no outfit by that name.");
            return;
        }

        OutfitSet &set = it->second;

        if (*container_name)
            container = get_obj_in_list_vis(ch, container_name, ch->carrying);
        if (!container && *container_name)
            container = get_obj_in_list_vis(ch, container_name, ch->in_room->contents);
        if (!container && !set.preferred_container.empty())
            container = get_obj_in_list_vis(ch, set.preferred_container.c_str(), ch->carrying);
        if (!container && !set.preferred_container.empty())
            container = get_obj_in_list_vis(ch, set.preferred_container.c_str(), ch->in_room->contents);

        int success = 0;
        int failed = 0;

        Utils::act("$n begins changing attire.", ACT_TO_ROOM, ch, 0, 0);
        Utils::act("You begin changing into your saved outfit.", ACT_TO_CHAR, ch, 0, 0);

        for (size_t i = 0; i < set.pieces.size(); ++i) {
            const OutfitPiece &piece = set.pieces[i];
            worn = 0;

            for (obj = ch->carrying; obj; obj = obj->next_content) {
                if (obj->wear_loc == piece.wear_loc) {
                    worn = obj;
                    break;
                }
            }

            if (worn && isname(piece.item_keyword.c_str(), worn->name)) {
                success++;
                continue;
            }

            if (worn) {
                if (!unequip_char(ch, worn)) {
                    Utils::act("You cannot remove $p.", ACT_TO_CHAR, ch, worn, 0);
                    failed++;
                    continue;
                }

                if (container) {
                    if (!obj_to_obj(worn, container)) {
                        obj_to_char(worn, ch);
                    } else {
                        Utils::act("You place $p into $P.", ACT_TO_CHAR, ch, worn, container);
                    }
                } else {
                    obj_to_char(worn, ch);
                }
            }

            OBJ_DATA *target = 0;

            if (container) {
                for (obj = container->contains; obj; obj = obj->next_content) {
                    if (isname(piece.item_keyword.c_str(), obj->name)) {
                        target = obj;
                        break;
                    }
                }
            }

            if (!target) {
                for (obj = ch->carrying; obj; obj = obj->next_content) {
                    if (obj->wear_loc == WEAR_NONE && isname(piece.item_keyword.c_str(), obj->name)) {
                        target = obj;
                        break;
                    }
                }
            }

            if (!target) {
                ch->sendf("You cannot find '%s'.\r\n", piece.item_keyword.c_str());
                failed++;
                continue;
            }

            if (target->in_obj) {
                obj_from_obj(target);
                obj_to_char(target, ch);
            }

            if (!equip_char(ch, target, piece.wear_loc)) {
                Utils::act("You cannot wear $p.", ACT_TO_CHAR, ch, target, 0);
                failed++;
                continue;
            }

            success++;
        }

        if (!failed) {
            ch->sendf("You finish changing into '%s'.\r\n", set.name.c_str());
            Utils::act("$n finishes changing attire.", ACT_TO_ROOM, ch, 0, 0);
        } else {
            ch->sendf("You finish changing into '%s' with some difficulty (%d succeeded, %d failed).\r\n",
                      set.name.c_str(), success, failed);
            Utils::act("$n finishes changing attire, though not without some difficulty.", ACT_TO_ROOM, ch, 0, 0);
        }

        return;
    }

    ch->sendln("Unknown outfit option.");
}