#define NX_SERVICE_ASSUME_NON_DOMAIN
#include <switch.h>
#include "pwm.h"

static Service g_pwmSrv;

// Forward declarations
Result _pwmInitialize(void);
void _pwmCleanup(void);

// Service guard macro
#define NX_GENERATE_SERVICE_GUARD(name) \
    Result name##Initialize(void) { \
        return _##name##Initialize(); \
    } \
    void name##Exit(void) { \
        _##name##Cleanup(); \
    }

NX_GENERATE_SERVICE_GUARD(pwm);

Result _pwmInitialize(void) {
    return smGetService(&g_pwmSrv, "pwm");
}

void _pwmCleanup(void) {
    serviceClose(&g_pwmSrv);
}

Service* pwmGetServiceSession(void) {
    return &g_pwmSrv;
}

Result pwmOpenSession2(PwmChannelSession *out, u32 device_code) {
    return serviceDispatchIn(&g_pwmSrv, 2, device_code,
        .out_num_objects = 1,
        .out_objects = &out->s,
    );
}

Result pwmChannelSessionGetDutyCycle(PwmChannelSession *c, double* out) {
    return serviceDispatchOut(&c->s, 7, *out);
}

void pwmChannelSessionClose(PwmChannelSession *controller) {
    serviceClose(&controller->s);
}