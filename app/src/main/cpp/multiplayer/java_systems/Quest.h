#pragma once

#include <jni.h>
#include "GuiWrapper.h"
#include <unordered_map>

class CQuest : public CGuiWrapper<CQuest>{
public:
    enum class QuestAction : uint8_t {
        ADD_QUEST,
        REMOVE_QUEST,
        CLEAR_QUEST
    };

    enum class QuestStatus : uint8_t {
        NOT_TAKEN,
        IN_PROGRESS,
        DONE,
        COMPLETED
    };

    struct QuestData {
        uint8_t id;
        uint8_t status;
    };

    static std::unordered_map<uint8_t, QuestData> g_quests;

    static void QuestAdd
    (uint8_t questid, const std::string& name, const std::string& description,
     uint32_t reward, uint8_t status, uint32_t progress, uint32_t reset_at);

    static int GetActiveQuestCount();
    static void NotifyActiveCountToJava();
};
