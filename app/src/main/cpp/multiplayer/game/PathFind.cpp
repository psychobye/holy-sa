//
// Created by Traw-GG on 05.10.2025.
//

#include "PathFind.h"
#include "util/patch.h"

void CPathFind::InjectHooks() {

}

void CPathFind::DoPathSearch(
        ePathType pathType,
        CVector originPos,
        CNodeAddress originAddrHint,
        CVector targetPos,
        CNodeAddress* outResultNodes,
        int16* outNodesCount,
        int32 maxNodesToFind,
        float* outDistance,
        float maxSearchDistance,
        CNodeAddress* targetNodeAddrHint,
        float maxSearchDepth,
        bool sameLaneOnly,
        CNodeAddress forbiddenNodeAddr,
        bool bAllowWaterNodeTransitions,
        bool forBoats
) {
    *outNodesCount = 0;

    const auto ResolveNode = [&, this](CVector nodePosn, CNodeAddress* addr) {
        if (addr && addr->IsValid()) {
            if (IsAreaNodesAvailable(*addr)) {
                return GetPathNode(*addr);
            }
        }
        const auto foundAddr = FindNodeClosestToCoors(
                nodePosn,
                pathType,
                maxSearchDistance,
                false,
                false,
                false,
                forBoats,
                false
        );
        return foundAddr.IsValid() ? GetPathNode(foundAddr) : nullptr;
    };

    CPathNode *origin = ResolveNode(originPos, &originAddrHint);
    CPathNode *target = ResolveNode(targetPos, targetNodeAddrHint);

    if (!origin || !target) {
        if (outDistance) *outDistance = 100000.0f;
        *outNodesCount = 0;
        return;
    }

    if (origin->GetAddress() == target->GetAddress()) {
        if (outDistance) *outDistance = 0.0f;
        *outNodesCount = 0;
        return;
    }

    if (origin->m_nFloodFill != target->m_nFloodFill) {
        if (outDistance) *outDistance = 100000.0f;
        *outNodesCount = 0;
        return;
    }

    std::fill(m_pathFindHashTable, m_pathFindHashTable + std::size(m_pathFindHashTable), nullptr);
    m_totalNumNodesInPathFindHashTable = 0;

    AddNodeToList(target, 0);

    size_t numNodesToBeCleared = 0;
    auto AddNodeToBeCleared = [&](CPathNode* node) {
        if (numNodesToBeCleared < std::size(aNodesToBeCleared)) {
            aNodesToBeCleared[numNodesToBeCleared++] = node->GetAddress();
        }
    };
    AddNodeToBeCleared(target);

    size_t iterDepth = 0;
    bool foundPath = false;

    while (true) {
        for (auto node = m_pathFindHashTable[iterDepth % std::size(m_pathFindHashTable)]; node; node = node->m_next) {
            if (node->GetAddress() == origin->GetAddress()) {
                foundPath = true;
            }

            for (auto linkNum = 0u; linkNum < node->m_nNumLinks; linkNum++) {
                const auto linkIdx = node->m_wBaseLinkId + linkNum;
                const auto linkedAddr = pAdjacentNodes[node->m_wAreaId][linkIdx];

                if (!IsAreaNodesAvailable(linkedAddr)) continue;

                auto linked = GetPathNode(linkedAddr);

                if (sameLaneOnly) {
                    const auto& naviLinkAddr = pAdjacentLinks[node->m_wAreaId][linkIdx];
                    if (IsAreaLoaded(naviLinkAddr.m_wAreaId)) {
                        const auto& naviLink = GetCarPathLink(naviLinkAddr);
                        if ((naviLink.m_attachedTo == linked->GetAddress() && !naviLink.m_numOppositeDirLanes) ||
                            (!naviLink.m_numSameDirLanes)) {
                            continue;
                        }
                    }
                }

                if (forbiddenNodeAddr == linked->GetAddress()) continue;
                if (node->m_bWaterNode != linked->m_bWaterNode && !bAllowWaterNodeTransitions) continue;

                const float newDist = node->m_totalDistFromOrigin + pAdjacentNodesDist[node->m_wAreaId][linkIdx];

                if (newDist < linked->m_totalDistFromOrigin) {
                    if (linked->m_totalDistFromOrigin != SHRT_MAX - 1) {
                        RemoveNodeFromList(linked);
                    } else {
                        AddNodeToBeCleared(linked);
                    }
                    linked->m_totalDistFromOrigin = newDist;
                    AddNodeToList(linked, newDist);
                }
            }

            RemoveNodeFromList(node);
        }

        if (!m_totalNumNodesInPathFindHashTable) break;
        if (++iterDepth > maxSearchDepth) break;
        if (numNodesToBeCleared >= std::size(aNodesToBeCleared) - 50) break;
        if (foundPath) break;
    }

    if (foundPath) {
        if (outDistance) *outDistance = origin->m_totalDistFromOrigin;

        if (outResultNodes && maxNodesToFind > 0) {
            outResultNodes[0] = origin->GetAddress();
            *outNodesCount = 1;

            CPathNode* current = origin;
            while (*outNodesCount < maxNodesToFind && current != target) {
                bool foundNext = false;

                for (auto linkNum = 0u; linkNum < current->m_nNumLinks && !foundNext; linkNum++) {
                    const auto linkIdx = current->m_wBaseLinkId + linkNum;
                    const auto linkedAddr = pAdjacentNodes[current->m_wAreaId][linkIdx];

                    if (!IsAreaNodesAvailable(linkedAddr)) continue;

                    auto linked = GetPathNode(linkedAddr);
                    if (current->m_totalDistFromOrigin - pAdjacentNodesDist[current->m_wAreaId][linkIdx] == linked->m_totalDistFromOrigin) {
                        outResultNodes[*outNodesCount] = linked->GetAddress();
                        (*outNodesCount)++;
                        current = linked;
                        foundNext = true;
                    }
                }

                if (!foundNext) break;
            }
        }
    } else {
        if (outDistance) *outDistance = 100000.0f;
        *outNodesCount = 0;
    }

    for (size_t i = 0; i < numNodesToBeCleared; i++) {
        GetPathNode(aNodesToBeCleared[i])->m_totalDistFromOrigin = SHRT_MAX - 1;
    }
}

CPathNode* CPathFind::GetPathNode(CNodeAddress address) {
    assert(address.IsValid());
    assert(IsAreaNodesAvailable(address));
    return &m_pPathNodes[address.m_wAreaId][address.m_wNodeId];
}

void CPathFind::UpdateStreaming(bool bForceStreaming) {
    CHook::CallFunction<void>(g_libGTASA + (VER_x32 ? 0x003195F4 + 1 : 0x3E0208), this, bForceStreaming);
}

std::span<CPathNode> CPathFind::GetPathNodesInArea(size_t areaId, ePathType ptype) const {
    if (const auto allNodes = m_pPathNodes[areaId]) {
        const auto numVehNodes = m_anNumVehicleNodes[areaId];
        switch (ptype) {
            case ePathType::PATH_TYPE_VEH: // Vehicles, boats, race tracks
                return std::span{ allNodes, m_anNumVehicleNodes[areaId] };
            case ePathType::PATH_TYPE_PED: // Peds only
                assert(m_anNumPedNodes[areaId] == m_anNumNodes[areaId] - numVehNodes); // Pirulax: I'm assuming this is true, so if this doesnt assert for a long time remove it
                return std::span{ allNodes + numVehNodes, m_anNumPedNodes[areaId] };
            case ePathType::PATH_TYPE_ALL: // All of the above
                return std::span{ allNodes, m_anNumNodes[areaId] };
            default:
                Log("Invalid pathType: %d", (int)ptype);
        }
    }
    return {}; // Area not loaded, return nothing.. Perhaps assert here instead?
}

bool CPathFind::IsWaterNodeNearby(CVector position, float radius) {
    for (auto areaId = 0u; areaId < NUM_PATH_MAP_AREAS; areaId++) {
        for (const auto& node : GetPathNodesInArea(areaId, PATH_TYPE_VEH)) {
            if (node.m_bWaterNode) {
                if ((node.GetNodeCoors() - position).SquaredMagnitude() <= sq(radius)) {
                    return true;
                }
            }
        }
    }
    return false;
}

CNodeAddress CPathFind::FindNodeClosestToCoors(
        CVector pos,
        ePathType nodeType,
        float maxDistance,
        uint16 unk2,
        int32 unk3,
        uint16 unk4,
        uint16 bBoatsOnly,
        int32 unk6
) {
    return CHook::CallFunction<CNodeAddress>("_ZN9CPathFind22FindNodeClosestToCoorsE7CVectorhfbbbbb",
                                             this,
                                             pos,
                                             nodeType,
                                             maxDistance,
                                             unk2,
                                             unk3,
                                             unk4,
                                             bBoatsOnly,
                                             unk6);
}

void CPathFind::RemoveNodeFromList(CPathNode* node) {
    node->m_prev->m_next = node->m_next;
    if (node->m_next) {
        node->m_next->m_prev = node->m_prev;
    }

    m_totalNumNodesInPathFindHashTable--;
}

void CPathFind::AddNodeToList(CPathNode* node, int32 distFromOrigin) {
    // Insert the node as the head into it's bucket

    auto& head = m_pathFindHashTable[distFromOrigin % std::size(m_pathFindHashTable)];

    node->m_next = head;

    // Make this node's `next` point to the head in the hash table
    // I guess this works as long as you only access the `m_prev` variable
    // as that's at offset 0
    // This is a really bad hack to avoid having to do special handling
    // for the head in `RemoveNodeFromList`...
    node->m_prev = reinterpret_cast<CPathNode*>(&head);

    if (head) {
        head->m_prev = node;
    }

    head = node;

    assert(distFromOrigin <= std::numeric_limits<decltype(node->m_totalDistFromOrigin)>::max()); // Prevent bugs from overflow
    node->m_totalDistFromOrigin = (int16)distFromOrigin;

    m_totalNumNodesInPathFindHashTable++;
}