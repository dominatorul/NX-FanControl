#pragma once

#define NX_GENERATE_SERVICE_GUARD(name) \
    Result name##Initialize(void) { \
        return _##name##Initialize(); \
    } \
    void name##Exit(void) { \
        _##name##Cleanup(); \
    }