/*
* ScriptData
* SDName:
* SD%Complete: 100
* SDComment:
* EndScriptData
*/

#include "AI/ScriptDevAI/ScriptDevAIMgr.h"
#include "AI/ScriptDevAI/include/sc_gossip.h"
#include "AI/ScriptDevAI/include/sc_common.h"
#include "Globals/ObjectMgr.h"
#include "GameEvents/GameEventMgr.h"
#include "World/WorldState.h"

enum
{
    SPELL_APPRENTICE_RIDING     = 33389,
    SPELL_JOURNEYMAN_RIDING     = 33392,
    SPELL_ARTISAN_RIDING        = 34093,
    SPELL_ARTISAN_FIRST_AID     = 10847,
    SPELL_MASTER_FIRST_AID      = 27029,
    SPELL_HEAVY_RUNECLOTH_BAND  = 18632,
    SPELL_HEAVY_NETHER_BAND     = 27033,
    SPELL_TELEPORT_VISUAL       = 41236,

    GOSSIP_MENU_MAIN            = 50200, // also contains other unscripted options - Contact Us, Donating, Reporting Bugs, Cheating Policy, etc.
    GOSSIP_MENU_BOOST_CONFIRM   = 50402,
    GOSSIP_MENU_CONTACT         = 50205,
    GOSSIP_MENU_WHAT_IS_LV      = 50201,
    GOSSIP_MENU_DONATING        = 50202,
    GOSSIP_MENU_BUG_REPORT      = 50203,
    GOSSIP_MENU_CHEATING        = 50204,

    GOSSIP_MENU_OVERLORD_MAIN   = 50414,

    GAME_EVENT_INSTANT_58       = 211,

    NPC_TEST_REALM_OVERLORD     = 101000
};

const std::vector<uint32> Level1StartingGear = { 25,35,36,37,39,40,43,44,47,48,51,52,55,56,57,59,117,120,121,129,139,140,147,153,159,1395,1396,2070,2092,2101,2102,2361,2362,2504,2508,2512,2516,3661,4536,4540,4604,6098,6116,6118,6119,6121,6122,6123,6124,6126,6127,6129,6135,6137,6138,6139,6140,6144,12282,20857,20891,20892,20893,20894,20895,20896,20898,20899,20900,20980,20981,20982,20983,23322,23344,23346,23347,23348,23474,23475,23477,23478,23479,24145,24146,25861,28979 };

struct CustomTeleportLocation
{
    uint32 map;
    float x, y, z, o;
    uint32 area;
};
static const CustomTeleportLocation teleLocs[] =
{
    {0, -11792.108398f, -3226.608154f, -29.721224f, 2.613495f, 0},  // Dark Portal - Alliance
    {0, -11774.694336f, -3184.537598f, -28.923182f, 2.749808f, 0},  // Dark Portal - Horde
    {530, -1982.2641f, 5073.6987f, 7.388187f, 1.227539f, 3703},     // Shattrath City - Alliance
    {530, -1992.693726f, 5078.106934f, 7.165706f, 0.944797f, 3703}, // Shattrath City - Horde

    {0, -8931.93f, -132.83f, 82.88f, 3.26f, 0},                     // Northshire (Human)
    {0, -6213.47f, 330.64f, 383.68f, 3.15f, 0},                     // Coldridge Valley (Dwarf and Gnome)
    {1, 10317.40f, 821.00f, 1326.37f, 2.15f, 0},                    // Shadowglen (Night Elf)
    {530, -3983.02f, -13898.48f, 96.30f, 5.32f, 0},                 // Ammen Vale (Draenei)

    {1, -607.47f, -4248.13f, 38.95f, 3.27f, 0},                     // Valley of Trials (Orc and Troll)
    {0, 1656.367f, 1682.592f, 120.78681f, 0.06632f, 0},             // Deathknell (Undead)
    {1, -2913.71f, -254.67f, 52.94f, 3.70f, 0},                     // Camp Narache (Tauren)
    {530, 10354.33f, -6370.34f, 36.04f, 1.92f, 0},                  // Sunstrider Isle (Blood Elf)
};

struct npc_vengeance_greeterAI : public ScriptedAI
{
    npc_vengeance_greeterAI(Creature* creature) : ScriptedAI(creature) { Reset(); }

    void Reset() override
    {
    }

    void UpdateAI(const uint32 diff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->GetVictim())
            return;

        DoMeleeAttackIfReady();
    }

    uint32 GetStarterMountForRace(Player* player)
    {
        uint32 mountEntry = 0;
        switch (player->getRace())
        {
        case RACE_HUMAN: mountEntry = 2411; break;      // Black Stallion Bridle
        case RACE_DWARF: mountEntry = 5872; break;      // Brown Ram
        case RACE_NIGHTELF: mountEntry = 8632; break;   // Reins of the Spotted Frostsaber
        case RACE_GNOME: mountEntry = 8595; break;      // Blue Mechanostrider
        case RACE_DRAENEI: mountEntry = 29744; break;   // Gray Elekk
        case RACE_ORC: mountEntry = 5665; break;        // Horn of the Dire Wolf
        case RACE_UNDEAD: mountEntry = 13331; break;    // Red Skeletal Horse
        case RACE_TAUREN: mountEntry = 15277; break;    // Gray Kodo
        case RACE_TROLL: mountEntry = 8588; break;      // Whistle of the Emerald Raptor
        case RACE_BLOODELF: mountEntry = 29221; break;  // Black Hawkstrider
        }
        return mountEntry;
    }

    uint32 GetStarterEpicMountForRace(Player* player)
    {
        uint32 mountEntry = 0;
        switch (player->getRace())
        {
        case RACE_HUMAN: mountEntry = 18776; break;     // Swift Palomino
        case RACE_DWARF: mountEntry = 18787; break;     // Swift Gray Ram
        case RACE_NIGHTELF: mountEntry = 18767; break;  // Reins of the Swift Mistsaber
        case RACE_GNOME: mountEntry = 18772; break;     // Swift Green Mechanostrider
        case RACE_DRAENEI: mountEntry = 29745; break;   // Great Blue Elekk
        case RACE_ORC: mountEntry = 18797; break;       // Horn of the Swift Timber Wolf 
        case RACE_UNDEAD: mountEntry = 18791; break;    // Purple Skeletal Warhorse
        case RACE_TAUREN: mountEntry = 18795; break;    // Great Gray Kodo
        case RACE_TROLL: mountEntry = 18790; break;     // Swift Orange Raptor
        case RACE_BLOODELF: mountEntry = 28927; break;  // Red Hawkstrider
        }
        return mountEntry;
    }

    void BoostPlayer(Player* player, uint32 targetLevel)
    {
        if (player->getLevel() < targetLevel)
        {
            player->GiveLevel(targetLevel);
            player->SetUInt32Value(PLAYER_XP, 0);
            player->learnClassLevelSpells(false);
            player->UpdateSkillsForLevel(true);

            if (player->getClass() == CLASS_HUNTER)
            {
                if (Pet* pet = player->GetPet())
                {
                    if (pet->getLevel() < targetLevel)
                    {
                        pet->GivePetLevel(targetLevel);
                        pet->SavePetToDB(PET_SAVE_AS_CURRENT, player);
                    }
                }
            }
        }

        // Learn skills
        player->CastSpell(player, SPELL_APPRENTICE_RIDING, TRIGGERED_OLD_TRIGGERED);
        player->CastSpell(player, SPELL_ARTISAN_FIRST_AID, TRIGGERED_OLD_TRIGGERED);
        player->CastSpell(player, SPELL_HEAVY_RUNECLOTH_BAND, TRIGGERED_OLD_TRIGGERED);
        switch (targetLevel) {
        case 60:
            player->CastSpell(player, SPELL_JOURNEYMAN_RIDING, TRIGGERED_OLD_TRIGGERED);
            break;
        case 70:
            player->CastSpell(player, SPELL_ARTISAN_RIDING, TRIGGERED_OLD_TRIGGERED);
            player->CastSpell(player, SPELL_MASTER_FIRST_AID, TRIGGERED_OLD_TRIGGERED);
            player->learnSpell(SPELL_HEAVY_NETHER_BAND, false);
            break;
        }
        SkillLineEntry const* sl = sSkillLineStore.LookupEntry(SKILL_FIRST_AID);
        if (sl)
        {
            uint32 maxSkill = player->GetSkillMaxPure(SKILL_FIRST_AID);

            if (player->GetSkillValue(SKILL_FIRST_AID) < maxSkill)
                player->SetSkill(SKILL_FIRST_AID, maxSkill, maxSkill);
        }

        // Remove any gear the character still has from initial creation (now useless)
        for (uint32 itemEntry : Level1StartingGear)
            player->DestroyItemCount(itemEntry, 200, true, false);

        if (targetLevel == 58) {
            // LV Starter Bag (8 Slot) x4
            if (!player->HasItemCount(2115, 4)) player->StoreNewItemInBestSlots(2115, 4);

            // Ground Mount
            uint32 groundMount = GetStarterMountForRace(player);
            if (!player->HasItemCount(groundMount, 1)) player->StoreNewItemInBestSlots(groundMount, 1);
        }
        else {
            // Onyxia Hide Backpack x4
            if (!player->HasItemCount(17966, 4)) player->StoreNewItemInBestSlots(17966, 4);

            // Epic Ground Mount
            uint32 groundMount = GetStarterEpicMountForRace(player);
            if (!player->HasItemCount(groundMount, 1)) player->StoreNewItemInBestSlots(groundMount, 1);

            // Flying Mount - Swift Blue Gryphon / Swift Red Wind Rider
            uint32 flyingMount = player->GetTeam() == ALLIANCE ? 25473 : 25477;
            if (targetLevel == 70 && !player->HasItemCount(flyingMount, 1)) player->StoreNewItemInBestSlots(flyingMount, 1);
        }

        player->SaveToDB();
    }
};

bool GossipHello_npc_vengeance_greeter(Player* player, Creature* creature)
{
    if (creature->GetEntry() == NPC_TEST_REALM_OVERLORD)
    {
        player->PrepareGossipMenu(creature, GOSSIP_MENU_OVERLORD_MAIN);
        if (player->getLevel() < 60)
            player->ADD_GOSSIP_ITEM_EXTENDED(GOSSIP_ICON_INTERACT_1, "|cFF00008BBoost to level 60|r", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 3, "Are you sure?", 0, false);
        else {
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TAXI, "Teleport to Shattrath City", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 5);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TAXI, "Teleport to Blasted Lands (Dark Portal)", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 6);
        }
        if (player->getLevel() < 70)
            player->ADD_GOSSIP_ITEM_EXTENDED(GOSSIP_ICON_INTERACT_1, "|cFF006400Boost to level 70|r", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 4, "Are you sure?", 0, false);
    }
    else
    {
        player->PrepareGossipMenu(creature, GOSSIP_MENU_MAIN);
        uint32 minLevel = 1;
        if (sGameEventMgr.IsActiveEvent(GAME_EVENT_INSTANT_58))
        {
            if (sGameEventMgr.IsActiveEvent(GAME_EVENT_I58_RESTRICTION_TBC_RACES) && (player->getRace() == RACE_DRAENEI || player->getRace() == RACE_BLOODELF))
                minLevel = 20;
            if (sGameEventMgr.IsActiveEvent(GAME_EVENT_I58_RESTRICTION_ALLIANCE_RACES) && player->GetTeam() == ALLIANCE)
                minLevel = 20;
            if (sGameEventMgr.IsActiveEvent(GAME_EVENT_I58_RESTRICTION_HORDE_RACES) && player->GetTeam() == HORDE)
                minLevel = 20;

            if (player->getLevel() >= minLevel && player->getLevel() <= 57)
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "I want to go straight to Outland. |cFF006400[INSTANT LEVEL 58]|r", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
        }
    }

    player->SendPreparedGossip(creature);
    return true;
}

bool GossipSelect_npc_vengeance_greeter(Player* player, Creature* creature, uint32 /*sender*/, uint32 action)
{
    if (npc_vengeance_greeterAI* vengeanceGreeterAI = dynamic_cast<npc_vengeance_greeterAI*>(creature->AI()))
    {
        switch (action)
        {
        case 100:
            player->PrepareGossipMenu(creature, GOSSIP_MENU_CONTACT);
            player->SendPreparedGossip(creature);
            break;
        case 101:
            player->PrepareGossipMenu(creature, GOSSIP_MENU_WHAT_IS_LV);
            player->SendPreparedGossip(creature);
            break;
        case 102:
            player->PrepareGossipMenu(creature, GOSSIP_MENU_DONATING);
            player->SendPreparedGossip(creature);
            break;
        case 103:
            player->PrepareGossipMenu(creature, GOSSIP_MENU_BUG_REPORT);
            player->SendPreparedGossip(creature);
            break;
        case 104:
            player->PrepareGossipMenu(creature, GOSSIP_MENU_CHEATING);
            player->SendPreparedGossip(creature);
            break;
        case GOSSIP_ACTION_INFO_DEF + 1:
            player->PrepareGossipMenu(creature, GOSSIP_MENU_BOOST_CONFIRM);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "I'm ready. Take me to the front! |cFF006400[INSTANT LEVEL 58]|r", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
            player->SendPreparedGossip(creature);
            break;
        case GOSSIP_ACTION_INFO_DEF + 2:
        {
            player->CLOSE_GOSSIP_MENU();
            vengeanceGreeterAI->BoostPlayer(player, 58);
            player->CastSpell(player, SPELL_TELEPORT_VISUAL, TRIGGERED_OLD_TRIGGERED);
            // Teleport Player to Dark Portal
            if (player->GetTeam() == ALLIANCE)
                player->TeleportTo(teleLocs[0].map, teleLocs[0].x, teleLocs[0].y, teleLocs[0].z, teleLocs[0].o);
            else
                player->TeleportTo(teleLocs[1].map, teleLocs[1].x, teleLocs[1].y, teleLocs[1].z, teleLocs[1].o);
            break;
        }
        case GOSSIP_ACTION_INFO_DEF + 3:
        {
            player->CLOSE_GOSSIP_MENU();
            vengeanceGreeterAI->BoostPlayer(player, 60);
            player->CastSpell(player, SPELL_TELEPORT_VISUAL, TRIGGERED_OLD_TRIGGERED);
            // Teleport Player to Dark Portal
            if (player->GetTeam() == ALLIANCE)
                player->TeleportTo(teleLocs[0].map, teleLocs[0].x, teleLocs[0].y, teleLocs[0].z, teleLocs[0].o);
            else
                player->TeleportTo(teleLocs[1].map, teleLocs[1].x, teleLocs[1].y, teleLocs[1].z, teleLocs[1].o);
            break;
        }
        case GOSSIP_ACTION_INFO_DEF + 4:
        {
            player->CLOSE_GOSSIP_MENU();
            vengeanceGreeterAI->BoostPlayer(player, 70);
            player->CastSpell(player, SPELL_TELEPORT_VISUAL, TRIGGERED_OLD_TRIGGERED);
            // Teleport Player To Shattrath City
            if (player->GetTeam() == ALLIANCE)
            {
                player->TeleportTo(teleLocs[2].map, teleLocs[2].x, teleLocs[2].y, teleLocs[2].z, teleLocs[2].o);
                player->SetHomebindToLocation(WorldLocation(teleLocs[2].map, teleLocs[2].x, teleLocs[2].y, teleLocs[2].z, teleLocs[2].o), teleLocs[2].area);
            }
            else
            {
                player->TeleportTo(teleLocs[3].map, teleLocs[3].x, teleLocs[3].y, teleLocs[3].z, teleLocs[3].o);
                player->SetHomebindToLocation(WorldLocation(teleLocs[3].map, teleLocs[3].x, teleLocs[3].y, teleLocs[3].z, teleLocs[3].o), teleLocs[3].area);
            }
            break;
        }
        // Teleport Only - Shattrath City
        case GOSSIP_ACTION_INFO_DEF + 5:
        {
            player->CLOSE_GOSSIP_MENU();
            player->CastSpell(player, SPELL_TELEPORT_VISUAL, TRIGGERED_OLD_TRIGGERED);
            // Teleport Player To Shattrath City
            if (player->GetTeam() == ALLIANCE)
                player->TeleportTo(teleLocs[2].map, teleLocs[2].x, teleLocs[2].y, teleLocs[2].z, teleLocs[2].o);
            else
                player->TeleportTo(teleLocs[3].map, teleLocs[3].x, teleLocs[3].y, teleLocs[3].z, teleLocs[3].o);
            break;
        }
        // Teleport Only - Dark Portal
        case GOSSIP_ACTION_INFO_DEF + 6:
        {
            player->CLOSE_GOSSIP_MENU();
            player->CastSpell(player, SPELL_TELEPORT_VISUAL, TRIGGERED_OLD_TRIGGERED);
            // Teleport Player to Dark Portal
            if (player->GetTeam() == ALLIANCE)
                player->TeleportTo(teleLocs[0].map, teleLocs[0].x, teleLocs[0].y, teleLocs[0].z, teleLocs[0].o);
            else
                player->TeleportTo(teleLocs[1].map, teleLocs[1].x, teleLocs[1].y, teleLocs[1].z, teleLocs[1].o);
            break;
        }
        }
    }

    return false;
}

enum
{
    SPELL_CYCLONE_VISUAL_SPAWN      = 8609,

    GOSSIP_TEXT_ENLIST_GREET        = 50403,
    GOSSIP_TEXT_ENLIST_REFUSE_LOW   = 50404,
    GOSSIP_TEXT_ENLIST_REFUSE_HIGH  = 50405,
    GOSSIP_TEXT_ENLIST_PET_MENU     = 50406,
    GOSSIP_TEXT_GREET_70            = 50407,
    GOSSIP_TEXT_REFUSE_LOW_70       = 50408,
    GOSSIP_TEXT_PICK_SPEC           = 50409,
    GOSSIP_TEXT_PICK_GEAR_1         = 50411,
    GOSSIP_TEXT_PICK_GEAR_2         = 50412,
    GOSSIP_TEXT_PICK_GEAR_3         = 50413,

    SAY_GREET       = -1800999,
    SAY_GREET_70    = -1800998,

    SET_ID_PRIMARY      = 0,
    SET_ID_SECONDARY    = 1,
    SET_ID_TERTIARY     = 2,
    SET_ID_INSTANT_58   = 3,

    NPC_ALLIANCE_OFFICER    = 100501, // instant 58
    NPC_HORDE_OFFICER       = 100502, // instant 58
    NPC_MASTER_PROVISIONER  = 100503, // instant 70

    NPC_PET_BAT     = 8600, // Plaguebat
    NPC_PET_BEAR    = 7443, // Shardtooth Mauler
    NPC_PET_BOAR    = 5992, // Ashmane Boar
    NPC_PET_CARRION = 1809, // Carrion Vulture
    NPC_PET_CAT     = 1713, // Elder Shadowmaw Panther
    NPC_PET_CRAB    = 1088, // Monstrous Crawler
    NPC_PET_CROC    = 1087, // Sawtooth Snapper
    NPC_PET_GORILLA = 6516, // Un'Goro Thunderer
    NPC_PET_HYENA   = 5986, // Rabid Snickerfang
    NPC_PET_OWL     = 7455, // Winterspring Owl
    NPC_PET_RAPTOR  = 687,  // Jungle Stalker
    NPC_PET_RAVAGER = 19349,// Thornfang Ravager
    NPC_PET_STRIDER = 4725, // Crazed Sandstrider
    NPC_PET_SCORPID = 9695, // Deathlash Scorpid
    NPC_PET_SPIDER  = 8933, // Cave Creeper
    NPC_PET_SERPENT = 5308, // Rogue Vale Screecher
    NPC_PET_TURTLE  = 6369, // Coralshell Tortoise
    NPC_PET_WOLF    = 9697  // Giant Ember Worg

    // different list of pets for 70?
};

// Instant 70
//                                                          head    shoulders   chest   hands   legs
const std::vector<uint32> PlatePrimaryGear              = { 12423,  12407,      15141,  14694,  32171 };
//                                                          head    shoulders   chest   hands   legs
const std::vector<uint32> LeatherPrimaryGear            = { 16156,  4728,       1170,   26227,  13776 };
//                                                          head    shoulders   chest   hands   legs
const std::vector<uint32> ClothPrimaryGear              = { 32129,  28016,      26082,  30288,  30921 };
//                                                          head    shoulders   chest   hands   legs
const std::vector<uint32> MailPrimaryGear               = { 26388,  32137,      29899,  32149,  25551 };

//                                                          back    ring1   ring2   neck   trinket1
const std::vector<uint32> SharedSecondaryGear           = { 14701,  7466,   7466,   27228, 4031 };
//                                                          wrist   waist   boots
const std::vector<uint32> PlateSecondaryGear            = { 16512,  20324,  16030 };
//                                                          wrist   waist   boots
const std::vector<uint32> ClothSecondaryGear            = { 32977,  18341,  26097 };
//                                                          wrist   waist   boots
const std::vector<uint32> LeatherSecondaryGear          = { 26015,  20789,  26225 };
//                                                          wrist   waist   boots
const std::vector<uint32> MailSecondaryGear             = { 32968,  25794,  28011 };

//                                                          mainhand    shield  trinket2    ranged
const std::vector<uint32> ProtWarriorTertiaryGear       = { 26797,      26559,  35772,      29210 };
//                                                          mainhand    offhand trinket2    ranged
const std::vector<uint32> FuryWarriorTertiaryGear       = { 26797,      23232,  35771,      29210 };
//                                                          2hand   trinket2    ranged
const std::vector<uint32> ArmsWarriorTertiaryGear       = { 1311,   35771,      29210 };
//                                                          mainhand    shield  trinket2    libram
const std::vector<uint32> HolyPaladinTertiaryGear       = { 19359,      26559,  35773,      23201 };
//                                                          2hand   trinket2    libram
const std::vector<uint32> RetPaladinTertiaryGear        = { 1311,   35771,      31033 };
//                                                          mainhand    shield  trinket2    libram
const std::vector<uint32> ProtPaladinTertiaryGear       = { 20811,      26559,  35772,      27917 };
//                                                          mainhand    offhand trinket2    idol
const std::vector<uint32> BalanceDruidTertiaryGear      = { 32177,      26573,  35770,      23197 };
//                                                          2hand   trinket2    idol
const std::vector<uint32> FeralDruidTertiaryGear        = { 20522,  35771,      28064 };
//                                                          mainhand    offhand trinket2    idol
const std::vector<uint32> RestoDruidTertiaryGear        = { 19359,      26573,  35773,      22398 };
//                                                          mainhand    offhand trinket2    wand
const std::vector<uint32> DiscHolyPriestTertiaryGear    = { 19359,      26573,  35773,      32482 };
//                                                          mainhand    offhand trinket2    wand
const std::vector<uint32> CasterDPSTertiaryGear         = { 32177,      26573,  35770,      32482 }; // warlock, shadow priest, mage
//                                                          mainhand    offhand trinket2    totem
const std::vector<uint32> EnhanceShamanTertiaryGear     = { 26797,      26797,  35771,      22395 };
//                                                          mainhand    shield  trinket2    totem
const std::vector<uint32> ElementalShamanTertiaryGear   = { 32177,      26559,  35770,      23199 };
//                                                          mainhand    shield  trinket2    totem
const std::vector<uint32> RestoShamanTertiaryGear       = { 19359,      26559,  35773,      23200 };
//                                                          2hand   trinket2    ranged  quiver
const std::vector<uint32> HunterTertiaryGear            = { 1311,   35771,      20313,  1281 };
//                                                          mainhand    offhand trinket2    ranged
const std::vector<uint32> CombatSubRogueTertiaryGear    = { 23232,      23232,  35771,      29210 };
//                                                          mainhand    offhand trinket2    ranged
const std::vector<uint32> AssassinRogueTertiaryGear     = { 12767,      12767,  35771,      29210 };

// "Best In Slot"
const std::vector<uint32> Lvl60BiS_ShamanResto          = { 22466, 21712, 22467, 21583, 22464, 22471, 22465, 22469, 22470, 22468, 23065, 21620, 23047, 19395, 23056, 22819, 22396 };
const std::vector<uint32> Lvl60BiS_ShamanEnhancement    = { 18817, 18404, 21684, 23045, 21374, 21602, 21624, 21586, 21651, 21705, 17063, 23038, 22321, 19289, 22798, 22395 };
const std::vector<uint32> Lvl60BiS_ShamanElemental      = { 19375, 22943, 21376, 23050, 21671, 21464, 21585, 22730, 21375, 21373, 21707, 21709, 19379, 23046, 22988, 23049, 23199 };
const std::vector<uint32> Lvl60BiS_PriestShadow         = { 23035, 18814, 22983, 22731, 23220, 21611, 19133, 21585, 19400, 19131, 21709, 19434, 19379, 23046, 22988, 23049, 22820 };
const std::vector<uint32> Lvl60BiS_PriestDiscHoly       = { 21615, 21712, 22515, 22960, 22512, 21604, 22513, 22517, 21582, 22516, 23061, 22939, 23027, 23047, 23056, 23048, 23009 };
const std::vector<uint32> Lvl60BiS_PaladinHoly          = { 19375, 23057, 22429, 23050, 22425, 21604, 22427, 20264, 21582, 22430, 23066, 19382, 19395, 23047, 23056, 23075, 23006 };
const std::vector<uint32> Lvl60BiS_PaladinRetribution   = { 21387, 18404, 21391, 23045, 21389, 22936, 21623, 23219, 21390, 21388, 23038, 17063, 22321, 19289, 22691, 23203, 17182 };
const std::vector<uint32> Lvl60BiS_PaladinProtection    = { 21387, 22732, 21639, 22938, 21389, 20616, 21674, 21598, 19855, 21706, 19376, 21601, 19431, 18406, 22988, 23043, 22401, 19019 };
const std::vector<uint32> Lvl60BiS_WarriorFuryArms      = { 12640, 23053, 21330, 23045, 23000, 22936, 21581, 23219, 23068, 19387, 18821, 23038, 22954, 23041, 23054, 23577, 17076, 22812 };
const std::vector<uint32> Lvl60BiS_WarriorProtection    = { 22418, 22732, 22419, 22938, 22416, 22423, 22421, 22422, 22417, 22420, 23059, 21601, 19431, 19406, 19019, 23043, 19368 };
const std::vector<uint32> Lvl60BiS_DruidFeralCat        = { 8345, 19377, 21665, 21710, 23226, 21602, 21672, 21586, 23071, 21493, 23038, 19432, 19406, 23041, 22988, 22632, 22397, 13385 };
const std::vector<uint32> Lvl60BiS_DruidFeralBear       = { 21693, 22732, 19389, 22938, 23226, 21602, 21605, 21675, 20627, 19381, 21601, 23018, 13966, 11811, 943, 23198 };
const std::vector<uint32> Lvl60BiS_DruidBalance         = { 19375, 23057, 22983, 23050, 19682, 23021, 21585, 22730, 19683, 19684, 23025, 21709, 19379, 23046, 22988, 23049, 23197 };
const std::vector<uint32> Lvl60BiS_DruidResto           = { 20628, 21712, 22491, 22960, 22488, 21604, 22493, 21582, 22489, 22492, 22939, 21620, 23047, 23027, 23056, 22632, 22399, 23048 };
const std::vector<uint32> Lvl60BiS_Rogue                = { 22478, 19377, 22479, 23045, 22476, 22483, 22477, 22481, 22482, 22480, 23060, 23038, 23041, 22954, 23054, 22802, 21126, 23577, 22812, 19019 };
const std::vector<uint32> Lvl60BiS_Mage                 = { 22498, 23057, 22983, 23050, 22496, 23021, 23070, 21585, 22730, 22500, 23062, 23237, 19379, 23046, 19339, 22807, 23049, 22821 };
const std::vector<uint32> Lvl60BiS_Hunter               = { 22438, 23053, 22439, 23045, 22436, 22443, 22437, 22441, 22442, 22440, 23067, 22961, 23041, 19406, 22816, 22802, 22812 };
const std::vector<uint32> Lvl60BiS_Warlock              = { 22506, 23057, 22507, 23050, 22504, 21186, 23070, 21585, 22730, 22508, 21709, 23025, 19379, 23046, 22807, 23049, 22820 };

const std::vector<uint32> Lvl70BiS_ShamanResto          = { 32524, 31016, 30873, 32528, 32238, 32275, 31012, 31019, 32370, 31022, 32258, 30869, 32500, 30882, 32496, 30619, 32344, 30023 };
const std::vector<uint32> Lvl70BiS_ShamanEnhancement    = { 32323, 34912, 32510, 32497, 32335, 32234, 32376, 30900, 32260, 31024, 32346, 30864, 32946, 32945, 28830, 32505, 32332, 27815 };
const std::vector<uint32> Lvl70BiS_ShamanElemental      = { 32331, 31017, 32242, 32527, 32247, 31008, 31014, 31020, 32349, 31023, 30044, 32259, 34009, 30872, 32483, 30626, 32374, 32330 };
const std::vector<uint32> Lvl70BiS_PriestShadow         = { 32590, 31065, 32239, 32527, 32247, 31061, 31064, 30916, 30666, 31070, 32256, 30870, 32237, 30872, 32343, 32483, 28789, 32374 };
const std::vector<uint32> Lvl70BiS_PriestDiscHoly       = { 32524, 31066, 32609, 32528, 32238, 31060, 31063, 30912, 32370, 31069, 30895, 30871, 32500, 30911, 32363, 32496, 29376, 32344 };
const std::vector<uint32> Lvl70BiS_PaladinHoly          = { 32524, 30992, 32243, 32528, 32238, 30983, 30988, 30994, 32370, 30996, 30897, 30862, 32500, 30882, 32496, 29376, 28592 };
const std::vector<uint32> Lvl70BiS_PaladinRetribution   = { 32323, 30990, 32345, 32497, 32335, 32278, 32373, 32341, 32591, 30866, 30032, 30057, 33503, 32332, 28830, 32505 };
const std::vector<uint32> Lvl70BiS_PaladinProtection    = { 32331, 30991, 32245, 30083, 29297, 30985, 30987, 30995, 32362, 30998, 32342, 32279, 34009, 30909, 31856, 28789, 30620, 32368 };
const std::vector<uint32> Lvl70BiS_WarriorFuryArms      = { 32323, 30975, 32345, 32497, 32335, 32278, 32373, 32341, 32260, 30979, 30032, 30861, 32369, 30881, 30105, 32505, 28830, 32348 };
const std::vector<uint32> Lvl70BiS_WarriorProtection    = { 34010, 30976, 32268, 32261, 29297, 30970, 32521, 30978, 32362, 30980, 32333, 32232, 32254, 32375, 32325, 30620, 32501 };
const std::vector<uint32> Lvl70BiS_DruidFeralCat        = { 32323, 32252, 32366, 32497, 32266, 32347, 32235, 31044, 32260, 31048, 30106, 32324, 32257, 28830, 32505, 30883 };
const std::vector<uint32> Lvl70BiS_DruidFeralBear       = { 28660, 31042, 32593, 29279, 28792, 31034, 31039, 31044, 32362, 31048, 30879, 32324, 32658, 30620, 30021, 33509 };
const std::vector<uint32> Lvl70BiS_DruidBalance         = { 32331, 31043, 32352, 32527, 32247, 31035, 31040, 31046, 32349, 31049, 30914, 32351, 32237, 30872, 32483, 30626, 32374, 27518 };
const std::vector<uint32> Lvl70BiS_DruidResto           = { 32337, 31041, 30886, 32528, 29309, 31032, 31037, 31045, 32370, 31047, 32339, 30868, 32500, 30911, 32496, 29376, 32344, 30051 };
const std::vector<uint32> Lvl70BiS_Rogue                = { 32323, 31028, 32366, 32497, 32266, 31026, 32235, 31029, 32260, 31030, 30879, 32324, 32369, 30881, 32269, 32326, 28830, 32505 };
const std::vector<uint32> Lvl70BiS_Mage                 = { 32331, 31057, 32239, 32527, 32247, 31055, 32525, 31058, 30015, 31059, 30888, 30870, 30910, 30872, 29982, 32483, 27683, 32374 };
const std::vector<uint32> Lvl70BiS_Hunter               = { 32323, 31004, 30880, 29301, 32497, 31001, 32376, 31005, 32260, 31006, 32346, 32251, 32369, 32236, 30906, 28830, 32505, 32248 };
const std::vector<uint32> Lvl70BiS_Warlock              = { 32590, 31052, 30050, 32527, 32247, 31050, 32525, 31053, 32349, 31054, 32256, 29918, 30910, 30872, 32343, 32483, 27683, 32374 };

// Instant 58
//                                                          ring1   ring2   neck
const std::vector<uint32> Instant58Shared               = { 6711,   6711,   1122 };
//                                                          back
const std::vector<uint32> Instant58PhysicalBack         = { 22230 };
//                                                          back
const std::vector<uint32> Instant58MagicBack            = { 4011 };
//                                                          bow
const std::vector<uint32> Instant58Bow                  = { 5548 };
//                                                          wrist   waist   boots
const std::vector<uint32> Instant58LeatherOffpieces     = { 20269,  20267,  3533 };
//                                                          staff
const std::vector<uint32> Instant58Staff                = { 2487 };
//                                                          wand    mainhand    offhand boots   waist   wrist
const std::vector<uint32> Instant58ClothCaster          = { 7186,   7169,       12863,  21088,  26056,  16119 };
//                                                          wrist   waist   boots
const std::vector<uint32> Instant58MailOffpieces        = { 3525,   1969,   1174 };
//                                                          mainhand    shield
const std::vector<uint32> Instant58GavelAndShield       = { 1312,       14609 };
//                                                          mainhand    offhand
const std::vector<uint32> Instant58DualWield            = { 25799,      25799 };
//                                                          wrist   waist   boots
const std::vector<uint32> Instant58PlateOffpieces       = { 18431,  3952,  14689 };

const std::vector<uint32> Instant58Hunter               = { 3479, 3051, 13795, 3050, 24534, 5545, 29889, 1047, 1281 };
const std::vector<uint32> Instant58Rogue                = { 23652, 1018, 1313, 7167, 20270, 20308, 3532, 20271, 20274, 25877 };
const std::vector<uint32> Instant58Druid                = { 5554, 1535, 3549, 1424, 14388, 1023, 25667 };
const std::vector<uint32> Instant58Mage                 = { 20328, 20327, 20343, 20331, 20330 };
const std::vector<uint32> Instant58Paladin              = { 32110, 12615, 2275, 12413, 16031, 23202, 7940 };
const std::vector<uint32> Instant58Priest               = { 19844, 2573, 16141, 25800, 18747 };
const std::vector<uint32> Instant58Shaman               = { 20775, 3529, 3232, 20793, 20777, 6213, 7940 };
const std::vector<uint32> Instant58Warlock              = { 24563, 26165, 24564, 24562, 24566 };
const std::vector<uint32> Instant58Warrior              = { 20136, 1028, 3243, 33772, 20284, 28961, 31822, 7940 };

const std::vector<uint32> Instant58TaxiNodesAlliance =
{
    // Eastern Kingdoms:
    2,     // Stormwind, Elwynn
    4,     // Sentinel Hill, Westfall
    5,     // Lakeshire, Redridge
    6,     // Ironforge, Dun Morogh
    7,     // Menethil Harbor, Wetlands
    8,     // Thelsamar, Loch Modan
    12,    // Darkshire, Duskwood
    14,    // Southshore, Hillsbrad
    16,    // Refuge Pointe, Arathi
    19,    // Booty Bay, Stranglethorn
    43,    // Aerie Peak, The Hinterlands
    45,    // Nethergarde Keep, Blasted Lands
    66,    // Chillwind Camp, Western Plaguelands
    67,    // Light's Hope Chapel, Eastern Plaguelands
    71,    // Morgan's Vigil, Burning Steppes
    74,    // Thorium Point, Searing Gorge
    // Kalimdor:
    26,    // Auberdine, Darkshore
    27,    // Rut'theran Village, Teldrassil
    28,    // Astranaar, Ashenvale
    31,    // Thalanaar, Feralas
    32,    // Theramore, Dustwallow Marsh
    33,    // Stonetalon Peak, Stonetalon Mountains
    37,    // Nijel's Point, Desolace
    39,    // Gadgetzan, Tanaris
    41,    // Feathermoon, Feralas
    49,    // Moonglade
    52,    // Everlook, Winterspring
    64,    // Talrendis Point, Azshara
    65,    // Talonbranch Glade, Felwood
    73,    // Cenarion Hold, Silithus
    79,    // Marshal's Refuge, Un'Goro Crater
    80     // Ratchet, The Barrens
};
const std::vector<uint32> Instant58TaxiNodesHorde =
{
    // Eastern kingdoms:
    10,    // The Sepulcher, Silverpine Forest
    11,    // Undercity, Tirisfal
    13,    // Tarren Mill, Hillsbrad
    17,    // Hammerfall, Arathi
    18,    // Booty Bay, Stranglethorn
    20,    // Grom'gol, Stranglethorn
    21,    // Kargath, Badlands
    56,    // Stonard, Swamp of Sorrows
    68,    // Light's Hope Chapel, Eastern Plaguelands
    70,    // Flame Crest, Burning Steppes
    75,    // Thorium Point, Searing Gorge
    76,    // Revantusk Village, The Hinterlands
    // Kalimdor:
    22,    // Thunder Bluff, Mulgore
    23,    // Orgrimmar, Durotar
    25,    // Crossroads, The Barrens
    29,    // Sun Rock Retreat, Stonetalon Mountains
    30,    // Freewind Post, Thousand Needles
    38,    // Shadowprey Village, Desolace
    40,    // Gadgetzan, Tanaris
    42,    // Camp Mojache, Feralas
    44,    // Valormok, Azshara
    48,    // Bloodvenom Post, Felwood
    53,    // Everlook, Winterspring
    55,    // Brackenwall Village, Dustwallow Marsh
    58,    // Zoram'gar Outpost, Ashenvale
    61,    // Splintertree Post, Ashenvale
    69,    // Moonglade
    72,    // Cenarion Hold, Silithus
    77,    // Camp Taurajo, The Barrens
    79,    // Marshal's Refuge, Un'Goro Crater
    80     // Ratchet, The Barrens
};

struct npc_enlistment_officerAI : public ScriptedAI
{
    npc_enlistment_officerAI(Creature* creature) : ScriptedAI(creature)
    { 
        Reset();

        FullGearList.clear();
        FullGearList.insert(std::end(FullGearList), std::begin(PlatePrimaryGear), std::end(PlatePrimaryGear));
        FullGearList.insert(std::end(FullGearList), std::begin(LeatherPrimaryGear), std::end(LeatherPrimaryGear));
        FullGearList.insert(std::end(FullGearList), std::begin(ClothPrimaryGear), std::end(ClothPrimaryGear));
        FullGearList.insert(std::end(FullGearList), std::begin(MailPrimaryGear), std::end(MailPrimaryGear));
        FullGearList.insert(std::end(FullGearList), std::begin(SharedSecondaryGear), std::end(SharedSecondaryGear));
        FullGearList.insert(std::end(FullGearList), std::begin(PlateSecondaryGear), std::end(PlateSecondaryGear));
        FullGearList.insert(std::end(FullGearList), std::begin(ClothSecondaryGear), std::end(ClothSecondaryGear));
        FullGearList.insert(std::end(FullGearList), std::begin(LeatherSecondaryGear), std::end(LeatherSecondaryGear));
        FullGearList.insert(std::end(FullGearList), std::begin(MailSecondaryGear), std::end(MailSecondaryGear));
        FullGearList.insert(std::end(FullGearList), std::begin(ProtWarriorTertiaryGear), std::end(ProtWarriorTertiaryGear));
        FullGearList.insert(std::end(FullGearList), std::begin(FuryWarriorTertiaryGear), std::end(FuryWarriorTertiaryGear));
        FullGearList.insert(std::end(FullGearList), std::begin(ArmsWarriorTertiaryGear), std::end(ArmsWarriorTertiaryGear));
        FullGearList.insert(std::end(FullGearList), std::begin(HolyPaladinTertiaryGear), std::end(HolyPaladinTertiaryGear));
        FullGearList.insert(std::end(FullGearList), std::begin(RetPaladinTertiaryGear), std::end(RetPaladinTertiaryGear));
        FullGearList.insert(std::end(FullGearList), std::begin(ProtPaladinTertiaryGear), std::end(ProtPaladinTertiaryGear));
        FullGearList.insert(std::end(FullGearList), std::begin(BalanceDruidTertiaryGear), std::end(BalanceDruidTertiaryGear));
        FullGearList.insert(std::end(FullGearList), std::begin(FeralDruidTertiaryGear), std::end(FeralDruidTertiaryGear));
        FullGearList.insert(std::end(FullGearList), std::begin(RestoDruidTertiaryGear), std::end(RestoDruidTertiaryGear));
        FullGearList.insert(std::end(FullGearList), std::begin(DiscHolyPriestTertiaryGear), std::end(DiscHolyPriestTertiaryGear));
        FullGearList.insert(std::end(FullGearList), std::begin(CasterDPSTertiaryGear), std::end(CasterDPSTertiaryGear));
        FullGearList.insert(std::end(FullGearList), std::begin(EnhanceShamanTertiaryGear), std::end(EnhanceShamanTertiaryGear));
        FullGearList.insert(std::end(FullGearList), std::begin(ElementalShamanTertiaryGear), std::end(ElementalShamanTertiaryGear));
        FullGearList.insert(std::end(FullGearList), std::begin(RestoShamanTertiaryGear), std::end(RestoShamanTertiaryGear));
        FullGearList.insert(std::end(FullGearList), std::begin(HunterTertiaryGear), std::end(HunterTertiaryGear));
        FullGearList.insert(std::end(FullGearList), std::begin(AssassinRogueTertiaryGear), std::end(AssassinRogueTertiaryGear));
        FullGearList.insert(std::end(FullGearList), std::begin(CombatSubRogueTertiaryGear), std::end(CombatSubRogueTertiaryGear));

        FullGearListInstant58.clear();
        FullGearListInstant58.insert(std::end(FullGearListInstant58), std::begin(Instant58Shared), std::end(Instant58Shared));
        FullGearListInstant58.insert(std::end(FullGearListInstant58), std::begin(Instant58PhysicalBack), std::end(Instant58PhysicalBack));
        FullGearListInstant58.insert(std::end(FullGearListInstant58), std::begin(Instant58MagicBack), std::end(Instant58MagicBack));
        FullGearListInstant58.insert(std::end(FullGearListInstant58), std::begin(Instant58Bow), std::end(Instant58Bow));
        FullGearListInstant58.insert(std::end(FullGearListInstant58), std::begin(Instant58LeatherOffpieces), std::end(Instant58LeatherOffpieces));
        FullGearListInstant58.insert(std::end(FullGearListInstant58), std::begin(Instant58Staff), std::end(Instant58Staff));
        FullGearListInstant58.insert(std::end(FullGearListInstant58), std::begin(Instant58ClothCaster), std::end(Instant58ClothCaster));
        FullGearListInstant58.insert(std::end(FullGearListInstant58), std::begin(Instant58MailOffpieces), std::end(Instant58MailOffpieces));
        FullGearListInstant58.insert(std::end(FullGearListInstant58), std::begin(Instant58GavelAndShield), std::end(Instant58GavelAndShield));
        FullGearListInstant58.insert(std::end(FullGearListInstant58), std::begin(Instant58DualWield), std::end(Instant58DualWield));
        FullGearListInstant58.insert(std::end(FullGearListInstant58), std::begin(Instant58PlateOffpieces), std::end(Instant58PlateOffpieces));
        FullGearListInstant58.insert(std::end(FullGearListInstant58), std::begin(Instant58Hunter), std::end(Instant58Hunter));
        FullGearListInstant58.insert(std::end(FullGearListInstant58), std::begin(Instant58Rogue), std::end(Instant58Rogue));
        FullGearListInstant58.insert(std::end(FullGearListInstant58), std::begin(Instant58Druid), std::end(Instant58Druid));
        FullGearListInstant58.insert(std::end(FullGearListInstant58), std::begin(Instant58Mage), std::end(Instant58Mage));
        FullGearListInstant58.insert(std::end(FullGearListInstant58), std::begin(Instant58Paladin), std::end(Instant58Paladin));
        FullGearListInstant58.insert(std::end(FullGearListInstant58), std::begin(Instant58Priest), std::end(Instant58Priest));
        FullGearListInstant58.insert(std::end(FullGearListInstant58), std::begin(Instant58Shaman), std::end(Instant58Shaman));
        FullGearListInstant58.insert(std::end(FullGearListInstant58), std::begin(Instant58Warlock), std::end(Instant58Warlock));
        FullGearListInstant58.insert(std::end(FullGearListInstant58), std::begin(Instant58Warrior), std::end(Instant58Warrior));

        FullGearListBiS60.clear();
        FullGearListBiS60.insert(std::end(FullGearListBiS60), std::begin(Lvl60BiS_ShamanResto), std::end(Lvl60BiS_ShamanResto));
        FullGearListBiS60.insert(std::end(FullGearListBiS60), std::begin(Lvl60BiS_ShamanEnhancement), std::end(Lvl60BiS_ShamanEnhancement));
        FullGearListBiS60.insert(std::end(FullGearListBiS60), std::begin(Lvl60BiS_ShamanElemental), std::end(Lvl60BiS_ShamanElemental));
        FullGearListBiS60.insert(std::end(FullGearListBiS60), std::begin(Lvl60BiS_PriestShadow), std::end(Lvl60BiS_PriestShadow));
        FullGearListBiS60.insert(std::end(FullGearListBiS60), std::begin(Lvl60BiS_PriestDiscHoly), std::end(Lvl60BiS_PriestDiscHoly));
        FullGearListBiS60.insert(std::end(FullGearListBiS60), std::begin(Lvl60BiS_PaladinHoly), std::end(Lvl60BiS_PaladinHoly));
        FullGearListBiS60.insert(std::end(FullGearListBiS60), std::begin(Lvl60BiS_PaladinRetribution), std::end(Lvl60BiS_PaladinRetribution));
        FullGearListBiS60.insert(std::end(FullGearListBiS60), std::begin(Lvl60BiS_PaladinProtection), std::end(Lvl60BiS_PaladinProtection));
        FullGearListBiS60.insert(std::end(FullGearListBiS60), std::begin(Lvl60BiS_WarriorFuryArms), std::end(Lvl60BiS_WarriorFuryArms));
        FullGearListBiS60.insert(std::end(FullGearListBiS60), std::begin(Lvl60BiS_WarriorProtection), std::end(Lvl60BiS_WarriorProtection));
        FullGearListBiS60.insert(std::end(FullGearListBiS60), std::begin(Lvl60BiS_DruidFeralCat), std::end(Lvl60BiS_DruidFeralCat));
        FullGearListBiS60.insert(std::end(FullGearListBiS60), std::begin(Lvl60BiS_DruidFeralBear), std::end(Lvl60BiS_DruidFeralBear));
        FullGearListBiS60.insert(std::end(FullGearListBiS60), std::begin(Lvl60BiS_DruidBalance), std::end(Lvl60BiS_DruidBalance));
        FullGearListBiS60.insert(std::end(FullGearListBiS60), std::begin(Lvl60BiS_DruidResto), std::end(Lvl60BiS_DruidResto));
        FullGearListBiS60.insert(std::end(FullGearListBiS60), std::begin(Lvl60BiS_Rogue), std::end(Lvl60BiS_Rogue));
        FullGearListBiS60.insert(std::end(FullGearListBiS60), std::begin(Lvl60BiS_Mage), std::end(Lvl60BiS_Mage));
        FullGearListBiS60.insert(std::end(FullGearListBiS60), std::begin(Lvl60BiS_Hunter), std::end(Lvl60BiS_Hunter));
        FullGearListBiS60.insert(std::end(FullGearListBiS60), std::begin(Lvl60BiS_Warlock), std::end(Lvl60BiS_Warlock));

        FullGearListBiS70.clear();
        FullGearListBiS70.insert(std::end(FullGearListBiS70), std::begin(Lvl70BiS_ShamanResto), std::end(Lvl70BiS_ShamanResto));
        FullGearListBiS70.insert(std::end(FullGearListBiS70), std::begin(Lvl70BiS_ShamanEnhancement), std::end(Lvl70BiS_ShamanEnhancement));
        FullGearListBiS70.insert(std::end(FullGearListBiS70), std::begin(Lvl70BiS_ShamanElemental), std::end(Lvl70BiS_ShamanElemental));
        FullGearListBiS70.insert(std::end(FullGearListBiS70), std::begin(Lvl70BiS_PriestShadow), std::end(Lvl70BiS_PriestShadow));
        FullGearListBiS70.insert(std::end(FullGearListBiS70), std::begin(Lvl70BiS_PriestDiscHoly), std::end(Lvl70BiS_PriestDiscHoly));
        FullGearListBiS70.insert(std::end(FullGearListBiS70), std::begin(Lvl70BiS_PaladinHoly), std::end(Lvl70BiS_PaladinHoly));
        FullGearListBiS70.insert(std::end(FullGearListBiS70), std::begin(Lvl70BiS_PaladinRetribution), std::end(Lvl70BiS_PaladinRetribution));
        FullGearListBiS70.insert(std::end(FullGearListBiS70), std::begin(Lvl70BiS_PaladinProtection), std::end(Lvl70BiS_PaladinProtection));
        FullGearListBiS70.insert(std::end(FullGearListBiS70), std::begin(Lvl70BiS_WarriorFuryArms), std::end(Lvl70BiS_WarriorFuryArms));
        FullGearListBiS70.insert(std::end(FullGearListBiS70), std::begin(Lvl70BiS_WarriorProtection), std::end(Lvl70BiS_WarriorProtection));
        FullGearListBiS70.insert(std::end(FullGearListBiS70), std::begin(Lvl70BiS_DruidFeralCat), std::end(Lvl70BiS_DruidFeralCat));
        FullGearListBiS70.insert(std::end(FullGearListBiS70), std::begin(Lvl70BiS_DruidFeralBear), std::end(Lvl70BiS_DruidFeralBear));
        FullGearListBiS70.insert(std::end(FullGearListBiS70), std::begin(Lvl70BiS_DruidBalance), std::end(Lvl70BiS_DruidBalance));
        FullGearListBiS70.insert(std::end(FullGearListBiS70), std::begin(Lvl70BiS_DruidResto), std::end(Lvl70BiS_DruidResto));
        FullGearListBiS70.insert(std::end(FullGearListBiS70), std::begin(Lvl70BiS_Rogue), std::end(Lvl70BiS_Rogue));
        FullGearListBiS70.insert(std::end(FullGearListBiS70), std::begin(Lvl70BiS_Mage), std::end(Lvl70BiS_Mage));
        FullGearListBiS70.insert(std::end(FullGearListBiS70), std::begin(Lvl70BiS_Hunter), std::end(Lvl70BiS_Hunter));
        FullGearListBiS70.insert(std::end(FullGearListBiS70), std::begin(Lvl70BiS_Warlock), std::end(Lvl70BiS_Warlock));
    }

    bool m_bCanGreet;
    uint32 m_uiGreetTimer;
    GuidList m_lPlayerGuids;
    std::vector<uint32> FullGearList;
    std::vector<uint32> FullGearListInstant58;
    std::vector<uint32> FullGearListBiS60;
    std::vector<uint32> FullGearListBiS70;

    void Reset() override
    {
        m_uiGreetTimer = 2000;
        m_bCanGreet = true;
        m_lPlayerGuids.clear();
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (m_uiGreetTimer < uiDiff)
        {
            m_bCanGreet = true;
        }
        else
            m_uiGreetTimer -= uiDiff;
    }

    void MoveInLineOfSight(Unit* pWho) override
    {
        if (m_bCanGreet && pWho->GetTypeId() == TYPEID_PLAYER && m_creature->IsWithinDistInMap(pWho, 15.0f) && m_creature->IsWithinLOSInMap(pWho))
        {
            if (((Player*)pWho)->isGameMaster())
                return;

            if ((pWho->getLevel() == 58 && ((((Player*)pWho)->GetTeam() == HORDE && m_creature->GetEntry() == NPC_HORDE_OFFICER) || (((Player*)pWho)->GetTeam() == ALLIANCE && m_creature->GetEntry() == NPC_ALLIANCE_OFFICER)))
                || (pWho->getLevel() == 70 && m_creature->GetEntry() == NPC_MASTER_PROVISIONER))
            {
                // check to see if player has been greeted already
                for (GuidList::const_iterator itr = m_lPlayerGuids.begin(); itr != m_lPlayerGuids.end(); ++itr)
                {
                    if ((*itr) == pWho->GetObjectGuid())
                        return;
                }

                m_lPlayerGuids.push_back(pWho->GetObjectGuid());

                m_creature->SetFacingToObject(pWho);

                if (m_creature->GetEntry() == NPC_HORDE_OFFICER || m_creature->GetEntry() == NPC_ALLIANCE_OFFICER)
                    DoScriptText(SAY_GREET, m_creature, pWho);
                else if (m_creature->GetEntry() == NPC_MASTER_PROVISIONER)
                    DoScriptText(SAY_GREET_70, m_creature, pWho);

                m_bCanGreet = false;
                m_uiGreetTimer = urand(5000, 10000);
            }
        }

        ScriptedAI::MoveInLineOfSight(pWho);
    }

    uint32 GetStarterMountForRace(Player* player)
    {
        uint32 mountEntry = 0;
        switch (player->getRace())
        {
        case RACE_HUMAN: mountEntry = 2411; break;      // Black Stallion Bridle
        case RACE_DWARF: mountEntry = 5872; break;      // Brown Ram
        case RACE_NIGHTELF: mountEntry = 8632; break;   // Reins of the Spotted Frostsaber
        case RACE_GNOME: mountEntry = 8595; break;      // Blue Mechanostrider
        case RACE_DRAENEI: mountEntry = 29744; break;   // Gray Elekk
        case RACE_ORC: mountEntry = 5665; break;        // Horn of the Dire Wolf
        case RACE_UNDEAD: mountEntry = 13331; break;    // Red Skeletal Horse
        case RACE_TAUREN: mountEntry = 15277; break;    // Gray Kodo
        case RACE_TROLL: mountEntry = 8588; break;      // Whistle of the Emerald Raptor
        case RACE_BLOODELF: mountEntry = 29221; break;  // Black Hawkstrider
        }
        return mountEntry;
    }

    uint32 GetStarterEpicMountForRace(Player* player)
    {
        uint32 mountEntry = 0;
        switch (player->getRace())
        {
        case RACE_HUMAN: mountEntry = 18776; break;     // Swift Palomino
        case RACE_DWARF: mountEntry = 18787; break;     // Swift Gray Ram
        case RACE_NIGHTELF: mountEntry = 18767; break;  // Reins of the Swift Mistsaber
        case RACE_GNOME: mountEntry = 18772; break;     // Swift Green Mechanostrider
        case RACE_DRAENEI: mountEntry = 29745; break;   // Great Blue Elekk
        case RACE_ORC: mountEntry = 18797; break;       // Horn of the Swift Timber Wolf 
        case RACE_UNDEAD: mountEntry = 18791; break;    // Purple Skeletal Warhorse
        case RACE_TAUREN: mountEntry = 18795; break;    // Great Gray Kodo
        case RACE_TROLL: mountEntry = 18790; break;     // Swift Orange Raptor
        case RACE_BLOODELF: mountEntry = 28927; break;  // Red Hawkstrider
        }
        return mountEntry;
    }

    void CreatePet(Player* player, Creature* creature, uint32 entry)
    {
        if (Creature* creatureTarget = m_creature->SummonCreature(entry, player->GetPositionX(), player->GetPositionY() + 2, player->GetPositionZ(), player->GetOrientation(), TEMPSPAWN_CORPSE_TIMED_DESPAWN, 500))
        {
            creatureTarget->SetLevel(player->getLevel() - 1);

            Pet* pet = new Pet(HUNTER_PET);

            if (!pet->CreateBaseAtCreature(creatureTarget))
            {
                delete pet;
                return;
            }

            pet->SetUInt32Value(UNIT_CREATED_BY_SPELL, 13481); // tame beast

            pet->SetOwnerGuid(player->GetObjectGuid());
            pet->setFaction(player->getFaction());

            if (player->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PLAYER_CONTROLLED))
            {
                pet->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PLAYER_CONTROLLED);
                pet->SetByteValue(UNIT_FIELD_BYTES_2, 1, UNIT_BYTE2_PLAYER_CONTROLLED_DEBUFF_LIMIT);
            }
            else
                pet->SetByteValue(UNIT_FIELD_BYTES_2, 1, UNIT_BYTE2_CREATURE_DEBUFF_LIMIT);

            if (player->IsImmuneToNPC())
                pet->SetImmuneToNPC(true);

            if (player->IsImmuneToPlayer())
                pet->SetImmuneToPlayer(true);

            if (player->IsPvP())
                pet->SetPvP(true);

            pet->GetCharmInfo()->SetPetNumber(sObjectMgr.GeneratePetNumber(), true);

            uint32 level = creatureTarget->getLevel();
            pet->SetCanModifyStats(true);
            pet->InitStatsForLevel(level);

            pet->SetHealthPercent(creatureTarget->GetHealthPercent());

            // destroy creature object
            creatureTarget->ForcedDespawn();

            // prepare visual effect for levelup
            pet->SetUInt32Value(UNIT_FIELD_LEVEL, level - 1);

            // add pet object to the world
            pet->GetMap()->Add((Creature*)pet);
            pet->AIM_Initialize();

            pet->AI()->SetReactState(REACT_DEFENSIVE);

            // visual effect for levelup
            pet->SetUInt32Value(UNIT_FIELD_LEVEL, level);

            // enable rename and abandon prompt
            pet->SetByteFlag(UNIT_FIELD_BYTES_2, 2, UNIT_CAN_BE_RENAMED);
            pet->SetByteFlag(UNIT_FIELD_BYTES_2, 2, UNIT_CAN_BE_ABANDONED);

            // this enables pet details window (Shift+P)
            pet->InitPetCreateSpells();

            pet->LearnPetPassives();
            pet->CastPetAuras(true);
            pet->CastOwnerTalentAuras();
            pet->UpdateAllStats();

            // start with maximum loyalty and training points
            pet->SetLoyaltyLevel(BEST_FRIEND);
            pet->ModifyLoyalty(26500);
            pet->SetTP(level * (BEST_FRIEND - 1));
            pet->SetPower(POWER_HAPPINESS, HAPPINESS_LEVEL_SIZE * 2); // Content

            pet->SetRequiredXpForNextLoyaltyLevel();

            // caster have pet now
            player->SetPet(pet);

            player->PetSpellInitialize();

            pet->SavePetToDB(PET_SAVE_AS_CURRENT, player);
        }

        player->PlayerTalkClass->CloseGossip();
        creature->MonsterWhisper("An excellent choice! May it serve you well on the battlefield.", player);
    }

    bool HasStarterSet(Player* player, std::vector<uint32> gearList)
    {
        for (auto item : gearList)
        {
            ItemPrototype const* proto = ObjectMgr::GetItemPrototype(item);
            if (!proto)
                continue;

            // don't check for quiver/ammo pouch
            if (proto->InventoryType == INVTYPE_BAG || item == 23197)
                continue;

            if (player->HasItemCount(item, 1))
                return true;
        }

        return false;
    }

    void RemoveStarterSet(Player* player, std::vector<uint32> gearList)
    {
        for (auto item : gearList)
        {
            ItemPrototype const* proto = ObjectMgr::GetItemPrototype(item);
            if (!proto)
                continue;

            // don't check for quiver/ammo pouch
            if (proto->InventoryType == INVTYPE_BAG)
                continue;

            player->DestroyItemCount(item, 2, true, false);
        }
    }

    void AddStarterSet(Player* player, Creature* creature, uint32 setId, int32 suffixOverride = 0, uint32 specId = 0)
    {
        std::vector<uint32> gearList;

        if (setId == SET_ID_INSTANT_58)
        {
            gearList.insert(std::end(gearList), std::begin(Instant58Shared), std::end(Instant58Shared));
            switch (player->getClass())
            {
                case CLASS_WARRIOR:
                    gearList.insert(std::end(gearList), std::begin(Instant58PhysicalBack), std::end(Instant58PhysicalBack));
                    gearList.insert(std::end(gearList), std::begin(Instant58Bow), std::end(Instant58Bow));
                    gearList.insert(std::end(gearList), std::begin(Instant58DualWield), std::end(Instant58DualWield));
                    gearList.insert(std::end(gearList), std::begin(Instant58PlateOffpieces), std::end(Instant58PlateOffpieces));
                    gearList.insert(std::end(gearList), std::begin(Instant58Warrior), std::end(Instant58Warrior));
                    break;
                case CLASS_PALADIN:
                    gearList.insert(std::end(gearList), std::begin(Instant58PhysicalBack), std::end(Instant58PhysicalBack));
                    gearList.insert(std::end(gearList), std::begin(Instant58MagicBack), std::end(Instant58MagicBack));
                    gearList.insert(std::end(gearList), std::begin(Instant58GavelAndShield), std::end(Instant58GavelAndShield));
                    gearList.insert(std::end(gearList), std::begin(Instant58PlateOffpieces), std::end(Instant58PlateOffpieces));
                    gearList.insert(std::end(gearList), std::begin(Instant58Paladin), std::end(Instant58Paladin));
                    break;
                case CLASS_PRIEST:
                    gearList.insert(std::end(gearList), std::begin(Instant58MagicBack), std::end(Instant58MagicBack));
                    gearList.insert(std::end(gearList), std::begin(Instant58Staff), std::end(Instant58Staff));
                    gearList.insert(std::end(gearList), std::begin(Instant58ClothCaster), std::end(Instant58ClothCaster));
                    gearList.insert(std::end(gearList), std::begin(Instant58Priest), std::end(Instant58Priest));
                    break;
                case CLASS_WARLOCK:
                    gearList.insert(std::end(gearList), std::begin(Instant58MagicBack), std::end(Instant58MagicBack));
                    gearList.insert(std::end(gearList), std::begin(Instant58Staff), std::end(Instant58Staff));
                    gearList.insert(std::end(gearList), std::begin(Instant58ClothCaster), std::end(Instant58ClothCaster));
                    gearList.insert(std::end(gearList), std::begin(Instant58Warlock), std::end(Instant58Warlock));
                    break;
                case CLASS_MAGE:
                    gearList.insert(std::end(gearList), std::begin(Instant58MagicBack), std::end(Instant58MagicBack));
                    gearList.insert(std::end(gearList), std::begin(Instant58Staff), std::end(Instant58Staff));
                    gearList.insert(std::end(gearList), std::begin(Instant58ClothCaster), std::end(Instant58ClothCaster));
                    gearList.insert(std::end(gearList), std::begin(Instant58Mage), std::end(Instant58Mage));
                    break;
                case CLASS_DRUID:
                    gearList.insert(std::end(gearList), std::begin(Instant58PhysicalBack), std::end(Instant58PhysicalBack));
                    gearList.insert(std::end(gearList), std::begin(Instant58MagicBack), std::end(Instant58MagicBack));
                    gearList.insert(std::end(gearList), std::begin(Instant58LeatherOffpieces), std::end(Instant58LeatherOffpieces));
                    gearList.insert(std::end(gearList), std::begin(Instant58Staff), std::end(Instant58Staff));
                    gearList.insert(std::end(gearList), std::begin(Instant58Druid), std::end(Instant58Druid));
                    break;
                case CLASS_ROGUE:
                    gearList.insert(std::end(gearList), std::begin(Instant58Bow), std::end(Instant58Bow));
                    gearList.insert(std::end(gearList), std::begin(Instant58PhysicalBack), std::end(Instant58PhysicalBack));
                    gearList.insert(std::end(gearList), std::begin(Instant58LeatherOffpieces), std::end(Instant58LeatherOffpieces));
                    gearList.insert(std::end(gearList), std::begin(Instant58Rogue), std::end(Instant58Rogue));
                    break;
                case CLASS_SHAMAN:
                    gearList.insert(std::end(gearList), std::begin(Instant58PhysicalBack), std::end(Instant58PhysicalBack));
                    gearList.insert(std::end(gearList), std::begin(Instant58MagicBack), std::end(Instant58MagicBack));
                    gearList.insert(std::end(gearList), std::begin(Instant58MailOffpieces), std::end(Instant58MailOffpieces));
                    gearList.insert(std::end(gearList), std::begin(Instant58GavelAndShield), std::end(Instant58GavelAndShield));
                    gearList.insert(std::end(gearList), std::begin(Instant58DualWield), std::end(Instant58DualWield));
                    gearList.insert(std::end(gearList), std::begin(Instant58Shaman), std::end(Instant58Shaman));
                    break;
                case CLASS_HUNTER:
                    gearList.insert(std::end(gearList), std::begin(Instant58PhysicalBack), std::end(Instant58PhysicalBack));
                    gearList.insert(std::end(gearList), std::begin(Instant58Bow), std::end(Instant58Bow));
                    gearList.insert(std::end(gearList), std::begin(Instant58MailOffpieces), std::end(Instant58MailOffpieces));
                    gearList.insert(std::end(gearList), std::begin(Instant58Hunter), std::end(Instant58Hunter));
                    break;
            }
        }
        else if (setId == SET_ID_PRIMARY)
        {
            switch (player->getClass())
            {
                case CLASS_WARRIOR:
                case CLASS_PALADIN:
                    gearList.insert(std::end(gearList), std::begin(PlatePrimaryGear), std::end(PlatePrimaryGear));
                    break;
                case CLASS_PRIEST:
                case CLASS_WARLOCK:
                case CLASS_MAGE:
                    gearList.insert(std::end(gearList), std::begin(ClothPrimaryGear), std::end(ClothPrimaryGear));
                    break;
                case CLASS_DRUID:
                case CLASS_ROGUE:
                    gearList.insert(std::end(gearList), std::begin(LeatherPrimaryGear), std::end(LeatherPrimaryGear));
                    break;
                case CLASS_SHAMAN:
                case CLASS_HUNTER:
                    gearList.insert(std::end(gearList), std::begin(MailPrimaryGear), std::end(MailPrimaryGear));
                    break;
            }
        }
        else if (setId == SET_ID_SECONDARY)
        {
            gearList.insert(std::end(gearList), std::begin(SharedSecondaryGear), std::end(SharedSecondaryGear));
            switch (player->getClass())
            {
                case CLASS_WARRIOR:
                case CLASS_PALADIN:
                    gearList.insert(std::end(gearList), std::begin(PlateSecondaryGear), std::end(PlateSecondaryGear));
                    break;
                case CLASS_PRIEST:
                case CLASS_WARLOCK:
                case CLASS_MAGE:
                    gearList.insert(std::end(gearList), std::begin(ClothSecondaryGear), std::end(ClothSecondaryGear));
                    break;
                case CLASS_DRUID:
                case CLASS_ROGUE:
                    gearList.insert(std::end(gearList), std::begin(LeatherSecondaryGear), std::end(LeatherSecondaryGear));
                    break;
                case CLASS_SHAMAN:
                case CLASS_HUNTER:
                    gearList.insert(std::end(gearList), std::begin(MailSecondaryGear), std::end(MailSecondaryGear));
                    break;
            }
        }
        else if (setId == SET_ID_TERTIARY)
        {
            switch (player->getClass())
            {
                case CLASS_WARRIOR:
                    if (specId == 0) gearList.insert(std::end(gearList), std::begin(ArmsWarriorTertiaryGear), std::end(ArmsWarriorTertiaryGear));
                    else if (specId == 1) gearList.insert(std::end(gearList), std::begin(FuryWarriorTertiaryGear), std::end(FuryWarriorTertiaryGear));
                    else if (specId == 2) gearList.insert(std::end(gearList), std::begin(ProtWarriorTertiaryGear), std::end(ProtWarriorTertiaryGear));
                    break;
                case CLASS_PALADIN:
                    if (specId == 0) gearList.insert(std::end(gearList), std::begin(HolyPaladinTertiaryGear), std::end(HolyPaladinTertiaryGear));
                    else if (specId == 1) gearList.insert(std::end(gearList), std::begin(ProtPaladinTertiaryGear), std::end(ProtPaladinTertiaryGear));
                    else if (specId == 2) gearList.insert(std::end(gearList), std::begin(RetPaladinTertiaryGear), std::end(RetPaladinTertiaryGear));
                    break;
                case CLASS_PRIEST:
                    if (specId == 0 || specId == 1) gearList.insert(std::end(gearList), std::begin(DiscHolyPriestTertiaryGear), std::end(DiscHolyPriestTertiaryGear));
                    else if (specId == 2) gearList.insert(std::end(gearList), std::begin(CasterDPSTertiaryGear), std::end(CasterDPSTertiaryGear));
                    break;
                case CLASS_WARLOCK:
                case CLASS_MAGE:
                    gearList.insert(std::end(gearList), std::begin(CasterDPSTertiaryGear), std::end(CasterDPSTertiaryGear));
                    break;
                case CLASS_DRUID:
                    if (specId == 0) gearList.insert(std::end(gearList), std::begin(BalanceDruidTertiaryGear), std::end(BalanceDruidTertiaryGear));
                    else if (specId == 1) gearList.insert(std::end(gearList), std::begin(FeralDruidTertiaryGear), std::end(FeralDruidTertiaryGear));
                    else if (specId == 2) gearList.insert(std::end(gearList), std::begin(RestoDruidTertiaryGear), std::end(RestoDruidTertiaryGear));
                    break;
                case CLASS_ROGUE:
                    if (specId == 0) gearList.insert(std::end(gearList), std::begin(AssassinRogueTertiaryGear), std::end(AssassinRogueTertiaryGear));
                    else if (specId == 1 || specId == 2) gearList.insert(std::end(gearList), std::begin(CombatSubRogueTertiaryGear), std::end(CombatSubRogueTertiaryGear));
                    break;
                case CLASS_SHAMAN:
                    if (specId == 0) gearList.insert(std::end(gearList), std::begin(ElementalShamanTertiaryGear), std::end(ElementalShamanTertiaryGear));
                    else if (specId == 1) gearList.insert(std::end(gearList), std::begin(EnhanceShamanTertiaryGear), std::end(EnhanceShamanTertiaryGear));
                    else if (specId == 2) gearList.insert(std::end(gearList), std::begin(RestoShamanTertiaryGear), std::end(RestoShamanTertiaryGear));
                    break;
                case CLASS_HUNTER:
                    gearList.insert(std::end(gearList), std::begin(HunterTertiaryGear), std::end(HunterTertiaryGear));
                    break;
            }
        }
        GivePlayerItems(player, gearList, setId, suffixOverride);
    }

    void GivePlayerItems(Player* recipient, std::vector<uint32> gearList, uint32 setId = 0, int32 suffixOverride = 0)
    {
        bool allSuccess = true;
        bool alreadyHave = false;
        for (auto item : gearList)
        {
            ItemPrototype const* proto = ObjectMgr::GetItemPrototype(item);
            if (!proto)
                continue;

            if (recipient->HasItemCount(item, (proto->InventoryType != INVTYPE_FINGER && proto->InventoryType != INVTYPE_WEAPON) ? 1 : 2))
            {
                alreadyHave = true;
                continue;
            }

            if (!recipient->StoreNewItemInBestSlots(item, 1, (proto->InventoryType == INVTYPE_TRINKET || proto->InventoryType == INVTYPE_RELIC || proto->InventoryType == INVTYPE_BAG) ? 0 : -suffixOverride))
                allSuccess = false;
        }

        if (alreadyHave)
            m_creature->MonsterWhisper("There are already some pieces of starter gear in your possession. You must destroy it first before I can give you more!", recipient);

        if (!allSuccess)
            m_creature->MonsterWhisper("You can't hold some of the gear I'm trying to give you! Make room for it first and speak to me again.", recipient);
        else if (!setId || setId == SET_ID_TERTIARY || setId == SET_ID_INSTANT_58)
            m_creature->MonsterWhisper("This equipment will serve you well in battle.", recipient);
    }

    void OfferPrimarySecondaryModChoices(Player* player, uint32 actionIdBase, uint32 specId)
    {
        switch (player->getClass())
        {
            case CLASS_WARRIOR:
                if (specId == 0 || specId == 1) // Arms/Fury
                {
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Ancestor (Str, Crit, Stam)", GOSSIP_SENDER_MAIN, actionIdBase + 54);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Battle (Str, Stam, Crit)", GOSSIP_SENDER_MAIN, actionIdBase + 56);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Beast (Str, Agil, Stam)", GOSSIP_SENDER_MAIN, actionIdBase + 41);
                }
                else if (specId == 2) // Protection
                {
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Blocking (Block, Str)", GOSSIP_SENDER_MAIN, actionIdBase + 47);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Champion (Str, Stam, Def)", GOSSIP_SENDER_MAIN, actionIdBase + 45);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Soldier (Str, Stam, Crit)", GOSSIP_SENDER_MAIN, actionIdBase + 43);
                }
                break;
            case CLASS_PALADIN:
                if (specId == 0) // Holy
                {
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Elder (Stam, Int, Mana Regen)", GOSSIP_SENDER_MAIN, actionIdBase + 44);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Mind (Spell Dmg/Heal, Spell Crit, Int)", GOSSIP_SENDER_MAIN, actionIdBase + 51);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Physician (Stam, Int, Spell Heal)", GOSSIP_SENDER_MAIN, actionIdBase + 37);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Vision (Spell Dmg/Heal, Int, Stam)", GOSSIP_SENDER_MAIN, actionIdBase + 53);
                }
                else if (specId == 1) // Protection
                {
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Champion (Str, Stam, Def)", GOSSIP_SENDER_MAIN, actionIdBase + 45);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Crusade (Spell Dmg/Heal, Int, Def)", GOSSIP_SENDER_MAIN, actionIdBase + 52);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Knight (Stam, Spell Dmg/Heal, Def)", GOSSIP_SENDER_MAIN, actionIdBase + 66);
                }
                else if (specId == 2) // Retribution
                {
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Ancestor (Str, Crit, Stam)", GOSSIP_SENDER_MAIN, actionIdBase + 54);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Beast (Str, Agil, Stam)", GOSSIP_SENDER_MAIN, actionIdBase + 41);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Soldier (Str, Stam, Crit)", GOSSIP_SENDER_MAIN, actionIdBase + 43);
                }
                break;
            case CLASS_PRIEST:
                if (specId == 0 || specId == 1) // Discipline/Holy
                {
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Elder (Stam, Int, Mana Regen)", GOSSIP_SENDER_MAIN, actionIdBase + 44);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Mind (Spell Dmg/Heal, Spell Crit, Int)", GOSSIP_SENDER_MAIN, actionIdBase + 51);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Physician (Stam, Int, Spell Heal)", GOSSIP_SENDER_MAIN, actionIdBase + 37);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Vision (Spell Dmg/Heal, Int, Stam)", GOSSIP_SENDER_MAIN, actionIdBase + 53);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Prophet (Int, Spirit, Spell Heal)", GOSSIP_SENDER_MAIN, actionIdBase + 38);
                }
                else if (specId == 2) // Shadow
                {
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Nightmare (Shadow Dmg, Stam, Int)", GOSSIP_SENDER_MAIN, actionIdBase + 55);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Elder (Stam, Int, Mana Regen)", GOSSIP_SENDER_MAIN, actionIdBase + 44);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Mind (Spell Dmg/Heal, Spell Crit, Int)", GOSSIP_SENDER_MAIN, actionIdBase + 51);
                }
                break;
            case CLASS_DRUID:
                if (specId == 0) // Balance
                {
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Elder (Stam, Int, Mana Regen)", GOSSIP_SENDER_MAIN, actionIdBase + 44);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Mind (Spell Dmg/Heal, Spell Crit, Int)", GOSSIP_SENDER_MAIN, actionIdBase + 51);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Vision (Spell Dmg/Heal, Int, Stam)", GOSSIP_SENDER_MAIN, actionIdBase + 53);
                }
                else if (specId == 1) // Feral
                {
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Bandit (Agi, Stam, Attack Power)", GOSSIP_SENDER_MAIN, actionIdBase + 40);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Beast (Str, Agil, Stam)", GOSSIP_SENDER_MAIN, actionIdBase + 41);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Champion (Str, Stam, Def)", GOSSIP_SENDER_MAIN, actionIdBase + 45);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Shadow (Attack Power, Agil, Stam)", GOSSIP_SENDER_MAIN, actionIdBase + 57);
                }
                else if (specId == 2) // Restoration
                {
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Elder (Stam, Int, Mana Regen)", GOSSIP_SENDER_MAIN, actionIdBase + 44);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Hierophant (Stam, Spirit, Spell Heal)", GOSSIP_SENDER_MAIN, actionIdBase + 42);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Prophet (Int, Spirit, Spell Heal)", GOSSIP_SENDER_MAIN, actionIdBase + 38);
                }
                break;
            case CLASS_SHAMAN:
                if (specId == 0) // Elemental
                {
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Invoker (Int, Spell Dmg/Heal, Spell Crit)", GOSSIP_SENDER_MAIN, actionIdBase + 39);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Elder (Stam, Int, Mana Regen)", GOSSIP_SENDER_MAIN, actionIdBase + 44);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Mind (Spell Dmg/Heal, Spell Crit, Int)", GOSSIP_SENDER_MAIN, actionIdBase + 51);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Vision (Spell Dmg/Heal, Int, Stam)", GOSSIP_SENDER_MAIN, actionIdBase + 53);
                }
                else if (specId == 1) // Enhancement
                {
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Ancestor (Str, Crit, Stam)", GOSSIP_SENDER_MAIN, actionIdBase + 54);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Beast (Str, Agil, Stam)", GOSSIP_SENDER_MAIN, actionIdBase + 41);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Hunt (Attack Power, Agil, Int)", GOSSIP_SENDER_MAIN, actionIdBase + 50);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Wild (Attack Power, Stam, Agil)", GOSSIP_SENDER_MAIN, actionIdBase + 60);
                }
                else if (specId == 2) // Restoration
                {
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Elder (Stam, Int, Mana Regen)", GOSSIP_SENDER_MAIN, actionIdBase + 44);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Mind (Spell Dmg/Heal, Spell Crit, Int)", GOSSIP_SENDER_MAIN, actionIdBase + 51);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Physician (Stam, Int, Spell Heal)", GOSSIP_SENDER_MAIN, actionIdBase + 37);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Prophet (Int, Spirit, Spell Heal)", GOSSIP_SENDER_MAIN, actionIdBase + 38);
                }
                break;
            case CLASS_WARLOCK: // All specs
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Invoker (Int, Spell Dmg/Heal, Spell Crit)", GOSSIP_SENDER_MAIN, actionIdBase + 39);
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Mind (Spell Dmg/Heal, Spell Crit, Int)", GOSSIP_SENDER_MAIN, actionIdBase + 51);
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Vision (Spell Dmg/Heal, Int, Stam)", GOSSIP_SENDER_MAIN, actionIdBase + 53);
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Nightmare (Shadow Dmg, Stam, Int)", GOSSIP_SENDER_MAIN, actionIdBase + 55);
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Sorcerer (Stam, Int, Spell Dmg/Heal)", GOSSIP_SENDER_MAIN, actionIdBase + 36);
                break;
            case CLASS_ROGUE:   // All specs
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Bandit (Agi, Stam, Attack Power)", GOSSIP_SENDER_MAIN, actionIdBase + 40);
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Shadow (Attack Power, Agil, Stam)", GOSSIP_SENDER_MAIN, actionIdBase + 57);
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Wild (Attack Power, Stam, Agil)", GOSSIP_SENDER_MAIN, actionIdBase + 60);
                break;
            case CLASS_HUNTER:  // All specs
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Hunt (Attack Power, Agil, Int)", GOSSIP_SENDER_MAIN, actionIdBase + 50);
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Bandit (Agil, Stam, Attack Power)", GOSSIP_SENDER_MAIN, actionIdBase + 40);
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Shadow (Attack Power, Agil, Stam)", GOSSIP_SENDER_MAIN, actionIdBase + 57);
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Wild (Attack Power, Stam, Agil)", GOSSIP_SENDER_MAIN, actionIdBase + 60);
                break;
            case CLASS_MAGE:    // All specs
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Invoker (Int, Spell Dmg/Heal, Spell Crit)", GOSSIP_SENDER_MAIN, actionIdBase + 39);
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Mind (Spell Dmg/Heal, Spell Crit, Int)", GOSSIP_SENDER_MAIN, actionIdBase + 51);
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Vision (Spell Dmg/Heal, Int, Stam)", GOSSIP_SENDER_MAIN, actionIdBase + 53);
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Sorcerer (Stam, Int, Spell Dmg/Heal)", GOSSIP_SENDER_MAIN, actionIdBase + 36);
                break;
        }
    }

    void OfferTertiaryModChoices(Player* player, uint32 actionIdBase, uint32 specId)
    {
        switch (player->getClass())
        {
            case CLASS_WARRIOR:
                if (specId == 0 || specId == 1) // Arms/Fury
                {
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Tiger (Str, Agil)", GOSSIP_SENDER_MAIN, actionIdBase + 14);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Bear (Str, Stam)", GOSSIP_SENDER_MAIN, actionIdBase + 7);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Boar (Str, Spirit)", GOSSIP_SENDER_MAIN, actionIdBase + 12);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Strength", GOSSIP_SENDER_MAIN, actionIdBase + 17);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Agility", GOSSIP_SENDER_MAIN, actionIdBase + 18);
                }
                else if (specId == 2) // Protection
                {
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Tiger (Str, Agil)", GOSSIP_SENDER_MAIN, actionIdBase + 14);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Bear (Str, Stam)", GOSSIP_SENDER_MAIN, actionIdBase + 7);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Boar (Str, Spirit)", GOSSIP_SENDER_MAIN, actionIdBase + 12);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Strength", GOSSIP_SENDER_MAIN, actionIdBase + 17);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Agility", GOSSIP_SENDER_MAIN, actionIdBase + 18);
                }
                break;
            case CLASS_PALADIN:
                if (specId == 0) // Holy
                {
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Eagle (Stam, Int)", GOSSIP_SENDER_MAIN, actionIdBase + 6);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Owl (Int, Spirit)", GOSSIP_SENDER_MAIN, actionIdBase + 9);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Intellect", GOSSIP_SENDER_MAIN, actionIdBase + 19);
                }
                else if (specId == 1) // Protection
                {
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Bear (Str, Stam)", GOSSIP_SENDER_MAIN, actionIdBase + 7);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Gorilla (Str, Int)", GOSSIP_SENDER_MAIN, actionIdBase + 10);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Eagle (Stam, Int)", GOSSIP_SENDER_MAIN, actionIdBase + 6);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Strength", GOSSIP_SENDER_MAIN, actionIdBase + 17);
                }
                else if (specId == 2) // Retribution
                {
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Bear (Str, Stam)", GOSSIP_SENDER_MAIN, actionIdBase + 7);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Gorilla (Str, Int)", GOSSIP_SENDER_MAIN, actionIdBase + 10);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Tiger (Str, Agil)", GOSSIP_SENDER_MAIN, actionIdBase + 14);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Strength", GOSSIP_SENDER_MAIN, actionIdBase + 17);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Agility", GOSSIP_SENDER_MAIN, actionIdBase + 18);
                }
                break;
            case CLASS_PRIEST: // All specs
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Owl (Int, Spirit)", GOSSIP_SENDER_MAIN, actionIdBase + 9);
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Whale (Stam, Spirit)", GOSSIP_SENDER_MAIN, actionIdBase + 8);
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Spirit", GOSSIP_SENDER_MAIN, actionIdBase + 15);
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Intellect", GOSSIP_SENDER_MAIN, actionIdBase + 19);
                break;
            case CLASS_DRUID:
                if (specId == 0) // Balance
                {
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Eagle (Stam, Int)", GOSSIP_SENDER_MAIN, actionIdBase + 6);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Whale (Stam, Spirit)", GOSSIP_SENDER_MAIN, actionIdBase + 8);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Spirit", GOSSIP_SENDER_MAIN, actionIdBase + 15);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Intellect", GOSSIP_SENDER_MAIN, actionIdBase + 19);
                }
                else if (specId == 1) // Feral
                {
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Bear (Str, Stam)", GOSSIP_SENDER_MAIN, actionIdBase + 7);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Monkey (Agil, Stam)", GOSSIP_SENDER_MAIN, actionIdBase + 5);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Tiger (Str, Agil)", GOSSIP_SENDER_MAIN, actionIdBase + 14);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Agility", GOSSIP_SENDER_MAIN, actionIdBase + 18);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Strength", GOSSIP_SENDER_MAIN, actionIdBase + 17);
                }
                else if (specId == 2) // Restoration
                {
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Eagle (Stam, Int)", GOSSIP_SENDER_MAIN, actionIdBase + 6);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Whale (Stam, Spirit)", GOSSIP_SENDER_MAIN, actionIdBase + 8);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Spirit", GOSSIP_SENDER_MAIN, actionIdBase + 15);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Intellect", GOSSIP_SENDER_MAIN, actionIdBase + 19);
                }
                break;
            case CLASS_SHAMAN:
                if (specId == 0) // Elemental
                {
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Eagle (Stam, Int)", GOSSIP_SENDER_MAIN, actionIdBase + 6);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Owl (Int, Spirit)", GOSSIP_SENDER_MAIN, actionIdBase + 9);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Intellect", GOSSIP_SENDER_MAIN, actionIdBase + 19);
                }
                else if (specId == 1) // Enhancement
                {
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Falcon (Agil, Int)", GOSSIP_SENDER_MAIN, actionIdBase + 11);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Monkey (Agil, Stam)", GOSSIP_SENDER_MAIN, actionIdBase + 5);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Tiger (Str, Agil)", GOSSIP_SENDER_MAIN, actionIdBase + 14);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Gorilla (Str, Int)", GOSSIP_SENDER_MAIN, actionIdBase + 10);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Wolf (Agil, Spirit)", GOSSIP_SENDER_MAIN, actionIdBase + 13);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Agility", GOSSIP_SENDER_MAIN, actionIdBase + 18);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Strength", GOSSIP_SENDER_MAIN, actionIdBase + 17);
                }
                else if (specId == 2) // Restoration
                {
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Eagle (Stam, Int)", GOSSIP_SENDER_MAIN, actionIdBase + 6);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Owl (Int, Spirit)", GOSSIP_SENDER_MAIN, actionIdBase + 9);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Spirit", GOSSIP_SENDER_MAIN, actionIdBase + 15);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Intellect", GOSSIP_SENDER_MAIN, actionIdBase + 19);
                }
                break;
            case CLASS_WARLOCK: // All specs
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Eagle (Stam, Int)", GOSSIP_SENDER_MAIN, actionIdBase + 6);
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Whale (Stam, Spirit)", GOSSIP_SENDER_MAIN, actionIdBase + 8);
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Intellect", GOSSIP_SENDER_MAIN, actionIdBase + 19);
                break;
            case CLASS_ROGUE:   // All specs
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Monkey (Agil, Stam)", GOSSIP_SENDER_MAIN, actionIdBase + 5);
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Tiger (Str, Agil)", GOSSIP_SENDER_MAIN, actionIdBase + 14);
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Agility", GOSSIP_SENDER_MAIN, actionIdBase + 18);
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Strength", GOSSIP_SENDER_MAIN, actionIdBase + 17);
                break;
            case CLASS_HUNTER:  // All specs
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Monkey (Agil, Stam)", GOSSIP_SENDER_MAIN, actionIdBase + 5);
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Falcon (Agil, Int)", GOSSIP_SENDER_MAIN, actionIdBase + 11);
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Wolf (Agil, Spirit)", GOSSIP_SENDER_MAIN, actionIdBase + 13);
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Intellect", GOSSIP_SENDER_MAIN, actionIdBase + 19);
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Agility", GOSSIP_SENDER_MAIN, actionIdBase + 18);
                break;
            case CLASS_MAGE:    // All specs
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Eagle (Stam, Int)", GOSSIP_SENDER_MAIN, actionIdBase + 6);
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Owl (Int, Spirit)", GOSSIP_SENDER_MAIN, actionIdBase + 9);
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Spirit", GOSSIP_SENDER_MAIN, actionIdBase + 15);
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Intellect", GOSSIP_SENDER_MAIN, actionIdBase + 19);
                break;
        }
        player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Stamina", GOSSIP_SENDER_MAIN, actionIdBase + 16); // all classes/specs may opt for this here
    }

    bool TaxiNodesKnown(Player& player) const
    {
        switch (uint32(player.GetTeam()))
        {
            case ALLIANCE:
                for (uint32 node : Instant58TaxiNodesAlliance)
                {
                    if (!player.m_taxi.IsTaximaskNodeKnown(node))
                        return false;
                }
                break;
            case HORDE:
                for (uint32 node : Instant58TaxiNodesHorde)
                {
                    if (!player.m_taxi.IsTaximaskNodeKnown(node))
                        return false;
                }
                break;
        }
        return true;
    }

    void TaxiNodesTeach(Player& player) const
    {
        // Vanilla taxi nodes up to 2.0.1 progression patch

        switch (uint32(player.GetTeam()))
        {
            case ALLIANCE:
                for (uint32 node : Instant58TaxiNodesAlliance)
                    player.m_taxi.SetTaximaskNode(node);
                break;
            case HORDE:
                for (uint32 node : Instant58TaxiNodesHorde)
                    player.m_taxi.SetTaximaskNode(node);
                break;
        }
    }
};

bool GossipHello_npc_enlistment_officer(Player* player, Creature* creature)
{
    if (npc_enlistment_officerAI* enlistmentOfficerAI = dynamic_cast<npc_enlistment_officerAI*>(creature->AI()))
    {
        if ((creature->GetEntry() == NPC_ALLIANCE_OFFICER || creature->GetEntry() == NPC_HORDE_OFFICER) && player->getLevel() < 58)
        {
            player->SEND_GOSSIP_MENU(GOSSIP_TEXT_ENLIST_REFUSE_LOW, creature->GetObjectGuid());
            return true;
        }

        player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TRAINER, "Train talent spell ranks", GOSSIP_SENDER_MAIN, GOSSIP_OPTION_TRAINER);

        if (creature->GetEntry() == NPC_MASTER_PROVISIONER)
        {
            if (player->getLevel() >= 60)
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "|cFF0008E8Full \"best in slot\" gear - Classic|r", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2000);
            if (player->getLevel() == 70)
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "|cFF0008E8Full \"best in slot\" gear - TBC|r", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 3000);

            if (enlistmentOfficerAI->HasStarterSet(player, enlistmentOfficerAI->FullGearListBiS60))
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "|cFF8B0000Remove all level 60 BiS gear.|r", GOSSIP_SENDER_MAIN, 93);
            if (enlistmentOfficerAI->HasStarterSet(player, enlistmentOfficerAI->FullGearListBiS70))
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "|cFF8B0000Remove all level 70 BiS gear.|r", GOSSIP_SENDER_MAIN, 94);

            if (enlistmentOfficerAI->HasStarterSet(player, enlistmentOfficerAI->FullGearList))
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "|cFF8B0000Remove all custom uncommon gear.|r", GOSSIP_SENDER_MAIN, 95);
            else if (player->getLevel() == 70)
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "Build starter set of uncommon quality level 70 gear with custom stat modifiers", GOSSIP_SENDER_MAIN, 99);

            uint32 groundMount = enlistmentOfficerAI->GetStarterEpicMountForRace(player);
            // Flying Mount - Swift Blue Gryphon / Swift Red Wind Rider
            uint32 flyingMount = player->GetTeam() == ALLIANCE ? 25473 : 25477;
            if ((!player->HasItemCount(groundMount, 1) || !player->HasItemCount(flyingMount, 1)) && player->getLevel() == 70)
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "I seem to be missing my mount. Can you give me one?", GOSSIP_SENDER_MAIN, 96);

            if (creature->GetMapId() == 530)
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TAXI, "Teleport to Blasted Lands (Dark Portal)", GOSSIP_SENDER_MAIN, 90);
            else
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TAXI, "Teleport to Shattrath City", GOSSIP_SENDER_MAIN, 91);

            if (player->getLevel() < 70)
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TAXI, "Teleport to Test Realm Overlord (Starting Area)", GOSSIP_SENDER_MAIN, 89);
        }
        else
        {
            if (enlistmentOfficerAI->HasStarterSet(player, enlistmentOfficerAI->FullGearListInstant58))
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "|cFF8B0000Remove all level 58 starter gear.|r", GOSSIP_SENDER_MAIN, 92);
            else
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "I need my starter gear set!", GOSSIP_SENDER_MAIN, 99);

            uint32 checkMount = enlistmentOfficerAI->GetStarterMountForRace(player);
            if (!player->HasItemCount(checkMount, 1))
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "I seem to be missing my mount. Can you give me one?", GOSSIP_SENDER_MAIN, 96);
        }

        if (!enlistmentOfficerAI->TaxiNodesKnown(*player))
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TAXI, "Learn Azeroth flight paths", GOSSIP_SENDER_MAIN, 97);

        player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "Basic Supplies", GOSSIP_SENDER_MAIN, GOSSIP_OPTION_VENDOR);

        if (player->getLevel() <= 60 || creature->GetEntry() == NPC_MASTER_PROVISIONER)
        {
            player->ADD_GOSSIP_ITEM_EXTENDED(GOSSIP_ICON_CHAT, "Unlearn Talents", GOSSIP_SENDER_MAIN, 500, "Are you sure?", 0, false);

            if (player->getClass() == CLASS_HUNTER)
            {
                if (!player->GetPet())
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Pet Menu", GOSSIP_SENDER_MAIN, 30);
                else if (player->GetPet()->m_spells.size() >= 1)
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Untrain Pet", GOSSIP_SENDER_MAIN, 499);
            }
        }

        if ((creature->GetEntry() == NPC_ALLIANCE_OFFICER || creature->GetEntry() == NPC_HORDE_OFFICER) && player->getLevel() > 58)
            player->SEND_GOSSIP_MENU(GOSSIP_TEXT_ENLIST_REFUSE_HIGH, creature->GetObjectGuid());
        else if (creature->GetEntry() == NPC_MASTER_PROVISIONER && player->getLevel() < 60)
            player->SEND_GOSSIP_MENU(GOSSIP_TEXT_REFUSE_LOW_70, creature->GetObjectGuid());
        else if (creature->GetEntry() == NPC_ALLIANCE_OFFICER || creature->GetEntry() == NPC_HORDE_OFFICER)
            player->SEND_GOSSIP_MENU(GOSSIP_TEXT_ENLIST_GREET, creature->GetObjectGuid());
        else if (creature->GetEntry() == NPC_MASTER_PROVISIONER)
        {
            if (player->GetMoney() < 100000000)
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_MONEY_BAG, "I need money!", GOSSIP_SENDER_MAIN, 600);
            player->ADD_GOSSIP_ITEM_EXTENDED(GOSSIP_ICON_INTERACT_1, "Reset my dungeon/raid lockouts.", GOSSIP_SENDER_MAIN, 700, "Are you sure?", 0, false);
            player->SEND_GOSSIP_MENU(GOSSIP_TEXT_GREET_70, creature->GetObjectGuid());
        }
    }
    return true;
}

bool GossipSelect_npc_enlistment_officer(Player* player, Creature* creature, uint32 /*uiSender*/, uint32 action)
{
    player->PlayerTalkClass->ClearMenus();
    Pet* pPet = player->GetPet();

    if (npc_enlistment_officerAI* enlistmentOfficerAI = dynamic_cast<npc_enlistment_officerAI*>(creature->AI()))
    {
        // Main Menu
        if (action == 29)
            GossipHello_npc_enlistment_officer(player, creature);

        // Pets - Page 1
        else if (action == 30)
        {
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "<- Back to Main Menu", GOSSIP_SENDER_MAIN, 29);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Next Page ->", GOSSIP_SENDER_MAIN, 31);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Bat", GOSSIP_SENDER_MAIN, 501);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Bear", GOSSIP_SENDER_MAIN, 502);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Boar", GOSSIP_SENDER_MAIN, 503);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Carrion Bird", GOSSIP_SENDER_MAIN, 504);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Cat", GOSSIP_SENDER_MAIN, 505);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Crab", GOSSIP_SENDER_MAIN, 506);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Crocolisk", GOSSIP_SENDER_MAIN, 507);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Gorilla", GOSSIP_SENDER_MAIN, 508);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Hyena", GOSSIP_SENDER_MAIN, 509);
            player->SEND_GOSSIP_MENU(GOSSIP_TEXT_ENLIST_PET_MENU, creature->GetObjectGuid());
        }

        // Pets - Page 2
        else if (action == 31)
        {
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "<- Back to Main Menu", GOSSIP_SENDER_MAIN, 29);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "<- Previous Page", GOSSIP_SENDER_MAIN, 30);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Owl", GOSSIP_SENDER_MAIN, 510);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Raptor", GOSSIP_SENDER_MAIN, 511);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Ravager", GOSSIP_SENDER_MAIN, 512);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Strider", GOSSIP_SENDER_MAIN, 513);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Scorpid", GOSSIP_SENDER_MAIN, 514);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Serpent", GOSSIP_SENDER_MAIN, 515);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Spider", GOSSIP_SENDER_MAIN, 516);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Turtle", GOSSIP_SENDER_MAIN, 517);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Wolf", GOSSIP_SENDER_MAIN, 518);
            player->SEND_GOSSIP_MENU(GOSSIP_TEXT_ENLIST_PET_MENU, creature->GetObjectGuid());
        }

        else if (action == GOSSIP_OPTION_VENDOR)
            player->GetSession()->SendListInventory(creature->GetObjectGuid());

        else if (action == GOSSIP_OPTION_TRAINER)
            player->GetSession()->SendTrainerList(creature->GetObjectGuid());

        // Teleport - Test Realm Overlord
        else if (action == 89)
        {
            player->CLOSE_GOSSIP_MENU();
            player->CastSpell(player, SPELL_TELEPORT_VISUAL, TRIGGERED_OLD_TRIGGERED);
            switch (player->getRace())
            {
            case RACE_HUMAN:
                player->TeleportTo(teleLocs[4].map, teleLocs[4].x, teleLocs[4].y, teleLocs[4].z, teleLocs[4].o);
                break;
            case RACE_DWARF:
            case RACE_GNOME:
                player->TeleportTo(teleLocs[5].map, teleLocs[5].x, teleLocs[5].y, teleLocs[5].z, teleLocs[5].o);
                break;
            case RACE_NIGHTELF:
                player->TeleportTo(teleLocs[6].map, teleLocs[6].x, teleLocs[6].y, teleLocs[6].z, teleLocs[6].o);
                break;
            case RACE_DRAENEI:
                player->TeleportTo(teleLocs[7].map, teleLocs[7].x, teleLocs[7].y, teleLocs[7].z, teleLocs[7].o);
                break;
            case RACE_TROLL:
            case RACE_ORC:
                player->TeleportTo(teleLocs[8].map, teleLocs[8].x, teleLocs[8].y, teleLocs[8].z, teleLocs[8].o);
                break;
            case RACE_UNDEAD:
                player->TeleportTo(teleLocs[9].map, teleLocs[9].x, teleLocs[9].y, teleLocs[9].z, teleLocs[9].o);
                break;
            case RACE_TAUREN:
                player->TeleportTo(teleLocs[10].map, teleLocs[10].x, teleLocs[10].y, teleLocs[10].z, teleLocs[10].o);
                break;
            case RACE_BLOODELF:
                player->TeleportTo(teleLocs[11].map, teleLocs[11].x, teleLocs[11].y, teleLocs[11].z, teleLocs[11].o);
                break;
            }
        }
        // Teleport - Blasted Lands (Dark Portal)
        else if (action == 90)
        {
            player->CLOSE_GOSSIP_MENU();
            player->CastSpell(player, SPELL_TELEPORT_VISUAL, TRIGGERED_OLD_TRIGGERED);
            if (player->GetTeam() == ALLIANCE)
                player->TeleportTo(teleLocs[0].map, teleLocs[0].x, teleLocs[0].y, teleLocs[0].z, teleLocs[0].o);
            else
                player->TeleportTo(teleLocs[1].map, teleLocs[1].x, teleLocs[1].y, teleLocs[1].z, teleLocs[1].o);
        }
        // Teleport - Shattrath City
        else if (action == 91)
        {
            player->CLOSE_GOSSIP_MENU();
            player->CastSpell(player, SPELL_TELEPORT_VISUAL, TRIGGERED_OLD_TRIGGERED);
            if (player->GetTeam() == ALLIANCE)
                player->TeleportTo(teleLocs[2].map, teleLocs[2].x, teleLocs[2].y, teleLocs[2].z, teleLocs[2].o);
            else
                player->TeleportTo(teleLocs[3].map, teleLocs[3].x, teleLocs[3].y, teleLocs[3].z, teleLocs[3].o);
        }

        // Remove Starter Set - Instant 58
        else if (action == 92)
        {
            enlistmentOfficerAI->RemoveStarterSet(player, enlistmentOfficerAI->FullGearListInstant58);
            player->CLOSE_GOSSIP_MENU();
        }
        // Remove Starter Set - BiS 60
        else if (action == 93)
        {
            enlistmentOfficerAI->RemoveStarterSet(player, enlistmentOfficerAI->FullGearListBiS60);
            player->CLOSE_GOSSIP_MENU();
        }
        // Remove Starter Set - BiS 70
        else if (action == 94)
        {
            enlistmentOfficerAI->RemoveStarterSet(player, enlistmentOfficerAI->FullGearListBiS70);
            player->CLOSE_GOSSIP_MENU();
        }
        // Remove Starter Set - Custom 70 Uncommon
        else if (action == 95)
        {
            enlistmentOfficerAI->RemoveStarterSet(player, enlistmentOfficerAI->FullGearList);
            player->CLOSE_GOSSIP_MENU();
        }

        // Add Starter Mount
        else if (action == 96)
        {
            if (creature->GetEntry() == NPC_MASTER_PROVISIONER) {
                uint32 groundMount = enlistmentOfficerAI->GetStarterEpicMountForRace(player);
                if (!player->HasItemCount(groundMount, 1)) player->StoreNewItemInBestSlots(groundMount, 1);

                // Flying Mount - Swift Blue Gryphon / Swift Red Wind Rider
                uint32 flyingMount = player->GetTeam() == ALLIANCE ? 25473 : 25477;
                if (!player->HasItemCount(flyingMount, 1) && player->getLevel() == 70) player->StoreNewItemInBestSlots(flyingMount, 1);
            }
            else {
                uint32 addMount = enlistmentOfficerAI->GetStarterMountForRace(player);
                player->StoreNewItemInBestSlots(addMount, 1);
            }

            player->PlayerTalkClass->CloseGossip();
        }

        // Learn Flight Paths
        else if (action == 97)
        {
            enlistmentOfficerAI->TaxiNodesTeach(*player);
            player->SetFacingToObject(creature);
            player->HandleEmote(EMOTE_ONESHOT_SALUTE);
            player->CastSpell(player, SPELL_CYCLONE_VISUAL_SPAWN, TRIGGERED_OLD_TRIGGERED);
            player->PlayerTalkClass->CloseGossip();
        }

        // Choose Instant 70 Starter Set
        else if (action == 99 && creature->GetEntry() == NPC_MASTER_PROVISIONER)
        {
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "<- Back to Main Menu", GOSSIP_SENDER_MAIN, 29);
            switch (player->getClass())
            {
                case CLASS_WARRIOR:
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Arms", GOSSIP_SENDER_MAIN, 100);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Fury", GOSSIP_SENDER_MAIN, 101);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Protection", GOSSIP_SENDER_MAIN, 102);
                    break;
                case CLASS_PALADIN:
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Holy", GOSSIP_SENDER_MAIN, 100);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Protection", GOSSIP_SENDER_MAIN, 101);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Retribution", GOSSIP_SENDER_MAIN, 102);
                    break;
                case CLASS_PRIEST:
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Discipline", GOSSIP_SENDER_MAIN, 100);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Holy", GOSSIP_SENDER_MAIN, 101);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Shadow", GOSSIP_SENDER_MAIN, 102);
                    break;
                case CLASS_DRUID:
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Balance", GOSSIP_SENDER_MAIN, 100);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Feral", GOSSIP_SENDER_MAIN, 101);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Restoration", GOSSIP_SENDER_MAIN, 102);
                    break;
                case CLASS_SHAMAN:
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Elemental", GOSSIP_SENDER_MAIN, 100);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Enhancement", GOSSIP_SENDER_MAIN, 101);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Restoration", GOSSIP_SENDER_MAIN, 102);
                    break;
                case CLASS_ROGUE:
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Assassination", GOSSIP_SENDER_MAIN, 100);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Combat", GOSSIP_SENDER_MAIN, 101);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Subtlety", GOSSIP_SENDER_MAIN, 102);
                    break;

                // these only give the illusion of choice
                case CLASS_WARLOCK:
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Affliction", GOSSIP_SENDER_MAIN, 100);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Demonology", GOSSIP_SENDER_MAIN, 100);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Destruction", GOSSIP_SENDER_MAIN, 100);
                    break;
                case CLASS_HUNTER:
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Beast Mastery", GOSSIP_SENDER_MAIN, 100);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Marksmanship", GOSSIP_SENDER_MAIN, 100);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Survival", GOSSIP_SENDER_MAIN, 100);
                    break;
                case CLASS_MAGE:
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Arcane", GOSSIP_SENDER_MAIN, 100);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Fire", GOSSIP_SENDER_MAIN, 100);
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Frost", GOSSIP_SENDER_MAIN, 100);
                    break;
            }
            player->SEND_GOSSIP_MENU(GOSSIP_TEXT_PICK_SPEC, creature->GetObjectGuid());
        }
        // Give Instant 58 Starter Set
        else if (action == 99 && (creature->GetEntry() == NPC_ALLIANCE_OFFICER || creature->GetEntry() == NPC_HORDE_OFFICER))
        {
            enlistmentOfficerAI->AddStarterSet(player, creature, SET_ID_INSTANT_58);
            player->PlayerTalkClass->CloseGossip();
        }

        // Primary - Specialization 1
        else if (action == 100)
        {
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "<- Back to Spec Selection", GOSSIP_SENDER_MAIN, 99);
            enlistmentOfficerAI->OfferPrimarySecondaryModChoices(player, 200, 0);
            player->SEND_GOSSIP_MENU(GOSSIP_TEXT_PICK_GEAR_1, creature->GetObjectGuid());
        }
        // Secondary - Specialization 1
        else if (action > 200 && action < 300)
        {
            enlistmentOfficerAI->AddStarterSet(player, creature, SET_ID_PRIMARY, action - 200);
            enlistmentOfficerAI->OfferPrimarySecondaryModChoices(player, 300, 0);
            player->SEND_GOSSIP_MENU(GOSSIP_TEXT_PICK_GEAR_2, creature->GetObjectGuid());
        }
        // Tertiary - Specialization 1
        else if (action > 300 && action < 400)
        {
            enlistmentOfficerAI->AddStarterSet(player, creature, SET_ID_SECONDARY, action - 300);
            enlistmentOfficerAI->OfferTertiaryModChoices(player, 600, 0);
            player->SEND_GOSSIP_MENU(GOSSIP_TEXT_PICK_GEAR_3, creature->GetObjectGuid());
        }
        // Tertiary - Specialization 1 (cont.)
        else if (action > 600 && action < 700)
        {
            enlistmentOfficerAI->AddStarterSet(player, creature, SET_ID_TERTIARY, action - 600, 0);
            player->PlayerTalkClass->CloseGossip();
        }

        // Primary - Specialization 2
        else if (action == 101)
        {
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "<- Back to Spec Selection", GOSSIP_SENDER_MAIN, 99);
            enlistmentOfficerAI->OfferPrimarySecondaryModChoices(player, 700, 1);
            player->SEND_GOSSIP_MENU(GOSSIP_TEXT_PICK_GEAR_1, creature->GetObjectGuid());
        }
        // Secondary - Specialization 2
        else if (action > 700 && action < 800)
        {
            enlistmentOfficerAI->AddStarterSet(player, creature, SET_ID_PRIMARY, action - 700);
            enlistmentOfficerAI->OfferPrimarySecondaryModChoices(player, 800, 1);
            player->SEND_GOSSIP_MENU(GOSSIP_TEXT_PICK_GEAR_2, creature->GetObjectGuid());
        }
        // Tertiary - Specialization 2
        else if (action > 800 && action < 900)
        {
            enlistmentOfficerAI->AddStarterSet(player, creature, SET_ID_SECONDARY, action - 800);
            enlistmentOfficerAI->OfferTertiaryModChoices(player, 900, 1);
            player->SEND_GOSSIP_MENU(GOSSIP_TEXT_PICK_GEAR_3, creature->GetObjectGuid());
        }
        // Tertiary - Specialization 2 (cont.)
        else if (action > 900 && action < 1000)
        {
            enlistmentOfficerAI->AddStarterSet(player, creature, SET_ID_TERTIARY, action - 900, 1);
            player->PlayerTalkClass->CloseGossip();
        }

        // Primary - Specialization 3
        else if (action == 102)
        {
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "<- Back to Spec Selection", GOSSIP_SENDER_MAIN, 99);
            enlistmentOfficerAI->OfferPrimarySecondaryModChoices(player, 1000, 2);
            player->SEND_GOSSIP_MENU(GOSSIP_TEXT_PICK_GEAR_1, creature->GetObjectGuid());
        }
        // Secondary - Specialization 3
        else if (action > 1000 && action < 1100)
        {
            enlistmentOfficerAI->AddStarterSet(player, creature, SET_ID_PRIMARY, action - 1000);
            enlistmentOfficerAI->OfferPrimarySecondaryModChoices(player, 1100, 2);
            player->SEND_GOSSIP_MENU(GOSSIP_TEXT_PICK_GEAR_2, creature->GetObjectGuid());
        }
        // Tertiary - Specialization 3
        else if (action > 1100 && action < 1200)
        {
            enlistmentOfficerAI->AddStarterSet(player, creature, SET_ID_SECONDARY, action - 1100);
            enlistmentOfficerAI->OfferTertiaryModChoices(player, 1200, 2);
            player->SEND_GOSSIP_MENU(GOSSIP_TEXT_PICK_GEAR_3, creature->GetObjectGuid());
        }
        // Tertiary - Specialization 3 (cont.)
        else if (action > 1200 && action < 1300)
        {
            enlistmentOfficerAI->AddStarterSet(player, creature, SET_ID_TERTIARY, action - 1200, 2);
            player->PlayerTalkClass->CloseGossip();
        }

        // Untrain Pet
        else if (action == 499)
        {
            player->PlayerTalkClass->CloseGossip();

            if (pPet && pPet->m_spells.size() >= 1)
            {
                CharmInfo* charmInfo = pPet->GetCharmInfo();
                if (charmInfo)
                {
                    for (PetSpellMap::iterator itr = pPet->m_spells.begin(); itr != pPet->m_spells.end();)
                    {
                        uint32 spell_id = itr->first;
                        ++itr;
                        pPet->unlearnSpell(spell_id, false);
                    }

                    uint32 cost = pPet->resetTalentsCost();

                    pPet->SetTP(pPet->getLevel() * (pPet->GetLoyaltyLevel() - 1));

                    for (int i = 0; i < MAX_UNIT_ACTION_BAR_INDEX; ++i)
                        if (UnitActionBarEntry const* ab = charmInfo->GetActionBarEntry(i))
                            if (ab->GetAction() && ab->IsActionBarForSpell())
                                charmInfo->SetActionBar(i, 0, ACT_DISABLED);

                    // relearn pet passives
                    pPet->LearnPetPassives();

                    pPet->m_resetTalentsTime = time(nullptr);
                    pPet->m_resetTalentsCost = cost;

                    player->PetSpellInitialize();
                }
            }
        }

        // Unlearn Talents
        else if (action == 500)
        {
            player->PlayerTalkClass->CloseGossip();
            player->resetTalents(true);
        }
        // Money!
        else if (action == 600)
        {
            player->PlayerTalkClass->CloseGossip();
            player->ModifyMoney(INT_MAX);
        }
        // Unbind Instances
        else if (action == 700)
        {
            player->PlayerTalkClass->CloseGossip();
            for (uint8 i = 0; i < MAX_DIFFICULTY; ++i)
            {
                Player::BoundInstancesMap& binds = player->GetBoundInstances(Difficulty(i));
                for (Player::BoundInstancesMap::iterator itr = binds.begin(); itr != binds.end();)
                {
                    if (itr->first != player->GetMapId())
                    {
                        DungeonPersistentState* save = itr->second.state;
                        std::string timeleft = secsToTimeString(save->GetResetTime() - time(nullptr), true);
                        player->UnbindInstance(itr, Difficulty(i));
                    }
                    else
                        ++itr;
                }
            }
        }

        else if (action == 501) enlistmentOfficerAI->CreatePet(player, creature, NPC_PET_BAT);
        else if (action == 502) enlistmentOfficerAI->CreatePet(player, creature, NPC_PET_BEAR);
        else if (action == 503) enlistmentOfficerAI->CreatePet(player, creature, NPC_PET_BOAR);
        else if (action == 504) enlistmentOfficerAI->CreatePet(player, creature, NPC_PET_CARRION);
        else if (action == 505) enlistmentOfficerAI->CreatePet(player, creature, NPC_PET_CAT);
        else if (action == 506) enlistmentOfficerAI->CreatePet(player, creature, NPC_PET_CRAB);
        else if (action == 507) enlistmentOfficerAI->CreatePet(player, creature, NPC_PET_CROC);
        else if (action == 508) enlistmentOfficerAI->CreatePet(player, creature, NPC_PET_GORILLA);
        else if (action == 509) enlistmentOfficerAI->CreatePet(player, creature, NPC_PET_HYENA);
        else if (action == 510) enlistmentOfficerAI->CreatePet(player, creature, NPC_PET_OWL);
        else if (action == 511) enlistmentOfficerAI->CreatePet(player, creature, NPC_PET_RAPTOR);
        else if (action == 512) enlistmentOfficerAI->CreatePet(player, creature, NPC_PET_RAVAGER);
        else if (action == 513) enlistmentOfficerAI->CreatePet(player, creature, NPC_PET_STRIDER);
        else if (action == 514) enlistmentOfficerAI->CreatePet(player, creature, NPC_PET_SCORPID);
        else if (action == 515) enlistmentOfficerAI->CreatePet(player, creature, NPC_PET_SERPENT);
        else if (action == 516) enlistmentOfficerAI->CreatePet(player, creature, NPC_PET_SPIDER);
        else if (action == 517) enlistmentOfficerAI->CreatePet(player, creature, NPC_PET_TURTLE);
        else if (action == 518) enlistmentOfficerAI->CreatePet(player, creature, NPC_PET_WOLF);

        // Classic Full Best in Slot
        // Pure DPS classes Hunter, Rogue, Mage, and Warlock don't require a secondary selection
        switch (action)
        {
        case GOSSIP_ACTION_INFO_DEF + 2000:
        {
            switch (player->getClass())
            {
            case CLASS_ROGUE:
                enlistmentOfficerAI->GivePlayerItems(player, Lvl60BiS_Rogue);
                player->CLOSE_GOSSIP_MENU();
                break;
            case CLASS_MAGE:
                enlistmentOfficerAI->GivePlayerItems(player, Lvl60BiS_Mage);
                player->CLOSE_GOSSIP_MENU();
                break;
            case CLASS_HUNTER:
                enlistmentOfficerAI->GivePlayerItems(player, Lvl60BiS_Hunter);
                player->CLOSE_GOSSIP_MENU();
                break;
            case CLASS_WARLOCK:
                enlistmentOfficerAI->GivePlayerItems(player, Lvl60BiS_Warlock);
                player->CLOSE_GOSSIP_MENU();
                break;
            case CLASS_SHAMAN:
                player->PrepareGossipMenu(creature, GOSSIP_TEXT_PICK_SPEC);
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Restoration", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2001);
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Enhancement", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2002);
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Elemental", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2003);
                player->SendPreparedGossip(creature);
                break;
            case CLASS_PRIEST:
                player->PrepareGossipMenu(creature, GOSSIP_TEXT_PICK_SPEC);
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Shadow", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2004);
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Discipline/Holy", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2005);
                player->SendPreparedGossip(creature);
                break;
            case CLASS_PALADIN:
                player->PrepareGossipMenu(creature, GOSSIP_TEXT_PICK_SPEC);
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Holy", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2006);
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Retribution", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2007);
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Protection", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2008);
                player->SendPreparedGossip(creature);
                break;
            case CLASS_WARRIOR:
                player->PrepareGossipMenu(creature, GOSSIP_TEXT_PICK_SPEC);
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Fury/Arms", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2009);
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Protection", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2010);
                player->SendPreparedGossip(creature);
                break;
            case CLASS_DRUID:
                player->PrepareGossipMenu(creature, GOSSIP_TEXT_PICK_SPEC);
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Feral (Cat/DPS)", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2011);
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Feral (Bear/Tank)", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2012);
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Balance", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2013);
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Restoration", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2014);
                player->SendPreparedGossip(creature);
                break;
            }
            break;
        }
        // Classic Full Best in Slot (Spec Selected)
        // Shaman - Restoration
        case GOSSIP_ACTION_INFO_DEF + 2001: enlistmentOfficerAI->GivePlayerItems(player, Lvl60BiS_ShamanResto); player->PlayerTalkClass->CloseGossip(); break;
        // Shaman - Enhancement
        case GOSSIP_ACTION_INFO_DEF + 2002: enlistmentOfficerAI->GivePlayerItems(player, Lvl60BiS_ShamanEnhancement); player->PlayerTalkClass->CloseGossip(); break;
        // Shaman - Elemental
        case GOSSIP_ACTION_INFO_DEF + 2003: enlistmentOfficerAI->GivePlayerItems(player, Lvl60BiS_ShamanElemental); player->PlayerTalkClass->CloseGossip(); break;
        // Priest - Shadow
        case GOSSIP_ACTION_INFO_DEF + 2004: enlistmentOfficerAI->GivePlayerItems(player, Lvl60BiS_PriestShadow); player->PlayerTalkClass->CloseGossip(); break;
        // Priest - Discipline/Holy
        case GOSSIP_ACTION_INFO_DEF + 2005: enlistmentOfficerAI->GivePlayerItems(player, Lvl60BiS_PriestDiscHoly); player->PlayerTalkClass->CloseGossip(); break;
        // Paladin - Holy
        case GOSSIP_ACTION_INFO_DEF + 2006: enlistmentOfficerAI->GivePlayerItems(player, Lvl60BiS_PaladinHoly); player->PlayerTalkClass->CloseGossip(); break;
        // Paladin - Retribution
        case GOSSIP_ACTION_INFO_DEF + 2007: enlistmentOfficerAI->GivePlayerItems(player, Lvl60BiS_PaladinRetribution); player->PlayerTalkClass->CloseGossip(); break;
        // Paladin - Protection
        case GOSSIP_ACTION_INFO_DEF + 2008: enlistmentOfficerAI->GivePlayerItems(player, Lvl60BiS_PaladinProtection); player->PlayerTalkClass->CloseGossip(); break;
        // Warrior - Fury/Arms
        case GOSSIP_ACTION_INFO_DEF + 2009: enlistmentOfficerAI->GivePlayerItems(player, Lvl60BiS_WarriorFuryArms); player->PlayerTalkClass->CloseGossip(); break;
        // Warrior - Protection
        case GOSSIP_ACTION_INFO_DEF + 2010: enlistmentOfficerAI->GivePlayerItems(player, Lvl60BiS_WarriorProtection); player->PlayerTalkClass->CloseGossip(); break;
        // Druid - Feral (Cat/DPS)
        case GOSSIP_ACTION_INFO_DEF + 2011: enlistmentOfficerAI->GivePlayerItems(player, Lvl60BiS_DruidFeralCat); player->PlayerTalkClass->CloseGossip(); break;
        // Druid - Feral (Bear/Tank)
        case GOSSIP_ACTION_INFO_DEF + 2012: enlistmentOfficerAI->GivePlayerItems(player, Lvl60BiS_DruidFeralBear); player->PlayerTalkClass->CloseGossip(); break;
        // Druid - Balance
        case GOSSIP_ACTION_INFO_DEF + 2013: enlistmentOfficerAI->GivePlayerItems(player, Lvl60BiS_DruidBalance); player->PlayerTalkClass->CloseGossip(); break;
        // Druid - Restoration
        case GOSSIP_ACTION_INFO_DEF + 2014: enlistmentOfficerAI->GivePlayerItems(player, Lvl60BiS_DruidResto); player->PlayerTalkClass->CloseGossip(); break;

        // TBC Full Best in Slot
        case GOSSIP_ACTION_INFO_DEF + 3000:
        {
            switch (player->getClass())
            {
            case CLASS_ROGUE:
                enlistmentOfficerAI->GivePlayerItems(player, Lvl70BiS_Rogue);
                player->CLOSE_GOSSIP_MENU();
                break;
            case CLASS_MAGE:
                enlistmentOfficerAI->GivePlayerItems(player, Lvl70BiS_Mage);
                player->CLOSE_GOSSIP_MENU();
                break;
            case CLASS_HUNTER:
                enlistmentOfficerAI->GivePlayerItems(player, Lvl70BiS_Hunter);
                player->CLOSE_GOSSIP_MENU();
                break;
            case CLASS_WARLOCK:
                enlistmentOfficerAI->GivePlayerItems(player, Lvl70BiS_Warlock);
                player->CLOSE_GOSSIP_MENU();
                break;
            case CLASS_SHAMAN:
                player->PrepareGossipMenu(creature, GOSSIP_TEXT_PICK_SPEC);
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Restoration", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 3001);
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Enhancement", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 3002);
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Elemental", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 3003);
                player->SendPreparedGossip(creature);
                break;
            case CLASS_PRIEST:
                player->PrepareGossipMenu(creature, GOSSIP_TEXT_PICK_SPEC);
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Shadow", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 3004);
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Discipline/Holy", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 3005);
                player->SendPreparedGossip(creature);
                break;
            case CLASS_PALADIN:
                player->PrepareGossipMenu(creature, GOSSIP_TEXT_PICK_SPEC);
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Holy", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 3006);
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Retribution", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 3007);
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Protection", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 3008);
                player->SendPreparedGossip(creature);
                break;
            case CLASS_WARRIOR:
                player->PrepareGossipMenu(creature, GOSSIP_TEXT_PICK_SPEC);
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Fury/Arms", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 3009);
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Protection", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 3010);
                player->SendPreparedGossip(creature);
                break;
            case CLASS_DRUID:
                player->PrepareGossipMenu(creature, GOSSIP_TEXT_PICK_SPEC);
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Feral (Cat/DPS)", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 3011);
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Feral (Bear/Tank)", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 3012);
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Balance", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 3013);
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Restoration", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 3014);
                player->SendPreparedGossip(creature);
                break;
            }
            break;
        }

        // TBC Full Best in Slot (Spec Selected)
        // Shaman - Restoration
        case GOSSIP_ACTION_INFO_DEF + 3001: enlistmentOfficerAI->GivePlayerItems(player, Lvl70BiS_ShamanResto); player->PlayerTalkClass->CloseGossip(); break;
        // Shaman - Enhancement
        case GOSSIP_ACTION_INFO_DEF + 3002: enlistmentOfficerAI->GivePlayerItems(player, Lvl70BiS_ShamanEnhancement); player->PlayerTalkClass->CloseGossip(); break;
        // Shaman - Elemental
        case GOSSIP_ACTION_INFO_DEF + 3003: enlistmentOfficerAI->GivePlayerItems(player, Lvl70BiS_ShamanElemental); player->PlayerTalkClass->CloseGossip(); break;
        // Priest - Shadow
        case GOSSIP_ACTION_INFO_DEF + 3004: enlistmentOfficerAI->GivePlayerItems(player, Lvl70BiS_PriestShadow); player->PlayerTalkClass->CloseGossip(); break;
        // Priest - Discipline/Holy
        case GOSSIP_ACTION_INFO_DEF + 3005: enlistmentOfficerAI->GivePlayerItems(player, Lvl70BiS_PriestDiscHoly); player->PlayerTalkClass->CloseGossip(); break;
        // Paladin - Holy
        case GOSSIP_ACTION_INFO_DEF + 3006: enlistmentOfficerAI->GivePlayerItems(player, Lvl70BiS_PaladinHoly); player->PlayerTalkClass->CloseGossip(); break;
        // Paladin - Retribution
        case GOSSIP_ACTION_INFO_DEF + 3007: enlistmentOfficerAI->GivePlayerItems(player, Lvl70BiS_PaladinRetribution); player->PlayerTalkClass->CloseGossip(); break;
        // Paladin - Protection
        case GOSSIP_ACTION_INFO_DEF + 3008: enlistmentOfficerAI->GivePlayerItems(player, Lvl70BiS_PaladinProtection); player->PlayerTalkClass->CloseGossip(); break;
        // Warrior - Fury/Arms
        case GOSSIP_ACTION_INFO_DEF + 3009: enlistmentOfficerAI->GivePlayerItems(player, Lvl70BiS_WarriorFuryArms); player->PlayerTalkClass->CloseGossip(); break;
        // Warrior - Protection
        case GOSSIP_ACTION_INFO_DEF + 3010: enlistmentOfficerAI->GivePlayerItems(player, Lvl70BiS_WarriorProtection); player->PlayerTalkClass->CloseGossip(); break;
        // Druid - Feral (Cat/DPS)
        case GOSSIP_ACTION_INFO_DEF + 3011: enlistmentOfficerAI->GivePlayerItems(player, Lvl70BiS_DruidFeralCat); player->PlayerTalkClass->CloseGossip(); break;
        // Druid - Feral (Bear/Tank)
        case GOSSIP_ACTION_INFO_DEF + 3012: enlistmentOfficerAI->GivePlayerItems(player, Lvl70BiS_DruidFeralBear); player->PlayerTalkClass->CloseGossip(); break;
        // Druid - Balance
        case GOSSIP_ACTION_INFO_DEF + 3013: enlistmentOfficerAI->GivePlayerItems(player, Lvl70BiS_DruidBalance); player->PlayerTalkClass->CloseGossip(); break;
        // Druid - Restoration
        case GOSSIP_ACTION_INFO_DEF + 3014: enlistmentOfficerAI->GivePlayerItems(player, Lvl70BiS_DruidResto); player->PlayerTalkClass->CloseGossip(); break;
    }
    }

    return true;
}

UnitAI* GetAI_npc_enlistment_officer(Creature* creature)
{
    return new npc_enlistment_officerAI(creature);
}

struct npc_target_dummyAI : public Scripted_NoMovementAI
{
    npc_target_dummyAI(Creature* creature) : Scripted_NoMovementAI(creature) { Reset(); }

    std::map<ObjectGuid, uint64> m_damageMap;

    time_t m_combatStartTime;
    time_t m_combatEndTime;

    uint32 m_uiEvadeTimer;

    void Reset() override
    {
        m_uiEvadeTimer = 0;
        SetCombatMovement(false);
        SetReactState(REACT_PASSIVE);
    }

    void Aggro(Unit* who) override
    {
        m_damageMap.clear();
        m_combatStartTime = time(nullptr);
        m_combatEndTime = 0;
    }

    void EnterEvadeMode() override
    {
        m_combatEndTime = time(nullptr);
        m_uiEvadeTimer = 0;
        ScriptedAI::EnterEvadeMode();
    }

    void SpellHit(Unit* caster, const SpellEntry* spellInfo) override
    {
        if (m_creature->IsInCombat())
        {
            m_creature->GetCombatManager().TriggerCombatTimer(false);
            m_uiEvadeTimer = 5000;
        }
    }

    void DamageTaken(Unit* dealer, uint32& uiDamage, DamageEffectType damagetype, SpellEntry const* spellInfo) override
    {
        m_creature->GetCombatManager().TriggerCombatTimer(false);
        if (dealer)
            m_damageMap[dealer->GetObjectGuid()] += uiDamage;
        m_uiEvadeTimer = 5000;
    }

    void ReceiveEmote(Player* player, uint32 uiEmote) override
    {
        if (uiEmote == TEXTEMOTE_WAVE)
        {
            time_t seconds = (m_combatEndTime ? m_combatEndTime : time(nullptr)) - m_combatStartTime;
            ChatHandler(player).PSendSysMessage("Player DPS during last combat that lasted %ld seconds: %ld Total Damage: %lu", seconds, m_damageMap[player->GetObjectGuid()] / seconds, m_damageMap[player->GetObjectGuid()]);
        }
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (m_uiEvadeTimer)
        {
            if (m_uiEvadeTimer <= uiDiff)
                EnterEvadeMode(); // calls reset
            else
                m_uiEvadeTimer -= uiDiff;
        }
    }
};

UnitAI* GetAI_npc_target_dummy(Creature* creature)
{
    return new npc_target_dummyAI(creature);
}

enum
{
    GOSSIP_TEXT_CRYSTAL     = 50400,
    GOSSIP_TEXT_CRYSTAL_2   = 50401,
};

bool GossipHello_npc_enchantment_crystal(Player* pPlayer, Creature* pCreature)
{
    pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "Head", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
    pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "Shoulder", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
    pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "Cloak", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 3);
    pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "Chest", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 4);
    pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "Wrist", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 5);
    pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "Hands", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 6);
    pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "Legs", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 7);
    pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "Feet", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 8);
    pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "Ring", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 9);
    pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "Ring2", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 10);
    pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "2H Weapon", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 11);
    pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "Mainhand Weapon", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 12);
    pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "Offhand", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 14);
    pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "Ranged", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 16);
    pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "Shield", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 17);

    pPlayer->PlayerTalkClass->SendGossipMenu(GOSSIP_TEXT_CRYSTAL, pCreature->GetObjectGuid());
    return true;
}

bool GossipSelect_npc_enchantment_crystal(Player* pPlayer, Creature* pCreature, uint32 uiSender, uint32 uiAction)
{
    if (uiAction == GOSSIP_ACTION_INFO_DEF + 1)
    {
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "34 attack power 16 hit", EQUIPMENT_SLOT_HEAD, 35452);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "17 strength 16 intellect", EQUIPMENT_SLOT_HEAD, 37891);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "22 spell damage 14 spell hit", EQUIPMENT_SLOT_HEAD, 35447);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "35 healing 12 spell damage 7 mp5", EQUIPMENT_SLOT_HEAD, 35445);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "18 stamina 20 resilience", EQUIPMENT_SLOT_HEAD, 35453);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "16 defense 17 dodge", EQUIPMENT_SLOT_HEAD, 35443);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "20 Fire resistance", EQUIPMENT_SLOT_HEAD, 35456);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "20 Arcane resistance", EQUIPMENT_SLOT_HEAD, 35455);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "20 Shadow resistance", EQUIPMENT_SLOT_HEAD, 35458);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "20 Nature resistance", EQUIPMENT_SLOT_HEAD, 35454);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "20 Frost resistance", EQUIPMENT_SLOT_HEAD, 35457);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "8 all resistance", EQUIPMENT_SLOT_HEAD, 37889);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Main Menu", 0, 0);
        pPlayer->PlayerTalkClass->SendGossipMenu(GOSSIP_TEXT_CRYSTAL_2, pCreature->GetObjectGuid());
    }
    else if (uiAction == GOSSIP_ACTION_INFO_DEF + 2)
    {
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "30 attack power 10 crit", EQUIPMENT_SLOT_SHOULDERS, 35417);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "26 attack power 14 crit", EQUIPMENT_SLOT_SHOULDERS, 29483);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "20 attack power 15 crit", EQUIPMENT_SLOT_SHOULDERS, 35439);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "18 spell damage 10 spell crit", EQUIPMENT_SLOT_SHOULDERS, 35406);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "15 spell damage 14 spell crit", EQUIPMENT_SLOT_SHOULDERS, 29467);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "12 spell damage 15 spell crit", EQUIPMENT_SLOT_SHOULDERS, 35437);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "33 healing 11 spell damage 4 mp5", EQUIPMENT_SLOT_SHOULDERS, 35404);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "31 healing 11 spell damage 5 mp5", EQUIPMENT_SLOT_SHOULDERS, 29475);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "22 healing 6 mp5", EQUIPMENT_SLOT_SHOULDERS, 35435);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "16 stamina 100 armor", EQUIPMENT_SLOT_SHOULDERS, 29480);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "10 dodge 15 defense", EQUIPMENT_SLOT_SHOULDERS, 35433);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "15 dodge 10 defense", EQUIPMENT_SLOT_SHOULDERS, 35402);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Main Menu", 0, 0);
        pPlayer->PlayerTalkClass->SendGossipMenu(GOSSIP_TEXT_CRYSTAL_2, pCreature->GetObjectGuid());
    }
    else if (uiAction == GOSSIP_ACTION_INFO_DEF + 3)
    {
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "12 agility", EQUIPMENT_SLOT_BACK, 34004);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "20 spell penetration", EQUIPMENT_SLOT_BACK, 34003);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "15 Fire resistance", EQUIPMENT_SLOT_BACK, 25081);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "15 Arcane resistance", EQUIPMENT_SLOT_BACK, 34005);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "15 Shadow resistance", EQUIPMENT_SLOT_BACK, 34006);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "15 Nature resistance", EQUIPMENT_SLOT_BACK, 25082);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "7 all resistance", EQUIPMENT_SLOT_BACK, 27962);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "12 dodge", EQUIPMENT_SLOT_BACK, 25086);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "12 defense", EQUIPMENT_SLOT_BACK, 47051);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "120 armor", EQUIPMENT_SLOT_BACK, 27961);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "increase stealth", EQUIPMENT_SLOT_BACK, 25083);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Main Menu", 0, 0);
        pPlayer->PlayerTalkClass->SendGossipMenu(GOSSIP_TEXT_CRYSTAL_2, pCreature->GetObjectGuid());
    }
    else if (uiAction == GOSSIP_ACTION_INFO_DEF + 4)
    {
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "15 resilience", EQUIPMENT_SLOT_CHEST, 33992);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "6 to all stats", EQUIPMENT_SLOT_CHEST, 27960);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "15 spirit", EQUIPMENT_SLOT_CHEST, 33990);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "15 defense", EQUIPMENT_SLOT_CHEST, 46594);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "150 hp", EQUIPMENT_SLOT_CHEST, 27957);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "150 Mana", EQUIPMENT_SLOT_CHEST, 27958);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "Main Menu", 0, 0);
        pPlayer->PlayerTalkClass->SendGossipMenu(GOSSIP_TEXT_CRYSTAL_2, pCreature->GetObjectGuid());
    }
    else if (uiAction == GOSSIP_ACTION_INFO_DEF + 5)
    {
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "24 attack power", EQUIPMENT_SLOT_WRISTS, 34002);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "15 spell damage", EQUIPMENT_SLOT_WRISTS, 27917);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "30 healing 10 spell damage", EQUIPMENT_SLOT_WRISTS, 27911);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "12 stamina", EQUIPMENT_SLOT_WRISTS, 27914);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "12 defense", EQUIPMENT_SLOT_WRISTS, 27906);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "12 strength", EQUIPMENT_SLOT_WRISTS, 27899);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "12 intellect ", EQUIPMENT_SLOT_WRISTS, 34001);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "4 to all stats", EQUIPMENT_SLOT_WRISTS, 27905);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "6 mp5", EQUIPMENT_SLOT_WRISTS, 27913);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "9 spirit", EQUIPMENT_SLOT_WRISTS, 20009);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Main Menu", 0, 0);
        pPlayer->PlayerTalkClass->SendGossipMenu(GOSSIP_TEXT_CRYSTAL_2, pCreature->GetObjectGuid());
    }
    else if (uiAction == GOSSIP_ACTION_INFO_DEF + 6)
    {
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "26 attack power", EQUIPMENT_SLOT_HANDS, 33996);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "15 strength", EQUIPMENT_SLOT_HANDS, 33995);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "15 agility", EQUIPMENT_SLOT_HANDS, 25080);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "20 spell damage", EQUIPMENT_SLOT_HANDS, 33997);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "35 healing 12 spell damage", EQUIPMENT_SLOT_HANDS, 33999);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "15 spell hit", EQUIPMENT_SLOT_HANDS, 33994);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "10 spell crit", EQUIPMENT_SLOT_HANDS, 33993);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Main Menu", 0, 0);
        pPlayer->PlayerTalkClass->SendGossipMenu(GOSSIP_TEXT_CRYSTAL_2, pCreature->GetObjectGuid());
    }
    else if (uiAction == GOSSIP_ACTION_INFO_DEF + 7)
    {
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "50 attack power 12 crit", EQUIPMENT_SLOT_LEGS, 35490);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "40 stamina 12 agility", EQUIPMENT_SLOT_LEGS, 35495);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "35 spell damage 20 stamina", EQUIPMENT_SLOT_LEGS, 31372);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "66 healing 22 spell damage 20 stamina", EQUIPMENT_SLOT_LEGS, 31370);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Main Menu", 0, 0);
        pPlayer->PlayerTalkClass->SendGossipMenu(GOSSIP_TEXT_CRYSTAL_2, pCreature->GetObjectGuid());
    }
    else if (uiAction == GOSSIP_ACTION_INFO_DEF + 8)
    {
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "Movespeed 6 agility", EQUIPMENT_SLOT_FEET, 34007);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "Movespeed 9 stamina", EQUIPMENT_SLOT_FEET, 34008);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "5% snare and root resistance 10 hit", EQUIPMENT_SLOT_FEET, 27954);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "12 agility", EQUIPMENT_SLOT_FEET, 27951);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "12 stamina", EQUIPMENT_SLOT_FEET, 27950);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "4 hp/mp5", EQUIPMENT_SLOT_FEET, 27948);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Main Menu", 0, 0);
        pPlayer->PlayerTalkClass->SendGossipMenu(GOSSIP_TEXT_CRYSTAL_2, pCreature->GetObjectGuid());
    }
    else if (uiAction == GOSSIP_ACTION_INFO_DEF + 9)
    {
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "4 to all stats", EQUIPMENT_SLOT_FINGER1, 27927);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "12 spell damage", EQUIPMENT_SLOT_FINGER1, 27924);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "20 healing 7 spell damage", EQUIPMENT_SLOT_FINGER1, 27926);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "2 damage to physical attacks", EQUIPMENT_SLOT_FINGER1, 27920);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Main Menu", 0, 0);
        pPlayer->PlayerTalkClass->SendGossipMenu(GOSSIP_TEXT_CRYSTAL_2, pCreature->GetObjectGuid());
    }
    else if (uiAction == GOSSIP_ACTION_INFO_DEF + 10)
    {
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "4 to all stats", EQUIPMENT_SLOT_FINGER2, 27927);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "12 spell damage", EQUIPMENT_SLOT_FINGER2, 27924);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "20 healing 7 spell damage", EQUIPMENT_SLOT_FINGER2, 27926);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "2 physical damage", EQUIPMENT_SLOT_FINGER2, 27920);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Main Menu", 0, 0);
        pPlayer->PlayerTalkClass->SendGossipMenu(GOSSIP_TEXT_CRYSTAL_2, pCreature->GetObjectGuid());
    }
    else if (uiAction == GOSSIP_ACTION_INFO_DEF + 11)
    {
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "70 attack power", EQUIPMENT_SLOT_MAINHAND, 27971);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "35 agility", EQUIPMENT_SLOT_MAINHAND, 27977);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "9 physical damage", EQUIPMENT_SLOT_MAINHAND, 20030);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Main Menu", 0, 0);
        pPlayer->PlayerTalkClass->SendGossipMenu(GOSSIP_TEXT_CRYSTAL_2, pCreature->GetObjectGuid());
    }
    else if (uiAction == GOSSIP_ACTION_INFO_DEF + 12)
    {
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "Mongoose", EQUIPMENT_SLOT_MAINHAND, 27984);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "Executioner", EQUIPMENT_SLOT_MAINHAND, 42974);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "20 strength", EQUIPMENT_SLOT_MAINHAND, 27972);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "20 agility", EQUIPMENT_SLOT_MAINHAND, 42620);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "40 spell damage", EQUIPMENT_SLOT_MAINHAND, 27975);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "81 healing 27 spell damage", EQUIPMENT_SLOT_MAINHAND, 34010);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "20 spirit", EQUIPMENT_SLOT_MAINHAND, 23803);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "7 physical damage", EQUIPMENT_SLOT_MAINHAND, 27967);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "50 Arcane/Fire spell damage", EQUIPMENT_SLOT_MAINHAND, 27981);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Next Page ->", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 13);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Main Menu", 0, 0);
        pPlayer->PlayerTalkClass->SendGossipMenu(GOSSIP_TEXT_CRYSTAL_2, pCreature->GetObjectGuid());
    }
    else if (uiAction == GOSSIP_ACTION_INFO_DEF + 13)
    {
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "30 intellect", EQUIPMENT_SLOT_MAINHAND, 27968);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "Battlemaster", EQUIPMENT_SLOT_MAINHAND, 28004);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "Lifestealing", EQUIPMENT_SLOT_MAINHAND, 20032);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "Crusader", EQUIPMENT_SLOT_MAINHAND, 20034);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "Deathfrost", EQUIPMENT_SLOT_MAINHAND, 46578);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "Fiery Weapon", EQUIPMENT_SLOT_MAINHAND, 13898);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "Icy Chill", EQUIPMENT_SLOT_MAINHAND, 20029);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "Spellsurge", EQUIPMENT_SLOT_MAINHAND, 28003);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "Adamantite Weapon Chain", EQUIPMENT_SLOT_MAINHAND, 42687);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "54 Shadow/Frost spell damage", EQUIPMENT_SLOT_MAINHAND, 27982);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "<- Previous Page", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 12);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Main Menu", 0, 0);
        pPlayer->PlayerTalkClass->SendGossipMenu(GOSSIP_TEXT_CRYSTAL_2, pCreature->GetObjectGuid());
    }
    else if (uiAction == GOSSIP_ACTION_INFO_DEF + 14)
    {
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "Mongoose", EQUIPMENT_SLOT_OFFHAND, 27984);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "Executioner", EQUIPMENT_SLOT_OFFHAND, 42974);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "20 strength", EQUIPMENT_SLOT_OFFHAND, 27972);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "20 agility", EQUIPMENT_SLOT_OFFHAND, 42620);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "7 physical damage", EQUIPMENT_SLOT_OFFHAND, 27967);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Next Page ->", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 15);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Main Menu", 0, 0);
        pPlayer->PlayerTalkClass->SendGossipMenu(GOSSIP_TEXT_CRYSTAL_2, pCreature->GetObjectGuid());
    }
    else if (uiAction == GOSSIP_ACTION_INFO_DEF + 15)
    {
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "Battlemaster", EQUIPMENT_SLOT_OFFHAND, 28004);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "Lifestealing", EQUIPMENT_SLOT_OFFHAND, 20032);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "Crusader", EQUIPMENT_SLOT_OFFHAND, 20034);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "Deathfrost", EQUIPMENT_SLOT_OFFHAND, 46578);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "Fiery Weapon", EQUIPMENT_SLOT_OFFHAND, 13898);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "Icy Chill", EQUIPMENT_SLOT_OFFHAND, 20029);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "Adamantite Weapon Chain", EQUIPMENT_SLOT_OFFHAND, 42687);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "<- Previous Page", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 14);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Main Menu ", 0, 0);
        pPlayer->PlayerTalkClass->SendGossipMenu(GOSSIP_TEXT_CRYSTAL_2, pCreature->GetObjectGuid());
    }
    else if (uiAction == GOSSIP_ACTION_INFO_DEF + 16)
    {
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "28 crit", EQUIPMENT_SLOT_RANGED, 30260);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "30 hit", EQUIPMENT_SLOT_RANGED, 22779);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "12 Range damage", EQUIPMENT_SLOT_RANGED, 30252);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Main Menu ", 0, 0);
        pPlayer->PlayerTalkClass->SendGossipMenu(GOSSIP_TEXT_CRYSTAL_2, pCreature->GetObjectGuid());
    }
    else if (uiAction == GOSSIP_ACTION_INFO_DEF + 17)
    {
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "12 resilience", EQUIPMENT_SLOT_OFFHAND, 44383);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "18 stamina", EQUIPMENT_SLOT_OFFHAND, 34009);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "12 intellect", EQUIPMENT_SLOT_OFFHAND, 27945);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "5 to all resistance", EQUIPMENT_SLOT_OFFHAND, 27947);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "15 Shield Block", EQUIPMENT_SLOT_OFFHAND, 27946);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "9 spirit", EQUIPMENT_SLOT_OFFHAND, 20016);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "8 Frost resistance", EQUIPMENT_SLOT_OFFHAND, 11224);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "30 armor", EQUIPMENT_SLOT_OFFHAND, 13464);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "26-38 damage if blocked", EQUIPMENT_SLOT_OFFHAND, 29454);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Main Menu", 0, 0);
        pPlayer->PlayerTalkClass->SendGossipMenu(GOSSIP_TEXT_CRYSTAL_2, pCreature->GetObjectGuid());
    }
    else if (uiAction == GOSSIP_ACTION_INFO_DEF + 0)
        GossipHello_npc_enchantment_crystal(pPlayer, pCreature);
    else
    {
        pPlayer->EnchantItem(uiAction, uiSender);
        GossipHello_npc_enchantment_crystal(pPlayer, pCreature);
    }
    return true;
}

enum
{
    GOSSIP_SENDER_OPTION    = 50,
    GOSSIP_SENDER_SUBOPTION = 51,
};

uint32 hairstyle;
uint32 haircolor;
uint32 facialfeature;
bool   helmetShown;

typedef struct maxStyles_struct {
    uint8 maxMale;
    uint8 maxFemale;
} maxStyles_t;

maxStyles_t maxHairStyles[MAX_RACES] =
{
    {0,0},  //                        0
    {11,18},// RACE_HUMAN           = 1,
    {6,6},  //  RACE_ORC            = 2,
    {10,13},// RACE_DWARF           = 3,
    {6,6},  // RACE_NIGHTELF        = 4,
    {10,9}, // RACE_UNDEAD_PLAYER   = 5,
    {7,6},  //  RACE_TAUREN         = 6,
    {6,6},  // RACE_GNOME           = 7,
    {5,4},  // RACE_TROLL           = 8,
    {0,0},  // RACE_GOBLIN          = 9,
    {9,13}, //  RACE_BLOODELF       = 10,
    {7,10}, //  RACE_DRAENEI        = 11
};

uint8 maxHairColor[MAX_RACES] =
{
    0,  //                        0
    9,  // RACE_HUMAN           = 1,
    7,  //  RACE_ORC            = 2,
    9,  // RACE_DWARF           = 3,
    7,  // RACE_NIGHTELF        = 4,
    9,  // RACE_UNDEAD_PLAYER   = 5,
    2,  //  RACE_TAUREN         = 6,
    8,  // RACE_GNOME           = 7,
    9,  // RACE_TROLL           = 8,
    0,  // RACE_GOBLIN          = 9,
    9,  //  RACE_BLOODELF       = 10,
    6,  //  RACE_DRAENEI        = 11
};

maxStyles_t maxFacialFeatures[MAX_RACES] =
{
    {0,0},  //                        0
    {8,6},  // RACE_HUMAN           = 1,
    {10,6}, // RACE_ORC             = 2,
    {10,5}, // RACE_DWARF           = 3,
    {5,9},  // RACE_NIGHTELF        = 4,
    {16,7}, // RACE_UNDEAD_PLAYER   = 5,
    {6,4},  // RACE_TAUREN          = 6,
    {7,6},  // RACE_GNOME           = 7,
    {10,5}, // RACE_TROLL           = 8,
    {0,0},  // RACE_GOBLIN          = 9,
    {10,9}, // RACE_BLOODELF        = 10,
    {7,6},  // RACE_DRAENEI         = 11
};

void SelectFacialFeature(Player* player, int change);
void SelectHairColor(Player* player, int change);
void SelectHairStyle(Player* player, int change);

bool GossipHello_npc_vengeance_barber(Player* player, Creature* barber)
{
    int32 text = 0;
    uint8 standState = player->getStandState();
    bool sitting = true;

    if (player->IsStandState() || standState == sitting)
        sitting = false;

    // Select a gossip text
    if (!sitting)
    {
        barber->MonsterWhisper("Please sit down!", player);
        return true;
    }
    else switch (player->getRace())
    {
        case RACE_HUMAN:
            text = player->getGender() == GENDER_FEMALE ? 50013 : 50012;    // texts seem to have been lost, even in l4g db release
            break;
        case RACE_ORC:
            text = player->getGender() == GENDER_FEMALE ? 50002 : 50001;
            break;
        case RACE_DWARF:
            text = player->getGender() == GENDER_FEMALE ? 50015 : 50014;
            break;
        case RACE_NIGHTELF:
            text = player->getGender() == GENDER_FEMALE ? 50019 : 50018;
            break;
        case RACE_UNDEAD:
            text = player->getGender() == GENDER_FEMALE ? 50008 : 50007;
            break;
        case RACE_TAUREN:
            text = player->getGender() == GENDER_FEMALE ? 50006 : 50005;
            break;
        case RACE_GNOME:
            text = player->getGender() == GENDER_FEMALE ? 50017 : 50016;
            break;
        case RACE_DRAENEI:
            text = player->getGender() == GENDER_FEMALE ? 50021 : 50020;
            break;
        case RACE_TROLL:
            text = player->getGender() == GENDER_FEMALE ? 50004 : 50003;
            break;
        case RACE_BLOODELF:
            text = player->getGender() == GENDER_FEMALE ? 50010 : 50009;
            break;

        break;
    }

    // store values to restore if player choose to cancel
    hairstyle = player->GetByteValue(PLAYER_BYTES, 2);
    haircolor = player->GetByteValue(PLAYER_BYTES, 3);
    facialfeature = player->GetByteValue(PLAYER_BYTES_2, 0);
    if (sitting)
    {
        if (!player->HasFlag(PLAYER_FLAGS, PLAYER_FLAGS_HIDE_HELM))
            player->ToggleFlag(PLAYER_FLAGS, PLAYER_FLAGS_HIDE_HELM);

        if (player->GetMoney() >= 0)
            player->ADD_GOSSIP_ITEM(0, "Cut my hair, barber!", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
        else
            player->ADD_GOSSIP_ITEM(0, "You need 0 copper to pay me.", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 8);
    }
    player->SEND_GOSSIP_MENU(text, barber->GetObjectGuid());
    return true;
}

void ChangeEffect(Player* player)
{
    // First Send update to player, so most recent datas are up
    player->SendCreateUpdateToPlayer(player);

    // Force client to reload this player, so changes are visible
    WorldPacket data(SMSG_FORCE_DISPLAY_UPDATE, 8);
    data << player->GetGuidStr().c_str();
    // player->BroadcastPacket(data, true);    // Add Send Data to client so relog is not needed and changes can be specateted instantly

    // Do some visual effect ( Vanish visual spell )
    player->CastSpell(player, 24222, TRIGGERED_OLD_TRIGGERED);
}

void SelectHairStyle(Player* player, int change)
{
    uint8 max = maxHairStyles[player->getRace()].maxMale;
    if (player->getGender() == GENDER_FEMALE)
        max = maxHairStyles[player->getRace()].maxFemale;

    int current = player->GetByteValue(PLAYER_BYTES, 2);

    current += change;

    if (current > max)
        current = 0;
    else if (current < 0)
        current = max;

    player->SetByteValue(PLAYER_BYTES, 2, current);
    ChangeEffect(player);
}

void SelectHairColor(Player* player, int change)
{
    uint8 max = maxHairColor[player->getRace()];
    int current = player->GetByteValue(PLAYER_BYTES, 3);

    current += change;

    if (current > max)
        current = 0;
    else if (current < 0)
        current = max;

    player->SetByteValue(PLAYER_BYTES, 3, current);
    ChangeEffect(player);
}

void SelectFacialFeature(Player* player, int change)
{
    uint8 max = maxFacialFeatures[player->getRace()].maxMale;
    if (player->getGender() == GENDER_FEMALE)
        max = maxFacialFeatures[player->getRace()].maxFemale;

    int current = player->GetByteValue(PLAYER_BYTES_2, 0);

    current += change;

    if (current > max)
        current = 0;
    else if (current < 0)
        current = max;

    player->SetByteValue(PLAYER_BYTES_2, 0, current);
    ChangeEffect(player);
}

bool GossipSelect_npc_vengeance_barber(Player* player, Creature* barber, uint32 sender, uint32 action)
{
    char const* FeatureGossipMenu1 = "I want to change my hair style.";
    if (player->getRace() == RACE_TAUREN)
        FeatureGossipMenu1 = "I want to change my horns.";

    char const* FeatureGossipMenu2 = "I want to change my hair color.";
    if (player->getRace() == RACE_TAUREN)
        FeatureGossipMenu1 = "I want to change my horn color.";

    char const* FeatureGossipMenu = "I want to change my facial hair style.";
    switch (player->getRace())
    {
    case RACE_HUMAN:
        if (player->getGender() == GENDER_FEMALE)
            FeatureGossipMenu = "I want to change my piercings.";
        break;
    case RACE_ORC:
        if (player->getGender() == GENDER_FEMALE)
            FeatureGossipMenu = "I want to change my piercings.";
        break;
    case RACE_DWARF:
        if (player->getGender() == GENDER_FEMALE)
            FeatureGossipMenu = "I want to change my earrings.";
        break;
    case RACE_NIGHTELF:
        if (player->getGender() == GENDER_FEMALE)
            FeatureGossipMenu = "I want to change my markings.";
        break;
    case RACE_UNDEAD:
        FeatureGossipMenu = "I want to change my face.";
        break;
    case RACE_TAUREN:
        if (player->getGender() == GENDER_FEMALE)
            FeatureGossipMenu = "I want to change my hair.";
        break;
    case RACE_GNOME:
        if (player->getGender() == GENDER_FEMALE)
            FeatureGossipMenu = "I want to change my earrings.";
        break;
    case RACE_TROLL:
        FeatureGossipMenu = "I want to change my tusks.";
        break;
    case RACE_BLOODELF:
        if (player->getGender() == GENDER_FEMALE)
            FeatureGossipMenu = "I want to change my earrings.";
        break;
    case RACE_DRAENEI:
        player->getGender() == GENDER_FEMALE ? FeatureGossipMenu = "I want to change my horns." : FeatureGossipMenu = "I want to change my tentacles.";
        break;
    }
    // MAP
    // 1 - main menu
    // 2 - next hair style
    // 3 - prev hair style
    // 4 - next hair color
    // 5 - prev hair color
    // 6 - next facial feature
    // 7 - prev facial feature
    // 8 - not enought gold, or other close

    switch (action)
    {
        // After player requested the action
    case GOSSIP_ACTION_INFO_DEF + 1:
        /*if ( sender == GOSSIP_SENDER_MAIN )
        {
            if(player->GetMoney() >= 500000)
                player->ModifyMoney(-500000);
            else
                player->SendBuyError( BUY_ERR_NOT_ENOUGHT_MONEY, barber, 0, 0);
        }*/
        player->ADD_GOSSIP_ITEM(0, FeatureGossipMenu1, GOSSIP_SENDER_OPTION, GOSSIP_ACTION_INFO_DEF + 2);
        player->ADD_GOSSIP_ITEM(0, FeatureGossipMenu2, GOSSIP_SENDER_OPTION, GOSSIP_ACTION_INFO_DEF + 4);
        player->ADD_GOSSIP_ITEM(0, FeatureGossipMenu, GOSSIP_SENDER_OPTION, GOSSIP_ACTION_INFO_DEF + 6);
        player->SEND_GOSSIP_MENU(50023, barber->GetObjectGuid());
        break;
        // hair style
        // next - increase hair style
    case GOSSIP_ACTION_INFO_DEF + 2:
        if (sender == GOSSIP_SENDER_SUBOPTION)
            SelectHairStyle(player, 1);
        // previous - decrease it
    case GOSSIP_ACTION_INFO_DEF + 3:
        if (action == GOSSIP_ACTION_INFO_DEF + 3 && sender == GOSSIP_SENDER_SUBOPTION)
            SelectHairStyle(player, -1);
        // choose options again
        player->ADD_GOSSIP_ITEM(0, "Next one!", GOSSIP_SENDER_SUBOPTION, GOSSIP_ACTION_INFO_DEF + 2);
        player->ADD_GOSSIP_ITEM(0, "Previous one!", GOSSIP_SENDER_SUBOPTION, GOSSIP_ACTION_INFO_DEF + 3);
        player->ADD_GOSSIP_ITEM(0, "I'll have this one.", GOSSIP_SENDER_SUBOPTION, GOSSIP_ACTION_INFO_DEF + 1);
        player->SEND_GOSSIP_MENU(50024, barber->GetObjectGuid());
        break;

        // hair color
        // next - increase hair color
    case GOSSIP_ACTION_INFO_DEF + 4:
        if (sender == GOSSIP_SENDER_SUBOPTION)
            SelectHairColor(player, 1);
        // previous - decrease hair color
    case GOSSIP_ACTION_INFO_DEF + 5:
        if (action == GOSSIP_ACTION_INFO_DEF + 5 && sender == GOSSIP_SENDER_SUBOPTION)
            SelectHairColor(player, -1);
        // choose options again
        player->ADD_GOSSIP_ITEM(0, "Next one!", GOSSIP_SENDER_SUBOPTION, GOSSIP_ACTION_INFO_DEF + 4);
        player->ADD_GOSSIP_ITEM(0, "Previous one!", GOSSIP_SENDER_SUBOPTION, GOSSIP_ACTION_INFO_DEF + 5);
        player->ADD_GOSSIP_ITEM(0, "I'll have this one.", GOSSIP_SENDER_SUBOPTION, GOSSIP_ACTION_INFO_DEF + 1);
        player->SEND_GOSSIP_MENU(50024, barber->GetObjectGuid());
        break;

        // facial feature
        // next - increase hair style
    case GOSSIP_ACTION_INFO_DEF + 6:
        if (sender == GOSSIP_SENDER_SUBOPTION)
            SelectFacialFeature(player, 1);
        // previous - decrease it
    case GOSSIP_ACTION_INFO_DEF + 7:
        if (action == GOSSIP_ACTION_INFO_DEF + 7 && sender == GOSSIP_SENDER_SUBOPTION)
            SelectFacialFeature(player, -1);
        // choose options again
        player->ADD_GOSSIP_ITEM(0, "Next one!", GOSSIP_SENDER_SUBOPTION, GOSSIP_ACTION_INFO_DEF + 6);
        player->ADD_GOSSIP_ITEM(0, "Previous one!", GOSSIP_SENDER_SUBOPTION, GOSSIP_ACTION_INFO_DEF + 7);
        player->ADD_GOSSIP_ITEM(0, "I'll have this one.", GOSSIP_SENDER_SUBOPTION, GOSSIP_ACTION_INFO_DEF + 1);
        player->SEND_GOSSIP_MENU(50024, barber->GetObjectGuid());
        break;

        // cannot affort
    case GOSSIP_ACTION_INFO_DEF + 8:
        player->PlayerTalkClass->CloseGossip();
        break;

    }
    return true;
}

void AddSC_custom_vengeance_helpers()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "npc_enlistment_officer";
    pNewScript->GetAI = &GetAI_npc_enlistment_officer;
    pNewScript->pGossipHello = &GossipHello_npc_enlistment_officer;
    pNewScript->pGossipSelect = &GossipSelect_npc_enlistment_officer;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_target_dummy";
    pNewScript->GetAI = &GetAI_npc_target_dummy;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_enchantment_crystal";
    pNewScript->pGossipHello = &GossipHello_npc_enchantment_crystal;
    pNewScript->pGossipSelect = &GossipSelect_npc_enchantment_crystal;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_vengeance_greeter";
    pNewScript->GetAI = &GetNewAIInstance<npc_vengeance_greeterAI>;
    pNewScript->pGossipHello = &GossipHello_npc_vengeance_greeter;
    pNewScript->pGossipSelect = &GossipSelect_npc_vengeance_greeter;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_vengeance_barber";
    pNewScript->pGossipHello = &GossipHello_npc_vengeance_barber;
    pNewScript->pGossipSelect = &GossipSelect_npc_vengeance_barber;
    pNewScript->RegisterSelf();
}
