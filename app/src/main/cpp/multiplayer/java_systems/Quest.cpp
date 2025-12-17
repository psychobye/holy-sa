#include "Quest.h"
#include "main.h"
#include "net/netgame.h"
#include "util/CJavaWrapper.h"

void CQuest::Show(uint8_t questid, const std::string &name, const std::string &description, uint32_t reward) {
    auto env = CJavaWrapper::GetEnv();

    if (!CQuest::thiz)
        Constructor();

    jstring jName = env->NewStringUTF(name.c_str());
    jstring jDesc = env->NewStringUTF(description.c_str());

    jmethodID method = env->GetMethodID(
            CQuest::clazz,
            "show",
            "(ILjava/lang/String;Ljava/lang/String;I)V"
    );

    env->CallVoidMethod(
            CQuest::thiz,
            method,
            static_cast<jint>(questid),
            jName,
            jDesc,
            static_cast<jint>(reward)
    );

    env->DeleteLocalRef(jName);
    env->DeleteLocalRef(jDesc);

    bIsShow = true;
}

void CNetGame::packetQuest(Packet* p)
{
    RakNet::BitStream bs((unsigned char*)p->data, p->length, false);
    bs.IgnoreBits(40);

    uint8_t questid;
    bs.Read(questid);

    std::string name;
    bs.ReadStr8(name);

    std::string desc;
    bs.ReadStr8(desc);

    uint32_t reward;
    bs.Read(reward);

    CQuest::Show(
            questid,
            cp1251_to_utf8(name),
            cp1251_to_utf8(desc),
            reward
    );
}