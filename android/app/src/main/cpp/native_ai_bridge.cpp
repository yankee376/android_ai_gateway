#include "native_ai_bridge.h"

#include <android/log.h>
#include <chrono>

#include "gpu.h"
#include "nanodet_engine.h"
#include "net.h"

#define LOG_TAG "NativeAIEngine"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

namespace {
NanoDetEngine g_nanodet_engine;
}

extern "C" {

AI_EXPORT int32_t get_ai_engine_version(void) {
    LOGI("get_ai_engine_version called from Flutter Dart FFI");
    return 110; // Version thử nghiệm 1.1.0
}

AI_EXPORT int32_t get_ncnn_has_vulkan(void) {
#if NCNN_VULKAN
    const int gpu_count = ncnn::get_gpu_count();
    LOGI("NCNN Vulkan GPU Count: %d", gpu_count);
    return gpu_count > 0 ? 1 : 0;
#else
    LOGI("NCNN compiled without Vulkan support");
    return 0;
#endif
}

AI_EXPORT int32_t load_nanodet_model(
    const char* param_path,
    const char* bin_path,
    int32_t use_gpu
) {
    LOGI(
        "load_nanodet_model called. use_gpu=%d",
        use_gpu
    );

    return g_nanodet_engine.load(
        param_path,
        bin_path,
        use_gpu == 1
    );
}

AI_EXPORT int32_t is_nanodet_model_loaded(void) {
    return g_nanodet_engine.is_loaded() ? 1 : 0;
}

AI_EXPORT int32_t get_nanodet_backend(void) {
    if (!g_nanodet_engine.is_loaded()) {
        return -1;
    }

    return g_nanodet_engine.is_using_gpu() ? 1 : 0;
}

AI_EXPORT void unload_nanodet_model(void) {
    g_nanodet_engine.unload();
}

AI_EXPORT AIInferenceResult process_image_frame(
    const uint8_t* image_bytes,
    int32_t width,
    int32_t height,
    int32_t format
) {
    const auto start_time =
        std::chrono::high_resolution_clock::now();

    LOGI(
        "NCNN Processing frame: %dx%d (Format: %d)",
        width,
        height,
        format
    );

    // Bước cũ vẫn được giữ để kiểm tra truyền ảnh.
    // Chưa chạy NanoDet inference ở commit này.
    if (image_bytes != nullptr && width > 0 && height > 0) {
        ncnn::Mat in = ncnn::Mat::from_pixels(
            image_bytes,
            ncnn::Mat::PIXEL_RGB,
            width,
            height
        );

        LOGI(
            "NCNN Mat created successfully: %dx%d, channels: %d",
            in.w,
            in.h,
            in.c
        );
    }

    const auto end_time =
        std::chrono::high_resolution_clock::now();

    const std::chrono::duration<float, std::milli> duration =
        end_time - start_time;

    AIInferenceResult result{};
    result.width = width;
    result.height = height;
    result.channels = 3;
    result.inference_time_ms = duration.count();

    // Vẫn là kết quả giả. Sẽ bỏ ở mốc inference tiếp theo.
    result.detected_class_id = 1;
    result.confidence = 0.98f;

    return result;
}

}
