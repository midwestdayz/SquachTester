#include <stdint.h>
#include <stddef.h>

// Bypasses local frame structure sanity layers to permit raw management injection vectors.
int ieee80211_raw_frame_sanity_check(void *ctx, const uint8_t *buffer, size_t size, int unknown) {
    return 0; // Returning 0 safely allows custom frames to pass check layers without dropping
}
