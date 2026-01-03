#pragma once
#include <jni.h>
#include <string>
#include <unordered_map>
#include <functional>
#include "GuiWrapper.h"

class CCEF : public CGuiWrapper<CCEF> {
public:
    enum class ePacketType : uint8_t { EXIT, SHOW };

    static void Init(const std::string& url);

    static void Show();
    static void Hide();

    static void SetSize(float size);
    static void SetUrl(const std::string& url);

    static void SendEvent(const std::string& event, const std::string& json);
    static void RegisterCallback(const std::string& event, std::function<void(const std::string&)> cb);

    static void OnJsEvent(const std::string& event, const std::string& data);

    // pkt
    static void pktInit(Packet *p);
    static void pktShow(Packet *p);
    static void pktHide(Packet *p);
    static void pktSetSize(Packet *p);
    static void pktSetUrl(Packet *p);

private:
    static std::unordered_map<std::string, std::function<void(const std::string&)>> callbacks;
};
