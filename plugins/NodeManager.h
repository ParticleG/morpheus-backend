//
// Created by g29624 on 2022/8/29.
//

#pragma once

#include <drogon/plugins/Plugin.h>
#include <helpers/I18nHelper.h>
#include <shared_mutex>
#include <structures/NodeServer.h>

namespace morpheus::plugins {
    class NodeManager : public drogon::Plugin<NodeManager>, helpers::I18nHelper<NodeManager> {
    public:
        static constexpr char projectName[] = CMAKE_PROJECT_NAME;
    public:
        void initAndStart(const Json::Value &config) override;

        void shutdown() override;

        void updateNode(structures::NodeServer &&nodeServer);

        std::string getBestNode() const;

        Json::Value getAllNodes() const;

        void checkNode(trantor::InetAddress address) const;

        Json::Value parseInfo() const;
    private:
        mutable std::shared_mutex _sharedMutex;

        std::atomic<uint32_t> _waitTimes;
        std::unordered_map<uint32_t, structures::NodeServer> _nodeMap;

        void _updateTimer(structures::NodeServer &nodeServer);
    };
}
