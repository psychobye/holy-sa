#pragma once

#include "chatwindow.h"
#include "main.h"
#include "tBikeHandlingData.h"

class CHandlingDefault
{
    static inline std::unordered_map<uint32, tHandlingData*> s_handlingList {};
    static inline std::unordered_map<uint32, tBikeHandlingData*> s_bikeHandlingList {};

public:
    static tHandlingData* GetAt(uint32 id) {

        auto it = s_handlingList.find(id);
        if(it != s_handlingList.end())
            return it->second;

        else {
            CChatWindow::AddMessage("No finded default handling for id %d ", id);

            auto h = s_handlingList.find(0);
            if (h == s_handlingList.end()) {
                Log("No 0 id handling %d ");
                exit(0);
            }

            return h->second;
        }
    }

    static tBikeHandlingData* GetBikeAt(uint32 id) {

        auto it = s_bikeHandlingList.find(id);
        if(it != s_bikeHandlingList.end())
            return it->second;

        else {
            CChatWindow::AddMessage("No finded default handling for id %d ", id);

            auto h = s_bikeHandlingList.find(0);
            if (h == s_bikeHandlingList.end()) {
                Log("No 0 id handling %d ");
                exit(0);
            }

            return h->second;
        }
    }

    static tHandlingData* GetCopyDefaultHandling(uint32 id)
    {
        auto handling = new tHandlingData;

        memcpy((void*)handling, (void*)GetAt(id), sizeof(tHandlingData));

        return handling;
    }

    static tBikeHandlingData* GetCopyBikeDefaultHandling(uint32 id)
    {
        auto handling = new tBikeHandlingData;

        memcpy((void*)handling, (void*)GetBikeAt(id), sizeof(tBikeHandlingData));

        return handling;
    }

    static void FillDefaultHandling(uint32 id, const tHandlingData* src)
    {
        auto handling = new tHandlingData;

        if (src) {
            memcpy((void*)handling, (void*)src, sizeof(tHandlingData));
        }
        s_handlingList[id] = handling;
    }

    static void FillDefaultBikeHandling(uint32 id, const tBikeHandlingData* src)
    {
        auto handling = new tBikeHandlingData;

        if (src) {
            memcpy((void*)handling, (void*)src, sizeof(tBikeHandlingData));
        }
        s_bikeHandlingList[id] = handling;
    }
};