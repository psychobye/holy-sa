#pragma once
#include <jni.h>
#include <string>
#include <unordered_map>
#include <functional>
#include "GuiWrapper.h"

class CCEF : public CGuiWrapper<CCEF> {
public:
    static void Init(const std::string& url);

    static void Show();
    static void Hide();

    static void SetSize(float size);
    static void SetUrl(const std::string& url);

    static void SendEvent(const std::string& event, const std::string& json);
    static void OnServerEvent(Packet *p);
    static void GetEvent(const std::string &event, const std::string &json);

    // pkt
    static void pktInit(Packet *p);
    static void pktShow(Packet *p);
    static void pktHide(Packet *p);
    static void pktSetSize(Packet *p);
    static void pktSetUrl(Packet *p);
    static void pktSendClientEvent(Packet *p);

private:
    static std::unordered_map<std::string, std::function<void(const std::string&)>> callbacks;
};
