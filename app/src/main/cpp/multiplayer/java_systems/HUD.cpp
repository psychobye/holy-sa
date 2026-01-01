//
// Created on 28.10.2022.
//

#include "HUD.h"
#include <jni.h>

#include "main.h"

#include "../game/game.h"
#include "net/netgame.h"
#include "gui/gui.h"
#include "keyboard.h"
#include "CSettings.h"
#include "chatwindow.h"
#include "Speedometr.h"
#include "util/patch.h"
#include "../game/Entity/Ped/Ped.h"
#include "game/Widgets/TouchInterface.h"

extern CJavaWrapper *g_pJavaWrapper;
extern CGUI* pGUI;

bool        CHUD::bIsOnlyVisualOff = true;
bool        CHUD::bIsShow = false;
bool        CHUD::bIsShowEnterExitButt = false;
bool        CHUD::bIsShowLockButt = false;
bool        CHUD::bIsShowChat = true;
int         CHUD::iLocalMoney = 0;
int         CHUD::iWantedLevel = 0;
bool        CHUD::bIsShowMafiaWar = false;
float       CHUD::fLastGiveDamage = 0.0f;
bool        CHUD::bIsTouchCameraButt = false;
bool        CHUD::bIsCamEditGui = false;
int         CHUD::iSatiety = 0;
PLAYERID    CHUD::lastGiveDamagePlayerId = INVALID_PLAYER_ID;

CVector2D   CHUD::radarBgPos1;
CVector2D   CHUD::radarBgPos2;

jobject CHUD::thiz = nullptr;

jmethodID jUpdateHudInfo;

void CHUD::ChangeChatTextSize(int size) {
    JNIEnv* env = g_pJavaWrapper->GetEnv();
    if(!env)return;

    jclass clazz = env->GetObjectClass(thiz);
    jmethodID method = env->GetMethodID(clazz, "ChangeChatFontSize", "(I)V");

    env->CallVoidMethod(thiz, method, size);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_lit_game_gui_hud_HudManager_HudInit(JNIEnv *env, jobject thiz) {

    CHUD::thiz = env->NewGlobalRef(thiz);
    jUpdateHudInfo = env->GetMethodID(env->GetObjectClass(thiz), "updateAmmo", "(III)V");

    CHUD::ToggleProgressTexts(CSettings::m_Settings.iHPArmourText);
    CHUD::ChangeChatTextSize(CSettings::m_Settings.iChatFontSize);

}

void CHUD::toggleAll(bool toggle, bool chat, bool onlyVisual)
{
    if(toggle == bIsShow)
    {
        return;
    }

    bIsShow = toggle;
    bIsOnlyVisualOff = onlyVisual;
   // CGame::ToggleHUDElement(HUD_ELEMENT_BUTTONS, toggle);

    CLocalPlayer::GetPlayerPed()->TogglePlayerControllable(toggle, true);
    CGame::ToggleHUDElement(HUD_ELEMENT_FPS, toggle);

    JNIEnv *env = g_pJavaWrapper->GetEnv();

    if (!env) {
        Log("No env");
        return;
    }
    jclass clazz = env->GetObjectClass(thiz);

    jmethodID method = env->GetMethodID(clazz, "toggleAll", "(ZZ)V");
    env->CallVoidMethod(thiz, method, toggle, chat);

    *(uint8_t*)(g_libGTASA + (VER_x32 ? 0x00819D88 : 0x9FF3A8)) = 0;
}

void CHUD::toggleLockButton(bool toggle)
{
    bIsShowLockButt = toggle;

    JNIEnv* env = g_pJavaWrapper->GetEnv();
    jclass clazz = env->GetObjectClass(thiz);

    jmethodID ToggleLockVehicleButton = env->GetMethodID(clazz, "toggleLockButton", "(Z)V");
    env->CallVoidMethod(thiz, ToggleLockVehicleButton, toggle);
}

bool CHUD::NeededRenderPassengerButton() {
    if (!CHUD::bIsShow || !bIsShowEnterExitButt || CKeyBoard::IsOpen())
        return false;

    CPedSamp* pPed = CGame::FindPlayerPed();
    if (!pPed || pPed->m_pPed->IsInVehicle())
        return false;

    VEHICLEID nearestId = CVehiclePool::FindNearestToLocalPlayerPed();
    if (nearestId == INVALID_VEHICLE_ID)
        return false;

    CVehicleSamp* pVeh = CVehiclePool::GetAt(nearestId);
    if (!pVeh || !pVeh->m_pVehicle)
        return false;

    float dist = pVeh->m_pVehicle->GetDistanceFromLocalPlayerPed();
    return (dist < 4.0f);
}

void CHUD::updateAmmo()
{
    CPedSamp* pPed = CGame::FindPlayerPed();

    JNIEnv* env = g_pJavaWrapper->GetEnv();

    env->CallVoidMethod(thiz, jUpdateHudInfo,
                        (int)pPed->m_pPed->m_aWeapons[pPed->m_pPed->m_nActiveWeaponSlot].m_nType,
                        (int)pPed->m_pPed->m_aWeapons[pPed->m_pPed->m_nActiveWeaponSlot].m_nTotalAmmo,
                        (int)pPed->m_pPed->m_aWeapons[pPed->m_pPed->m_nActiveWeaponSlot].dwAmmoInClip
                        );
}

void CHUD::updateBars() {
    auto env = g_pJavaWrapper->GetEnv();
    auto pPed = CGame::FindPlayerPed();

    jclass clazz = env->GetObjectClass(thiz);
    jmethodID method = env->GetMethodID(clazz, "updateBars", "(III)V");

    env->CallVoidMethod(thiz, method,
                        (int)pPed->GetHealth(),
                        (int)pPed->GetArmour(),
                        (int)CHUD::iSatiety
    );
}
void CHUD::UpdateWanted()
{
    JNIEnv* env = g_pJavaWrapper->GetEnv();

    if (!env)
    {
        Log("No env");
        return;
    }
    jclass clazz = env->GetObjectClass(thiz);
    jmethodID method = env->GetMethodID(clazz, "UpdateWanted", "(I)V");

    env->CallVoidMethod(thiz, method, iWantedLevel);
}

void CHUD::updateLevelInfo(int level, int currentexp, int maxexp)
{
    JNIEnv* env = g_pJavaWrapper->GetEnv();

    if (!env)
    {
        Log("No env");
        return;
    }
    jclass clazz = env->GetObjectClass(thiz);
    jmethodID method = env->GetMethodID(clazz, "updateLevelInfo", "(III)V");

    env->CallVoidMethod(thiz, method, level, currentexp, maxexp);
}

void CHUD::showUpdateTargetNotify(int type, char *text)
{
    JNIEnv* env = g_pJavaWrapper->GetEnv();

    if (!env)
    {
        Log("No env");
        return;
    }
    jclass clazz = env->GetObjectClass(thiz);
    jmethodID method = env->GetMethodID(clazz, "showUpdateTargetNotify", "(ILjava/lang/String;)V");

    jclass strClass = env->FindClass("java/lang/String");
    jmethodID ctorID = env->GetMethodID(strClass, "<init>", "([BLjava/lang/String;)V");
    jstring encoding = env->NewStringUTF("UTF-8");

    jbyteArray bytes = env->NewByteArray(strlen(text));
    env->SetByteArrayRegion(bytes, 0, strlen(text), (jbyte*)text);
    jstring jtext = (jstring) env->NewObject(strClass, ctorID, bytes, encoding);
    env->CallVoidMethod(thiz, method, type, jtext);
    env->DeleteLocalRef(encoding);
}

void CHUD::hideTargetNotify()
{
    JNIEnv* env = g_pJavaWrapper->GetEnv();

    if (!env)
    {
        Log("No env");
        return;
    }
    jclass clazz = env->GetObjectClass(thiz);
    jmethodID method = env->GetMethodID(clazz, "hideTargetNotify", "()V");

    env->CallVoidMethod(thiz, method);
}

void CHUD::toggleGps(bool toggle)
{
    JNIEnv* env = g_pJavaWrapper->GetEnv();

    if (!env)
    {
        Log("No env");
        return;
    }
    jclass clazz = env->GetObjectClass(thiz);
    jmethodID method = env->GetMethodID(clazz, "toggleGps", "(Z)V");

    env->CallVoidMethod(thiz, method, toggle);
}

void CHUD::toggleServerLogo(bool toggle)
{
    JNIEnv* env = g_pJavaWrapper->GetEnv();

    if (!env)
    {
        Log("No env");
        return;
    }
    jclass clazz = env->GetObjectClass(thiz);
    jmethodID method = env->GetMethodID(clazz, "toggleServerLogo", "(Z)V");

    env->CallVoidMethod(thiz, method, toggle);
}

void CHUD::toggleGreenZone(bool toggle)
{
    JNIEnv* env = g_pJavaWrapper->GetEnv();

    if (!env)
    {
        Log("No env");
        return;
    }
    jclass clazz = env->GetObjectClass(thiz);
    jmethodID method = env->GetMethodID(clazz, "toggleGreenZone", "(Z)V");

    env->CallVoidMethod(thiz, method, toggle);
}

void CHUD::UpdateMoney()
{
    JNIEnv* env = g_pJavaWrapper->GetEnv();

    if (!env)
    {
        Log("No env");
        return;
    }
    jclass clazz = env->GetObjectClass(thiz);
    jmethodID method = env->GetMethodID(clazz, "updateMoney", "(I)V");

    env->CallVoidMethod(thiz, method, iLocalMoney);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_lit_game_gui_hud_HudManager_ClickLockVehicleButton(JNIEnv *env, jobject thiz) {
    pNetGame->SendChatCommand("/lock");
}

extern "C"
JNIEXPORT void JNICALL
Java_com_lit_game_gui_Speedometer_ClickSpedometr(JNIEnv *env, jobject thiz, jint turn_id,
                                                      jboolean toggle) {
    uint8_t packet = ID_CUSTOM_RPC;
    uint8_t RPC = RPC_TURN_SIGNAL;
    uint8_t button = turn_id;
    uint8_t toggle_ = toggle;


    RakNet::BitStream bsSend;
    bsSend.Write(packet);
    bsSend.Write(RPC);
    bsSend.Write(button);
    bsSend.Write(toggle_);
    pNetGame->GetRakClient()->Send(&bsSend, SYSTEM_PRIORITY, RELIABLE_SEQUENCED, 0);
}

void CNetGame::packetSalary(Packet* p)
{
    RakNet::BitStream bs((unsigned char*)p->data, p->length, false);
    uint8_t packetID;
    uint32_t rpcID;
    uint32_t salary;
    uint32_t lvl;
    float exp;

    bs.Read(packetID);
    bs.Read(rpcID);
    bs.Read(salary);
    bs.Read(lvl);
    bs.Read(exp);


    CHUD::UpdateSalary(salary, lvl, exp);
}

void CHUD::UpdateSalary(int salary, int lvl, float exp)
{
    JNIEnv* env = g_pJavaWrapper->GetEnv();

    jclass clazz = env->GetObjectClass(thiz);
    jmethodID UpdateSalary = env->GetMethodID(clazz, "updateSalary", "(IIF)V");

    env->CallVoidMethod(thiz, UpdateSalary, salary, lvl, exp);
}

void CHUD::SetChatInput(const char ch[])
{
    static char msg_utf[1024];
    cp1251_to_utf8(msg_utf, ch);
    JNIEnv* env = CJavaWrapper::GetEnv();

    jstring jch = env->NewStringUTF(msg_utf);

    jclass clazz = env->GetObjectClass(thiz);
    jmethodID AddToChatInput = env->GetMethodID(clazz, "AddToChatInput", "(Ljava/lang/String;)V");

    env->CallVoidMethod(thiz, AddToChatInput, jch);
    env->DeleteLocalRef(jch);
}

void CHUD::AddChatMessage(const char msg[])
{
    if(!thiz)return;

    static char msg_utf[1024];
    cp1251_to_utf8(msg_utf, msg);

    JNIEnv* env = g_pJavaWrapper->GetEnv();
    //
    jstring jmsg = env->NewStringUTF( msg_utf );

    jclass clazz = env->GetObjectClass(thiz);
    jmethodID AddChatMessage = env->GetMethodID(clazz, "AddChatMessage", "(Ljava/lang/String;)V");

    env->CallVoidMethod(thiz, AddChatMessage, jmsg);
    env->DeleteLocalRef(jmsg);
}

void CHUD::addGiveDamageNotify(PLAYERID id, int weaponId, float damage, ePedPieceTypes bodypart)
{
    if(!CSettings::m_Settings.iIsEnableDamageInformer) return;

    if(damage > 200) damage = 200.0f;

    if(lastGiveDamagePlayerId == id) {
        fLastGiveDamage += damage;
    }
    else {
        lastGiveDamagePlayerId = id;
        fLastGiveDamage = damage;
    }

    JNIEnv* env = CJavaWrapper::GetEnv();

    jstring jnick;

    if(CActorPool::GetAt(id))
        jnick = env->NewStringUTF( CActorPool::GetName(id) );
    else
        jnick = env->NewStringUTF( CPlayerPool::GetPlayerName(id) );

    jstring jweap = env->NewStringUTF( CUtil::GetWeaponName(weaponId) );
    jstring jbodypartname = env->NewStringUTF(pedPieceNames[bodypart].c_str());

    jclass clazz = env->GetObjectClass(thiz);
    jmethodID method = env->GetMethodID(clazz, "addGiveDamageNotify", "(Ljava/lang/String;Ljava/lang/String;FLjava/lang/String;)V");

    env->CallVoidMethod(thiz, method, jnick, jweap, fLastGiveDamage, jbodypartname);
    env->DeleteLocalRef(jnick);
    env->DeleteLocalRef(jweap);
    env->DeleteLocalRef(jbodypartname);
}

void CHUD::addTakeDamageNotify(char nick[], int weaponId, float damage)
{
    if(!CSettings::m_Settings.iIsEnableDamageInformer) return;

    JNIEnv* env = g_pJavaWrapper->GetEnv();
    if(damage > 200) damage = 200.0f;
    jstring jnick = env->NewStringUTF( nick );
    jstring jweap = env->NewStringUTF( CUtil::GetWeaponName(weaponId) );

    jclass clazz = env->GetObjectClass(thiz);
    jmethodID method = env->GetMethodID(clazz, "addTakeDamageNotify", "(Ljava/lang/String;Ljava/lang/String;F)V");

    env->CallVoidMethod(thiz, method, jnick, jweap, damage);
    env->DeleteLocalRef(jnick);
    env->DeleteLocalRef(jweap);
}

void CHUD::ToggleProgressTexts(bool toggle)
{
    JNIEnv* env = g_pJavaWrapper->GetEnv();

    jclass clazz = env->GetObjectClass(thiz);
    jmethodID method = env->GetMethodID(clazz, "toggleProgressTexts", "(Z)V");
    env->CallVoidMethod(thiz, method, toggle);
}

void CHUD::ToggleChat(bool toggle){
    bIsShowChat = toggle;

    JNIEnv* env = g_pJavaWrapper->GetEnv();

    jclass clazz = env->GetObjectClass(thiz);
    jmethodID ToggleChat = env->GetMethodID(clazz, "ToggleChat" , "(Z)V");
    env->CallVoidMethod(thiz, ToggleChat, toggle);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_lit_game_gui_hud_HudManager_SetRadarBgPos(JNIEnv *env, jobject thiz, jfloat x1, jfloat y1,
                                                    jfloat x2, jfloat y2) {
    CHUD::radarBgPos1.x = x1;
    CHUD::radarBgPos1.y = y1;

    CHUD::radarBgPos2.x = x2;
    CHUD::radarBgPos2.y = y2;
}

void CHUD::ToggleChatInput(bool toggle)
{
    JNIEnv* env = g_pJavaWrapper->GetEnv();


    jclass clazz = env->GetObjectClass(thiz);
    jmethodID ToggleChatInput = env->GetMethodID(clazz, "ToggleChatInput", "(Z)V");

    env->CallVoidMethod(thiz, ToggleChatInput, toggle);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_lit_game_gui_hud_Chat_SendChatMessage(JNIEnv *env, jobject thiz, jbyteArray str) {
    jbyte* pMsg = env->GetByteArrayElements(str, nullptr);
    jsize length = env->GetArrayLength(str);

    std::string szStr((char*)pMsg, length);
    //const char *inputText = pEnv->GetStringUTFChars(str, nullptr);

    if(pNetGame) {
        CGame::PostToMainThread([=]{
            CKeyBoard::m_sInput = szStr;
            CKeyBoard::Send();
        });
       // pNetGame->SendChatMessage(const_cast<char *>(szStr.c_str()));

    }

    env->ReleaseByteArrayElements(str, pMsg, JNI_ABORT);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_lit_game_gui_hud_HudManager_clickCameraMode(JNIEnv *env, jobject thiz) {
    CPedSamp *pPed = CLocalPlayer::GetPlayerPed();
    if(!pPed) return;

    if(CLocalPlayer::IsSpectating())
        return;

    if(pPed->m_pPed->IsInVehicle()) {
        CHUD::bIsTouchCameraButt = true;
    }
    else {
        CFirstPersonCamera::Toggle();
    }
}
extern "C"
JNIEXPORT void JNICALL
Java_com_lit_game_gui_hud_HudManager_clickMultText(JNIEnv *env, jobject thiz) {
    pNetGame->SendChatCommand("/action");
}

void CNetGame::packetUpdateSatiety(Packet* p)
{
    RakNet::BitStream bs((unsigned char*)p->data, p->length, false);

    bs.IgnoreBits(40); // skip packet and rpc id

    uint8_t value;

    bs.Read(value);

    CHUD::iSatiety = value;

    CHUD::updateBars();
}

void CNetGame::packetTorpedoButt(Packet* p)
{
    RakNet::BitStream bs((unsigned char*)p->data, p->length, false);

    bs.IgnoreBits(40); // skip packet and rpc id

    uint8_t value;

    bs.Read(value);

    JNIEnv* env = g_pJavaWrapper->GetEnv();


    jclass clazz = env->GetObjectClass(CHUD::thiz);
    jmethodID method = env->GetMethodID(clazz, "toggleTorpedoButt", "(Z)V");

    env->CallVoidMethod(CHUD::thiz, method, value);

}

extern "C"
JNIEXPORT void JNICALL
Java_com_lit_game_gui_hud_HudManager_sendTorpedo(JNIEnv *env, jobject thiz) {
    uint8_t packet = ID_CUSTOM_RPC;
    uint8_t RPC = 80;


    RakNet::BitStream bsSend;
    bsSend.Write(packet);
    bsSend.Write(RPC);
    bsSend.Write(1);
    pNetGame->GetRakClient()->Send(&bsSend, SYSTEM_PRIORITY, RELIABLE_SEQUENCED, 0);
}
extern "C"
JNIEXPORT void JNICALL
Java_com_lit_game_gui_hud_HudManager_nativeSetRadarPos(JNIEnv *env, jobject thiz, jfloat x1,
                                                            jfloat y1, jfloat width,
                                                            jfloat height) {
    CHUD::radarPos.x = x1;
    CHUD::radarPos.y = y1;
    CHUD::radarSize.x = width;
    CHUD::radarSize.y = height;
}