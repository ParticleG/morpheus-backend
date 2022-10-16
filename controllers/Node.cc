//
// Created by Particle_G on 2021/8/19.
//

#include <controllers/Node.h>
#include <helpers/ResponseJson.h>
#include <magic_enum.hpp>

using namespace drogon;
using namespace magic_enum;
using namespace std;
using namespace morpheus::api::v1;
using namespace morpheus::helpers;
using namespace morpheus::plugins;
using namespace morpheus::structures;
using namespace morpheus::types;
using namespace trantor;

Node::Node() : _nodeManager(app().getPlugin<NodeManager>()) {}

void Node::allocate(const HttpRequestPtr &req, function<void(const HttpResponsePtr &)> &&callback) {
    ResponseJson response;
    handleExceptions([&]() {
        response.setData(_nodeManager->getBestNode());
    }, response);
    response.to(callback);
}

void Node::check(const HttpRequestPtr &req, function<void(const HttpResponsePtr &)> &&callback) {
    ResponseJson response;
    handleExceptions([&]() {
        _nodeManager->checkNode(
                req->attributes()->get<InetAddress>("address")
        );
    }, response);
    response.to(callback);
}

void Node::report(const HttpRequestPtr &req, function<void(const HttpResponsePtr &)> &&callback) {
    ResponseJson response;
    handleExceptions([&]() {
        const auto &requestJson = req->attributes()->get<RequestJson>("requestJson");
        _nodeManager->updateNode(NodeServer{
                requestJson["ip"].asString(),
                static_cast<uint16_t>(requestJson["port"].asUInt()),
                requestJson["taskInterval"].asDouble(),
                requestJson["description"].asString(),
                requestJson["info"]
        });
    }, response);
    response.to(callback);
}