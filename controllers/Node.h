//
// Created by Particle_G on 2021/8/19.
//

#pragma once

#include <drogon/HttpController.h>
#include <plugins/NodeManager.h>
#include <structures/ExceptionHandlers.h>

namespace morpheus::api::v1 {
    class Node :
            public drogon::HttpController<Node>,
            public structures::ResponseJsonHandler<Node> {
    public:
        static constexpr char projectName[] = CMAKE_PROJECT_NAME;

    public:
        Node();

        METHOD_LIST_BEGIN
            METHOD_ADD(Node::allocate, "/allocate", drogon::Get);
            METHOD_ADD(Node::check, "/check", drogon::Get, "morpheus::filters::NodeCheck");
            METHOD_ADD(Node::report, "/report", drogon::Post, "morpheus::filters::NodeReport");
        METHOD_LIST_END

        void allocate(const drogon::HttpRequestPtr &req, std::function<void(const drogon::HttpResponsePtr &)> &&callback);

        void check(const drogon::HttpRequestPtr &req, std::function<void(const drogon::HttpResponsePtr &)> &&callback);

        void report(const drogon::HttpRequestPtr &req, std::function<void(const drogon::HttpResponsePtr &)> &&callback);

    private:
        plugins::NodeManager *_nodeManager;
    };
}

