#include "ActionButtonPacketHandler.h"
#include "../../../Player.h"
#include "../../outgoing/impl/InterfacePackets.h"
#include "../../outgoing/impl/SystemPackets.h"
#include <iostream>

namespace Skeleton {

void ActionButtonPacketHandler::handle(std::shared_ptr<Player> player, StreamBuffer& inStream, int32_t opcode, int32_t length) {
    int32_t interfaceId = inStream.ReadShort() & 0xFFFF;
    int32_t buttonId = inStream.ReadShort() & 0xFFFF;
    int32_t buttonId2 = 0;
    int32_t teleX;
    int32_t teleY;
    int32_t teleZ;
    bool isRunning;

    if (length >= 6) {
        buttonId2 = inStream.ReadShort() & 0xFFFF;
    }
    if (buttonId2 == 65535) {
        buttonId2 = 0;
    }
    LOG_INFO("[ActionButtonPacketHandler::handle()], Opcode = {}, Length = {}, interfaceID = {}, buttonId = {}, buttonId2 = {}", opcode, length, interfaceId, buttonId, buttonId2);

    switch (interfaceId) {
        case 92: // combat options
            switch (buttonId) {
                case 2: // punch
                case 3: // kick
                case 4: // block
                break;
            }
            break;
        case 271: // prayer tab, handle buttons
            switch (buttonId) {
                case 5: // thick skin
                case 6: // burst of strength
                case 7: // charity of thought
                case 8: // rock skin                                        
                case 9: // superhuman strength
                case 10: // improved reflexes               
                case 11: // rapid restore
                case 12: // rapid heal
                case 25: // protect item
                break;
            }                
            break;
        case 12: // bank
            switch (buttonId) {
                case 10: // noted items
                    break;
                case 11: // regular items
                    break;
            }
            break;
        case 182: // logout tab
            switch (buttonId) {
                case 6:
                    if (auto client = player->GetClient()) {
                        StreamBuffer outStream(8);
                        SystemPackets::SendLogout(outStream, *client->GetEncryptor());
                        client->Send(outStream);
                        client->RequestDisconnect();
                    }
                    break;
            }
            break;
        case 261: // settings tab 
            switch (buttonId) {
                case 0: // run toggle
                    isRunning = player->GetMovementHandler().IsRunningToggled();
                    player->GetMovementHandler().SetRunningToggled(!isRunning);
                    LOG_INFO("[BUTTON] Run toggle: {}", !isRunning);    
                    break;
                case 5: // settings
                    break;
                case 1: // chat effects
                    break;
                case 3: // mouse buttons
                    break;
                case 4: // accept aid
                    break;
                case 2: // split private chat
                    break;
                case 8: // sound on
                    break;
            }
            break;
        case 192: // Magic tab (from ardova)
            switch (buttonId) {
                case 0: // home
                    teleX = 3435;
                    teleY = 2901;
                    teleZ = 0;
                    player->GetMovementHandler().SetTeleportTarget(Position(teleX, teleY, teleZ));
                    break;
                case 15: // varrock
                    teleX = 3251;
                    teleY = 3397;
                    teleZ = 0;
                    player->GetMovementHandler().SetTeleportTarget(Position(teleX, teleY, teleZ));
                    break;
                case 18: // lumbridge
                    teleX = 3232;
                    teleY = 3232;
                    teleZ = 0;
                    player->GetMovementHandler().SetTeleportTarget(Position(teleX, teleY, teleZ));
                    break;
                case 21: // falador
                    teleX = 2964;
                    teleY = 3378;
                    teleZ = 0;
                    player->GetMovementHandler().SetTeleportTarget(Position(teleX, teleY, teleZ));
                    break;
                case 23: // house
                    teleX = 2341;
                    teleY = 3171;
                    teleZ = 0;
                    player->GetMovementHandler().SetTeleportTarget(Position(teleX, teleY, teleZ));
                    break;
                case 26: // camelot
                    teleX = 2758;
                    teleY = 3478;
                    teleZ = 0;
                    player->GetMovementHandler().SetTeleportTarget(Position(teleX, teleY, teleZ));
                    break;
                case 32: // ardougne
                    teleX = 2612;
                    teleY = 3283;
                    teleZ = 0;
                    player->GetMovementHandler().SetTeleportTarget(Position(teleX, teleY, teleZ));
                    break;
                case 37: // watchtower
                    teleX = 2552;
                    teleY = 3114;
                    teleZ = 0;
                    player->GetMovementHandler().SetTeleportTarget(Position(teleX, teleY, teleZ));
                    break;
                case 44: // trollheim
                    teleX = 2816;
                    teleY = 3671;
                    teleZ = 0;
                    player->GetMovementHandler().SetTeleportTarget(Position(teleX, teleY, teleZ));
                    break;
                case 47: // ape atoll
                    teleX = 2776;
                    teleY = 2787;
                    teleZ = 0;
                    player->GetMovementHandler().SetTeleportTarget(Position(teleX, teleY, teleZ));
                    break;
            }
            break;
        case 464: // emotes tab
            switch (buttonId) { 
                case 1: // yes
                case 2: // no
                case 3: // bow
                case 4: // angry
                case 5: // think                
                case 37: // skillcape emote
                break; 
            }
            break;
        case 387: // equipment tab
            switch (buttonId) {
                case 50: 
                    break; // items kept on death
                case 51:
                    {
                        StreamBuffer outStream(2048);
                        InterfacePackets::SendInterface(outStream, *player->GetClient()->GetEncryptor(), 465);
                        player->GetClient()->Send(outStream);
                    }
                    break; // show equipment screen
            }
            break;
        case 320: // skills
            switch (buttonId) {
                case 123: // attack
                    break;
                case 126: // strength
                    break;
                case 129: // defense
                    break;
            }
            break;
        default:
            break;
    }
}

}
