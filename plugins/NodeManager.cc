//
// Created by g29624 on 2022/8/29.
//

#include <drogon/HttpAppFramework.h>
#include <magic_enum.hpp>
#include <plugins/NodeManager.h>
#include <structures/Exceptions.h>
#include <types/ResultCode.h>

using namespace drogon;
using namespace magic_enum;
using namespace std;
using namespace trantor;
using namespace morpheus::plugins;
using namespace morpheus::structures;
using namespace morpheus::types;

void NodeManager::initAndStart(const Json::Value &config) {
    if (!(
            config["srcDir"].isString() &&
            config["port"].isUInt() &&
            config["account"].isString() &&
            config["password"].isString() &&
            config["senderEmail"].isString() &&
            config["senderName"].isString()
    )) {
        LOG_ERROR << R"(Invalid config)";
        abort();
    }
    LOG_INFO << "NodeManager loaded.";
}

void NodeManager::shutdown() {
    LOG_INFO << "NodeManager shutdown.";
}

void NodeManager::updateNode(NodeServer &&nodeServer) {
    unique_lock<shared_mutex> lock(_sharedMutex);
    auto [nodePair, isNew] = _nodeMap.try_emplace(
            nodeServer.address.ipNetEndian(),
            std::move(nodeServer)
    );
    if (!isNew) {
        nodePair->second.setInfo(std::move(nodeServer.getInfo()));
    }
    nodePair->second.setLastSeen();
    _updateTimer(nodePair->second);
}

string NodeManager::getBestNode() const {
    //TODO: implement logics
    try {
        shared_lock<shared_mutex> lock(_sharedMutex);
        return _nodeMap.begin()->second.address.toIpPort();
    } catch (const out_of_range &) {
        throw ResponseException(
                i18n("notAvailable"),
                ResultCode::NotAvailable,
                k503ServiceUnavailable
        );
    }
}

Json::Value NodeManager::getAllNodes() const {
    Json::Value result(Json::arrayValue);
    try {
        shared_lock<shared_mutex> lock(_sharedMutex);
        for (const auto &[_, nodeServer]: _nodeMap) {
            result.append(nodeServer.address.toIpPort());
        }
    } catch (const out_of_range &) {
        throw ResponseException(
                i18n("notAvailable"),
                ResultCode::NotAvailable,
                k503ServiceUnavailable
        );
    }
    return result;
}

void NodeManager::checkNode(trantor::InetAddress address) const {
    try {
        shared_lock<shared_mutex> lock(_sharedMutex);
        if (!_nodeMap.contains(address.ipNetEndian())) {
            throw ResponseException(
                    i18n("notFound"),
                    ResultCode::NotFound,
                    k404NotFound
            );
        }
    } catch (const out_of_range &) {
        throw ResponseException(
                i18n("notFound"),
                ResultCode::NotFound,
                k404NotFound
        );
    }
}

Json::Value NodeManager::parseInfo() const {
    Json::Value result(Json::arrayValue);
    shared_lock<shared_mutex> lock(_sharedMutex);
    for (const auto &[_, node]: _nodeMap) {
        result.append(node.parse());
    }
    return result;
}

void NodeManager::_updateTimer(NodeServer &nodeServer) {
    if (_waitTimes == 0) {
        // Disable report timeout
        return;
    }
    // Invalidate old report timeout
    app().getLoop()->invalidateTimer(nodeServer.removalTimerId);

    const auto &netEndian = nodeServer.address.ipNetEndian();
    // Set new report timeout
    nodeServer.removalTimerId = app().getLoop()->runAfter(
            nodeServer.taskInterval * _waitTimes,
            [this, netEndian]() {
                unordered_map<uint32_t, NodeServer>::node_type node; // Declare node first to avoid memory leak
                unique_lock<shared_mutex> lock(_sharedMutex);
                node = _nodeMap.extract(netEndian);
                if (node.empty()) {
                    LOG_WARN << "Node " << netEndian << " already removed";
                }
            }
    );
}
