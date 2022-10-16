//
// Created by ParticleG on 2022/2/11.
//

#pragma once

namespace morpheus::types {
    enum class JsonValue {
        Null,
        Object,
        Array,
        Bool,
        UInt,
        UInt64,
        Int,
        Int64,
        Double,
        String,
    };
}