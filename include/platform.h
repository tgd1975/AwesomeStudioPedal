#pragma once

/**
 * @file platform.h
 * @brief Portability shims for platform-specific compiler attributes
 *
 * Include this header in files that use attributes which only exist on
 * certain targets so that the same source compiles on all platforms.
 */

/**
 * IRAM_ATTR places a function in IRAM on the ESP32, enabling fast
 * execution from interrupt context. On other platforms it is a no-op.
 */
#ifndef IRAM_ATTR
#define IRAM_ATTR
#endif
