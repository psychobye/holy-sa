//
// Created on 30.05.2023.
//
#include "../game/common.h"
#include "gui.h"
#include "GiftNotify.h"
#include "main.h"
#include "util/CJavaWrapper.h"
#include "../net/netgame.h"

//void CGiftNotify::Render() {
//    if(!m_bIsShow)
//        return;
//
//    if(!m_pGiftBoxTex) {
//        m_pGiftBoxTex = CUtil::LoadTextureFromDB("gui", "gift_box");
//    }
//    ImVec2 windowSize = ImVec2(950, 400);
//    ImVec2 windowPos = CGUI::GetCenterScreen(windowSize);
//
//    ImGui::SetNextWindowSize(windowSize);
//    ImGui::SetNextWindowPos(windowPos);
//
//    ImGui::Begin("gift_give", &m_bIsShow, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoBackground);
//    {
//        auto pDrawList = ImGui::GetWindowDrawList();
//
//        // background
//        ImColor colorTop(IM_COL32(21, 8, 11, 170));
//        ImColor colorBottom(IM_COL32(4, 120, 126, 170));
//        pDrawList->AddRectFilledMultiColor(windowPos,windowPos + windowSize,
//                                                            colorBottom, colorBottom,
//                                                            colorTop, colorTop,
//                                                            15,
//                                                            ImDrawCornerFlags_All
//        );
//
//        // texture
//        ImVec2 imgSize = ImVec2(210, 210);
//        ImVec2 imgPos = ImVec2( windowPos.x + 20, windowPos.y + 80);
//        pDrawList->AddImage((ImTextureID)m_pGiftBoxTex->raster, imgPos, imgPos + imgSize);
//
//        // button
//        ImVec2 buttSize = ImVec2(200, 80);
//        ImVec2 buttPos = CGUI::GetCenterScreen(buttSize);
//        buttPos.y = windowPos.y + windowSize.y - buttSize.y - 30;
//
//        ImGui::SetCursorPos(buttPos);
//
//        if (ImGui::ButtonLr("Забрать", buttSize)) {
//            m_bIsShow = false;
//
//            RwTextureDestroy(m_pGiftBoxTex);
//            m_pGiftBoxTex = nullptr;
//        }
//
//        ImVec2 textPos = ImVec2(imgPos.x + imgSize.x + 50, imgPos.y + 70);
//
//        ImGui::SetCursorPos(textPos);
//        CGUI::RainbowText(CGiftNotify::m_pGiftName, 35);
//
//        textPos.y += 40;
//        //ImVec2 text2pos = ImVec2(ImGui::GetCursorScreenPos().x, ImGui::GetCursorScreenPos().y + 45);
//        pDrawList->AddText(nullptr, 20, textPos, ImColor(0xc9c7c7ff), "Используйте '/roulette'");
//    }
//    ImGui::End();
//
//}

void CGiftNotify::show(char* prizeName, int rare) {
    auto env = g_pJavaWrapper->GetEnv();

    jstring java_text = env->NewStringUTF(prizeName);

    if(CGiftNotify::thiz == nullptr) {
        jmethodID constructor = env->GetMethodID(CGiftNotify::clazz, "<init>",
                                                 "(Ljava/lang/String;I)V");
        CGiftNotify::thiz = env->NewObject(CGiftNotify::clazz, constructor, java_text, rare);
        CGiftNotify::thiz = env->NewGlobalRef(CGiftNotify::thiz);
    }
    env->DeleteLocalRef(java_text);

    CAudioStreamPool::PlayIndividualStream("http://files.russia.online/sounds/gift_notify.mp3", false);
}


void CNetGame::packetGiftNotify(Packet* p)
{
    RakNet::BitStream bs((unsigned char*)p->data, p->length, false);

    bs.IgnoreBits(40); // skip packet and rpc id

    uint8_t len;
    bs.Read(len);

    char text[len + 1];
    bs.Read(text, len);
    text[len] = '\0';

    char utf_text[len + 1];
    cp1251_to_utf8(utf_text, text, len);

    CGiftNotify::show(utf_text);
  //  memset(CGiftNotify::m_pGiftName, 0, sizeof(CGiftNotify::m_pGiftName));
  //  sprintf(CGiftNotify::m_pGiftName, "Вы получили подарок! ( %s )", utf_text);


  //  CGiftNotify::m_bIsShow = true;
}

extern "C"
JNIEXPORT void JNICALL
Java_com_lit_game_gui_GiftNotify_nativeHide(JNIEnv *env, jobject thiz) {
    env->DeleteGlobalRef(CGiftNotify::thiz);
    CGiftNotify::thiz = nullptr;
}
extern "C"
JNIEXPORT void JNICALL
Java_com_lit_game_gui_treasure_Treasure_nativeShowGiftNotify(JNIEnv *env, jobject thiz,
                                                                  jstring name, jint rare) {
    const char *name_cpp = env->GetStringUTFChars(name, nullptr);

    char name_tmp[255];
    strcpy(name_tmp, name_cpp);
    env->ReleaseStringUTFChars(name, name_cpp);

    CGiftNotify::show(name_tmp, rare);
}