/*
    This is a SampVoice project file
    Author: CyberMor <cyber.mor.2020@gmail.ru>
    open.mp version author: AmyrAhmady (iAmir) <hhm6@yahoo.com>

    See more here https://github.com/AmyrAhmady/sampvoice
    Original repository: https://github.com/CyberMor/sampvoice

    Copyright (c) Daniel (CyberMor) 2020 All rights reserved
*/

#include "BlackList.h"
#include "net/netgame.h"
#include "chatwindow.h"

#include <fstream>


BlackList::LockedPlayer::LockedPlayer(std::string playerName, const PLAYERID playerId) noexcept
    : playerName(std::move(playerName)), playerId(playerId) {}

bool BlackList::Load(const std::string& filePath)
{
    constexpr auto kFileMode = std::ios::in;

    std::ifstream blackListFile { filePath, kFileMode };

    if (!blackListFile || !blackListFile.is_open())
        return false;

    BlackList::blackList.clear();

    {
        std::string nickName;

        while (std::getline(blackListFile, nickName) && !nickName.empty())
        {
            PLAYERID playerId { SV::kNonePlayer };

            if (pNetGame != nullptr)
            {
                for (int iPlayerId { 0 }; iPlayerId < MAX_PLAYERS; ++iPlayerId)
                    {
                        if (const auto playerName = CPlayerPool::GetPlayerName(iPlayerId); playerName != nullptr)
                        {
                            if (nickName == playerName)
                            {
                                playerId = iPlayerId;
                                break;
                            }
                        }
                    }
            }

            BlackList::blackList.emplace_back(std::move(nickName), playerId);
        }
    }

    return true;
}

void BlackList::Free() noexcept
{
    if (!BlackList::initStatus)
        return;

    BlackList::blackList.clear();

    BlackList::initStatus = false;
}

// TODO: imlplement
void BlackList::LockPlayer(const WORD playerId)
{
    if (playerId == SV::kNonePlayer || playerId >= MAX_PLAYERS)
        return;

    if (pNetGame == nullptr)
        return;

    const char* rawName = CPlayerPool::GetPlayerName(playerId);
    if (rawName == nullptr)
        return;

    std::string nameCopy = rawName;

    if (auto pPlayer = CPlayerPool::GetSpawnedPlayer(playerId))
        pPlayer->Remove();

    for (const auto& playerInfo : BlackList::blackList)
    {
        if (playerInfo.playerName == nameCopy)
            return;
    }

    // CChatWindow::AddMessage("Игрок %s(%d) добавлен в чёрный список", nameCopy.c_str(), playerId);

    // BlackList::blackList.emplace_front(std::move(nameCopy), playerId);
}

void BlackList::UnlockPlayer(const WORD playerId)
{
    Log("[sv:dbg:blacklist:unlockplayer] : removing "
        "player (%hu) from blacklist...", playerId);

    BlackList::blackList.remove_if([&playerId](const auto& object)
    {
        return playerId == object.playerId;
    });
}

void BlackList::UnlockPlayer(const std::string& playerName)
{
    Log("[sv:dbg:blacklist:unlockplayer] : removing "
        "player (%s) from blacklist...", playerName.c_str());

    BlackList::blackList.remove_if([&playerName](const auto& object)
    {
        return playerName == object.playerName;
    });
}

const std::list<BlackList::LockedPlayer>& BlackList::RequestBlackList() noexcept
{
    return BlackList::blackList;
}

bool BlackList::IsPlayerBlocked(const WORD playerId) noexcept
{
    if (playerId != SV::kNonePlayer)
    {
        for (const auto& playerInfo : BlackList::blackList)
        {
            if (playerInfo.playerId == playerId)
                return true;
        }
    }

    return false;
}

bool BlackList::IsPlayerBlocked(const std::string& playerName) noexcept
{
    for (const auto& playerInfo : BlackList::blackList)
    {
        if (playerInfo.playerName == playerName)
            return true;
    }

    return false;
}

