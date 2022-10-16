//
// Created by Particle_G on 2021/8/19.
//

#include <structures/NodeServer.h>

using namespace std;
using namespace morpheus::structures;
using namespace trantor;

NodeServer::NodeServer(
        const string &ip,
        uint16_t port,
        double taskInterval,
        string description,
        Json::Value info
) : address(ip, port),
    taskInterval(taskInterval),
    description(std::move(description)),
    _info(std::move(info)) {}

NodeServer::NodeServer(NodeServer &&nodeServer) noexcept:
        address(nodeServer.address),
        taskInterval(nodeServer.taskInterval),
        description(nodeServer.description),
        _info(nodeServer._info) {}

Json::Value NodeServer::getInfo() const {
    shared_lock<shared_mutex> lock(_sharedMutex);
    return _info;
}

void NodeServer::setInfo(Json::Value info) {
    unique_lock<shared_mutex> lock(_sharedMutex);
    _info = std::move(info);
}

int64_t NodeServer::getLastSeen() const {
    shared_lock<shared_mutex> lock(_sharedMutex);
    return _lastSeen.microSecondsSinceEpoch();
}

void NodeServer::setLastSeen() {
    unique_lock<shared_mutex> lock(_sharedMutex);
    _lastSeen = Date::now();
}

Json::Value NodeServer::parse() const {
    Json::Value result;
    result["host"] = address.toIpPort();
    result["description"] = description;
    result["info"] = getInfo();
    result["timerId"] = removalTimerId.load();
    result["lastSeen"] = getLastSeen();
    return result;
}