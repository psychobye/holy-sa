#include "Quest.h"
#include "main.h"
#include "net/netgame.h"
#include "util/CJavaWrapper.h"

std::unordered_map<uint8_t, CQuest::QuestData> CQuest::g_quests;

void CQuest::QuestAdd(uint8_t questid, const std::string &name, const std::string &description,
                  uint32_t reward, uint8_t status, uint32_t progress, uint32_t reset_at) {
    auto env = CJavaWrapper::GetEnv();

    if (!CQuest::thiz)
        Constructor();

    jstring jName = env->NewStringUTF(name.c_str());
    jstring jDesc = env->NewStringUTF(description.c_str());

    jmethodID method = env->GetMethodID(
            CQuest::clazz,
            "add",
            "(ILjava/lang/String;Ljava/lang/String;IIII)V"
    );

    env->CallVoidMethod(
            CQuest::thiz,
            method,
            static_cast<jint>(questid),
            jName,
            jDesc,
            static_cast<jint>(reward),
            static_cast<jint>(status),
            static_cast<jint>(progress),
            static_cast<jint>(reset_at)
    );

    env->DeleteLocalRef(jName);
    env->DeleteLocalRef(jDesc);

    bIsShow = true;
}

void CNetGame::packetQuest(Packet* p)
{
    RakNet::BitStream bs((unsigned char*)p->data, p->length, false);
    bs.IgnoreBits(40);

    uint8_t type;
    bs.Read(type);

    uint8_t questid;
    bs.Read(questid);

    std::string name;
    bs.ReadStr8(name);

    std::string desc;
    bs.ReadStr8(desc);

    uint32_t reward;
    bs.Read(reward);

    uint8_t status;
    bs.Read(status);

    uint32_t progress;
    bs.Read(progress);

    uint32_t reset_at;
    bs.Read(reset_at);

    if(static_cast<CQuest::QuestAction>(type) == CQuest::QuestAction::ADD_QUEST) {
        CQuest::g_quests[questid] = { questid, status };
        CQuest::QuestAdd(
                questid,
                cp1251_to_utf8(name),
                cp1251_to_utf8(desc),
                reward,
                status,
                progress,
                reset_at
        );
    } else if (static_cast<CQuest::QuestAction>(type) == CQuest::QuestAction::REMOVE_QUEST){
        CQuest::g_quests.erase(questid);
    } else if (static_cast<CQuest::QuestAction>(type) == CQuest::QuestAction::CLEAR_QUEST){
        CQuest::g_quests.clear();
    }

    CQuest::NotifyActiveCountToJava();
}

int CQuest::GetActiveQuestCount() {
    int count = 0;
    for (const auto& kv : CQuest::g_quests) {
        const QuestData &qd = kv.second;
        if (static_cast<CQuest::QuestStatus>(qd.status) != CQuest::QuestStatus::COMPLETED)
            count++;
    }
    return count;
}

void CQuest::NotifyActiveCountToJava() {
    auto env = CJavaWrapper::GetEnv();
    if (!CQuest::thiz) Constructor();

    static jmethodID midUpdate = env->GetMethodID(CQuest::clazz, "updateActiveCount", "(I)V");
    jint cnt = static_cast<jint>(GetActiveQuestCount());
    env->CallVoidMethod(CQuest::thiz, midUpdate, cnt);
}