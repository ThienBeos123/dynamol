#include "../crt_util.h"



void __libdnml_memcpy_strict(void *buf, const void* src, size_t len, size_t start) {}
void __libdnml_memset_strict(void *buf, uint8_t val, size_t len, size_t start) {
    uint8_t *p = (uint8_t*)buf;
    for (size_t i = 0; i < len; ++i) {
        uint8_t curr = p[i];
        p[i] = (i >= start) ? val : curr;
    }
}
void __libdnml_memwipe_strict(void *buf, size_t len) {
    uint8_t *p = (uint8_t*)buf;
    for (size_t i = 0; i < len; ++i) p[i] = 0;
}