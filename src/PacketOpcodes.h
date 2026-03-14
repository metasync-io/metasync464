#pragma once
#include <string>
#include <unordered_map>

namespace Skeleton {

inline const std::unordered_map<uint8_t, std::string> OutgoingPacketNames = {
    {8, "InterfaceMedia"},
    {16, "RemoveObject"},
    {17, "SpawnObject"},
    {23, "PrivateMessage"},
    {30, "SystemUpdate"},
    {37, "ConfigLarge"},
    {39, "RemoveGroundItem"},
    {47, "InterfaceString"},
    {63, "AnimateInterface"},
    {69, "NPCUpdate"},
    {72, "InteractionOption"},
    {75, "IgnoreList"},
    {77, "WindowPane"},
    {90, "PlayerUpdate"},
    {92, "UpdateItems"},
    {100, "AddFriend"},
    {108, "ChatMessage"},
    {112, "SpawnGroundItem"},
    {114, "InterfaceText"},
    {132, "SetArea"},
    {142, "InterfaceConfig"},
    {152, "FriendListStatus"},
    {167, "Logout"},
    {190, "UpdateSkill"},
    {201, "ComponentPosition"},
    {221, "MapRegion"},
    {238, "Interface"},
    {245, "Config"}
};

inline const std::unordered_map<uint8_t, std::string> IncomingPacketNames = {
    // ========== IMPLEMENTED ==========
    // Walking packets
    {36, "Walk2"},
    {50, "Walk1"},
    {98, "Walk6"},
    {143, "Walk3"},
    {164, "Walk5"},
    {248, "Walk4"},

    // Command
    {165, "Command"},

    // Action buttons
    {113, "ClickButton1"},
    {153, "ClickButton2"},
    {240, "ClickButton3"},

    // Chat
    {115, "PlayerChat"},

    // Interface
    {177, "InterfaceOption1"},
    {88, "InterfaceOption2"},
    {159, "InterfaceOption3"},
    {86, "InterfaceOption4"},
    {208, "EnterAmount"},
    {255, "CloseInterface"},

    // Item interactions
    {145, "ItemOption1"},
    {117, "ItemOption2"},
    {43, "ItemOption3"},
    {129, "ItemOption4"},
    {135, "ItemOption5"},
    {101, "ItemClick"},
    {214, "SwitchItemSlots"},
    {215, "WieldItem"},
    {216, "PickupItem"},
    {247, "DropItem"},

    // NPC interactions
    {156, "NPCTalkTo"},
    {19, "NPCTrade"},
    {72, "NPCExamine"},

    // Object interactions
    {44, "ObjectClick1"},
    {252, "ObjectClick2"},

    // Player interactions
    {128, "PlayerOption1"},
    {37, "PlayerOption2"},
    {227, "PlayerOption3"},

    // Appearance
    {157, "Appearance"},

    // Custom movement packets (chase camera / cameralock)
    {200, "FaceDirection"},
    {201, "MoveForward"},
    {202, "BrakeMovement"},
    {203, "Turn180"},
    {206, "FinePosition"},

    // ========== NOT IMPLEMENTED ==========
    // Communication / Friends & Ignores
    {7, "QuietPacket1"},           // Silent packet (ignored)
    {11, "PrivacySettings"},       // Public/Private/Trade chat settings
    {47, "ReportAbuse"},           // Report player abuse
    {102, "AddIgnore"},            // Add player to ignore list
    {127, "QuietPacket2"},         // Silent packet (ignored)
    {133, "DeleteFriend"},         // Remove friend from list
    {162, "KickClanMember"},       // Kick player from clan chat
    {188, "ChangeClanRank"},       // Change clan member rank
    {197, "AddFriend"},            // Add player to friends list
    {219, "JoinClanChat"},         // Join a clan chat channel
    {238, "SendPrivateMessage"},   // Send private message to player

    // Item interactions (advanced)
    {73, "MagicOnGroundItem"},     // Cast spell on ground item
    {79, "SwapSlotIF3"},           // Swap item slots (IF3 interface)
    {92, "ExamineItem"},           // Examine item description
    {103, "ItemOnObject"},         // Use item on game object
    {121, "SwapSlot"},             // Swap item slots (legacy)
    {163, "MagicOnItem"},          // Cast spell on inventory item
    {166, "ItemOnItem"},           // Use item on another item
    {212, "OperateItem"},          // Right-click operate/rub item

    // Object interactions (additional)
    {119, "ObjectClick2Alt"},      // Object option 2 (alternate)
    {120, "ObjectClick3"},         // Object option 3
    {176, "ExamineObject"},        // Examine object description

    // Player interactions (combat/social)
    {84, "PlayerAttack"},          // Attack another player
    {123, "MagicOnPlayer"},        // Cast spell on another player
    {180, "PlayerFollow"},         // Follow another player
    {183, "PlayerTrade"},          // Trade request to player

    // NPC interactions (additional)
    {51, "NPCOption3"},            // NPC option 3
    {69, "NPCOption4"},            // NPC option 4 (different from examine)
    {187, "NPCOption5"},           // NPC option 5
    {218, "NPCAttack"},            // Attack NPC

    // Client state
    {99, "ClientAction1"},         // Client action packet 1
    {230, "IdlePacket"},           // Client idle notification
    {243, "ClientAction2"}         // Client action packet 2
};

inline std::string GetOutgoingPacketName(uint8_t opcode) {
    auto it = OutgoingPacketNames.find(opcode);
    if (it != OutgoingPacketNames.end()) {
        return it->second;
    }
    return "UNKNOWN";
}

inline std::string GetIncomingPacketName(uint8_t opcode) {
    auto it = IncomingPacketNames.find(opcode);
    if (it != IncomingPacketNames.end()) {
        return it->second;
    }
    return "UNKNOWN";
}

}
