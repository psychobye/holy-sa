//
// Created on 26.11.2022.
//

#include "ObjectEditor.h"
#include <jni.h>
#include "main.h"

#include "../game/game.h"
#include "net/netgame.h"
#include "gui/gui.h"
#include "keyboard.h"
#include "CSettings.h"
#include "chatwindow.h"
#include "util/CJavaWrapper.h"
#include "game/Models/ModelInfo.h"
#include "game/Entity/Ped/Ped.h"


void CObjectEditor::Start(int slot)
{
    if(!CLocalPlayer::GetPlayerPed()->IsValidAttach(slot)){
        CChatWindow::AddMessage("Invalid attach slot %d", slot);
        return;
    }
    CObjectEditor::iEditedId = slot;
    CObjectEditor::editType = TYPE_PLAYER_ATTACH;

    CObjectEditor::showGui();
}


void CObjectEditor::Start(CObjectSamp* object)
{
    if(!object) {
        CChatWindow::AddMessage("Invalid object id");
        return;
    }
    CObjectEditor::pObject = object;

    CObjectEditor::editType = TYPE_OBJECT;

    CObjectEditor::showGui();
}

void CObjectEditor::Start(uint16 objectId, bool isPlayerObject)
{
    auto pObject = CObjectPool::GetAt(objectId);
    if(!pObject) {
        CChatWindow::AddMessage("Invalid object id %d", objectId);
        return;
    }
    CObjectEditor::iEditedId = objectId;
    CObjectEditor::bPlayerObj = isPlayerObject;

    if(pObject->m_bAttachedType == eObjectAttachType::TO_VEHICLE) // attached to veh?
        CObjectEditor::editType = TYPE_VEHICLE_ATTACH;
    else
        CObjectEditor::editType = TYPE_OBJECT;

    CObjectEditor::showGui();
}

void CObjectEditor::showGui() {
    JNIEnv* env = CJavaWrapper::GetEnv();

    if(CObjectEditor::thiz == nullptr)
    {
        jmethodID constructor = env->GetMethodID(CObjectEditor::clazz, "<init>", "()V");
        CObjectEditor::thiz = env->NewObject(CObjectEditor::clazz, constructor);
        CObjectEditor::thiz = env->NewGlobalRef(CObjectEditor::thiz);
    }

    bIsToggle = true;
}


extern "C"
JNIEXPORT void JNICALL
Java_com_lit_game_gui_AttachEdit_Exit(JNIEnv *env, jobject thiz) {
    CGame::PostToMainThread([=] {
        CPedSamp *pPlayer = CLocalPlayer::GetPlayerPed();


        if (CObjectEditor::editType == CObjectEditor::TYPE_PLAYER_ATTACH) {
            int slot = CObjectEditor::iEditedId;
            auto attach = pPlayer->GetAttachedObject(slot);

            if (!attach) {
                CChatWindow::AddMessage("invalid object. type = %d, id = %d ", CObjectEditor::editType, slot);
                return;
            }
            CObjectEditor::SendOnEditAttach(
                    0,
                    slot,
                    attach->dwModelId,
                    attach->dwSampBone,
                    attach->vecOffset,
                    attach->vecRotation,
                    attach->vecScale
            );
        } else if (CObjectEditor::editType == CObjectEditor::TYPE_OBJECT || CObjectEditor::editType == CObjectEditor::TYPE_VEHICLE_ATTACH) {//
            auto pObject = CObjectPool::GetAt(CObjectEditor::iEditedId);
            if (!pObject) {
                CChatWindow::AddMessage("invalid object. type = %d, id = %d ", CObjectEditor::editType, CObjectEditor::iEditedId);
                return;
            }

            auto pos = pObject->m_pEntity->GetPosition();
            CVector rot;
            pObject->m_pEntity->GetOrientationDeg(rot.x, rot.y, rot.z);

            RakNet::BitStream bsSend;

            bsSend.Write((bool) CObjectEditor::bPlayerObj);
            bsSend.Write((uint16_t) CObjectEditor::iEditedId);
            bsSend.Write((uint32_t) CObjectEditor::eResponse::EDIT_RESPONSE_CANCEL);
            bsSend.Write((float) pos.x);
            bsSend.Write((float) pos.y);
            bsSend.Write((float) pos.z);
            bsSend.Write((float) rot.x);
            bsSend.Write((float) rot.y);
            bsSend.Write((float) rot.z);

            pNetGame->GetRakClient()->RPC(&RPC_EditObject, &bsSend, HIGH_PRIORITY, RELIABLE, 0, false, UNASSIGNED_NETWORK_ID, nullptr);
        }

        CObjectEditor::editType = CObjectEditor::TYPE_NONE;
        CObjectEditor::iEditedId = INVALID_EDITED_SLOT;
        CObjectEditor::bIsToggle = false;
        CObjectEditor::pObject = nullptr;
        env->DeleteGlobalRef(CObjectEditor::thiz);
        CObjectEditor::thiz = nullptr;
    });
}

void CObjectEditor::SendOnEditAttach(int response, int index, int modelid, int bone, CVector offset, CVector rot, CVector scale){

    RakNet::BitStream bsSend;

    bsSend.Write((uint32_t)response);
    bsSend.Write((uint32_t)index);
    bsSend.Write((uint32_t)modelid);
    bsSend.Write((uint32_t)bone);
    bsSend.Write(offset);
    bsSend.Write(rot);
    bsSend.Write(scale);
    bsSend.Write((uint32_t)0);
    bsSend.Write((uint32_t)0);

    pNetGame->GetRakClient()->RPC(&RPC_EditAttachedObject, &bsSend, HIGH_PRIORITY, RELIABLE, 0, false, UNASSIGNED_NETWORK_ID, nullptr);
}
extern "C"
JNIEXPORT void JNICALL
Java_com_lit_game_gui_AttachEdit_AttachClick(JNIEnv *env, jobject thiz, jint button_type,
                                                  jboolean button_id) {
    CGame::PostToMainThread([=] {


        auto pPlayer = CLocalPlayer::GetPlayerPed();
        int slot = CObjectEditor::iEditedId;
        auto attach = pPlayer->GetAttachedObject(slot);

        CObjectSamp *pObject = nullptr;
        if (CObjectEditor::editType != CObjectEditor::TYPE_PLAYER_ATTACH) {
            if (CObjectEditor::pObject)
                pObject = CObjectEditor::pObject;
            else {
                pObject = CObjectPool::GetAt(CObjectEditor::iEditedId);
                pObject->bNeedReAttach = true;
            }
        }
        if (button_type == 0) { // �����/����
            float value = (button_id) ? (0.006f) : (-0.006f);

            if (CObjectEditor::editType == CObjectEditor::TYPE_PLAYER_ATTACH) {
                attach->vecOffset.z += value;
            }
            if (CObjectEditor::editType == CObjectEditor::TYPE_VEHICLE_ATTACH) {
                pObject->m_vecAttachedOffset.x += value;
            }
            if (CObjectEditor::editType == CObjectEditor::TYPE_OBJECT) {
                auto pos = pObject->m_pEntity->GetPosition();
                pos.x += value * 5;

                pObject->m_pEntity->Teleport(pos, false);
            }
        }

        if (button_type == 1) { // �����/���
            float value = (button_id) ? (0.006f) : (-0.006f);

            if (CObjectEditor::editType == CObjectEditor::TYPE_PLAYER_ATTACH) {
                attach->vecOffset.x += value;
            }
            if (CObjectEditor::editType == CObjectEditor::TYPE_VEHICLE_ATTACH) {
                pObject->m_vecAttachedOffset.z += value;
            }
            if (CObjectEditor::editType == CObjectEditor::TYPE_OBJECT) {
                auto pos = pObject->m_pEntity->GetPosition();
                pos.z += value * 5;

                pObject->m_pEntity->Teleport(pos, false);
            }
        }
        if (button_type == 2) { // Y
            float value = (button_id) ? (0.006f) : (-0.006f);

            if (CObjectEditor::editType == CObjectEditor::TYPE_PLAYER_ATTACH) {
                attach->vecOffset.y += value;
            }
            if (CObjectEditor::editType == CObjectEditor::TYPE_VEHICLE_ATTACH) {
                pObject->m_vecAttachedOffset.y += value;
            }
            if (CObjectEditor::editType == CObjectEditor::TYPE_OBJECT) {
                auto pos = pObject->m_pEntity->GetPosition();
                pos.y += value * 5;

                pObject->m_pEntity->Teleport(pos, false);
                pObject->m_pEntity->UpdateRW();
                pObject->m_pEntity->UpdateRwFrame();
            }
        }
        if (button_type == 3) { // scale
            float value = (button_id) ? (0.006f) : (-0.006f);

            if (CObjectEditor::editType == CObjectEditor::TYPE_PLAYER_ATTACH) {
                attach->vecScale += value;
            }
            if (CObjectEditor::editType == CObjectEditor::TYPE_VEHICLE_ATTACH) {
                //  pObject->m_vec += value;
            }
        }
        if (button_type == 4) { // rot x
            float value = (button_id) ? (1.0f) : (-1.0f);

            if (CObjectEditor::editType == CObjectEditor::TYPE_PLAYER_ATTACH) {
                attach->vecRotation.x += value;
            }
            if (CObjectEditor::editType == CObjectEditor::TYPE_VEHICLE_ATTACH) {
                pObject->m_vecAttachedRotation.x += value;
            }
            if (CObjectEditor::editType == CObjectEditor::TYPE_OBJECT) {

                float x, y, z;
                pObject->m_pEntity->GetOrientationDeg(x, y, z);
                x += value;

                pObject->InstantRotate(x, y, z);
            }
        }
        if (button_type == 5) { // rot y
            float value = (button_id) ? (1.0f) : (-1.0f);

            if (CObjectEditor::editType == CObjectEditor::TYPE_PLAYER_ATTACH) {
                attach->vecRotation.y += value;
            }
            if (CObjectEditor::editType == CObjectEditor::TYPE_VEHICLE_ATTACH) {
                pObject->m_vecAttachedRotation.y += value;
            }
            if (CObjectEditor::editType == CObjectEditor::TYPE_OBJECT) {
                float x, y, z;
                pObject->m_pEntity->GetOrientationDeg(x, y, z);
                y += value;

                pObject->InstantRotate(x, y, z);
            }
        }
        if (button_type == 6) { // rot z
            float value = (button_id) ? (1.0f) : (-1.0f);

            if (CObjectEditor::editType == CObjectEditor::TYPE_PLAYER_ATTACH) {
                attach->vecRotation.z += value;
            }
            if (CObjectEditor::editType == CObjectEditor::TYPE_VEHICLE_ATTACH) {
                pObject->m_vecAttachedRotation.z += value;
            }
            if (CObjectEditor::editType == CObjectEditor::TYPE_OBJECT) {

                float x, y, z;
                pObject->m_pEntity->GetOrientationDeg(x, y, z);
                Log("rotZ = %.3f", z);
                z += value;

                pObject->InstantRotate(x, y, z);
            }
        }
    });
}
extern "C"
JNIEXPORT void JNICALL
Java_com_lit_game_gui_AttachEdit_Save(JNIEnv *env, jobject thiz) {
    CPedSamp* pPlayer = CLocalPlayer::GetPlayerPed();
    int slot = CObjectEditor::iEditedId;

    if(CObjectEditor::pObject) {
        auto pObject = CObjectEditor::pObject;

        auto pos = pObject->m_pEntity->GetPosition();

        auto mat = pObject->m_pEntity->GetMatrix().ToRwMatrix();
        CQuaternion quat;
        quat.SetFromMatrix(&mat);
        quat.Normalize();

//        CVector rot;
//        pObject->m_pEntity->GetOrientation(rot.x, rot.y, rot.z);

        char ff[0xFF];
        sprintf(ff, "%sSavedObjects.txt", g_pszStorage);
        auto fileOut = fopen(ff, "a");
        if(!fileOut)
            CChatWindow::AddMessage("Cant open sSavedObjects.txt");

        auto modelId = pObject->m_pEntity->m_nModelIndex;
        fprintf(fileOut, "%d, %s, %d, %.3f, %.3f, %.3f, %.3f, %.3f, %.3f, %.3f, -1\n",
                    modelId,
                    CModelInfo::GetModelInfo(modelId)->m_modelName,
                    pPlayer->m_pPed->m_nAreaCode,
                    pos.x, pos.y, pos.z,
                    quat.x, quat.y, quat.z, quat.w
                    );
        fclose(fileOut);

        CChatWindow::AddMessage("-> Saved to sSavedObjects");

        goto exit;
    }
    if(CObjectEditor::editType == CObjectEditor::TYPE_PLAYER_ATTACH) {
        auto attach = pPlayer->GetAttachedObject(slot);

        if(!attach)
            goto exit;

        CObjectEditor::SendOnEditAttach(
                1,
                slot,
                attach->dwModelId,
                attach->dwSampBone,
                attach->vecOffset,
                attach->vecRotation,
                attach->vecScale
        );
    }
    if(CObjectEditor::editType == CObjectEditor::TYPE_OBJECT)
    {
        auto pObject = CObjectPool::GetAt(CObjectEditor::iEditedId);
        if(!pObject)
            goto exit;

        auto pos = pObject->m_pEntity->GetPosition();
        CVector rot;
        pObject->m_pEntity->GetOrientationDeg(rot.x, rot.y, rot.z);

        RakNet::BitStream bsSend;

        bsSend.Write((bool)     CObjectEditor::bPlayerObj);
        bsSend.Write((uint16_t) CObjectEditor::iEditedId);
        bsSend.Write((uint32_t) CObjectEditor::eResponse::EDIT_RESPONSE_FINAL);
        bsSend.Write((float)    pos.x);
        bsSend.Write((float)    pos.y);
        bsSend.Write((float)    pos.z);
        bsSend.Write((float)    rot.x);
        bsSend.Write((float)    rot.y);
        bsSend.Write((float)    rot.z);

        pNetGame->GetRakClient()->RPC(&RPC_EditObject, &bsSend, HIGH_PRIORITY, RELIABLE, 0, false, UNASSIGNED_NETWORK_ID, nullptr);
    }
    if(CObjectEditor::editType == CObjectEditor::TYPE_VEHICLE_ATTACH)
    {
        auto pObject = CObjectPool::GetAt(CObjectEditor::iEditedId);
        if(!pObject)
            goto exit;

        auto pos = pObject->m_vecAttachedOffset;
        auto rot = pObject->m_vecAttachedRotation;

        RakNet::BitStream bsSend;

        bsSend.Write((bool)     CObjectEditor::bPlayerObj);
        bsSend.Write((uint16_t) CObjectEditor::iEditedId);
        bsSend.Write((uint32_t) CObjectEditor::eResponse::EDIT_RESPONSE_FINAL);
        bsSend.Write((float)    pos.x);
        bsSend.Write((float)    pos.y);
        bsSend.Write((float)    pos.z);
        bsSend.Write((float)    rot.x);
        bsSend.Write((float)    rot.y);
        bsSend.Write((float)    rot.z);

        pNetGame->GetRakClient()->RPC(&RPC_EditObject, &bsSend, HIGH_PRIORITY, RELIABLE, 0, false, UNASSIGNED_NETWORK_ID, nullptr);
    }



    exit:
        CObjectEditor::bIsToggle = false;
        CObjectEditor::pObject = nullptr;
        CObjectEditor::iEditedId = INVALID_OBJECT_ID;

        env->DeleteGlobalRef(CObjectEditor::thiz );
        CObjectEditor::thiz = nullptr;
}