#include "nanodet_engine.h"

#include <android/log.h>
#include <cstring>

#include "cpu.h"
#include "gpu.h"

#define LOG_TAG "NativeAIEngine"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

NanoDetEngine::~NanoDetEngine() {
    unload();
}

void NanoDetEngine::clear_unlocked() {
    net_.clear();
    loaded_ = false;
    using_gpu_ = false;
}

int NanoDetEngine::load(
    const char* param_path,
    const char* bin_path,
    bool request_gpu
) {
    std::lock_guard<std::mutex> guard(mutex_);

    clear_unlocked();

    if (param_path == nullptr ||
        bin_path == nullptr ||
        std::strlen(param_path) == 0 ||
        std::strlen(bin_path) == 0) {
        LOGE("NanoDet model paths are invalid");
        return -1;
    }

    LOGI("Loading NanoDet param: %s", param_path);
    LOGI("Loading NanoDet bin: %s", bin_path);

    int thread_count = ncnn::get_big_cpu_count();
    if (thread_count < 1) {
        thread_count = 1;
    }

    ncnn::set_cpu_powersave(2);
    ncnn::set_omp_num_threads(thread_count);

    bool enable_gpu = false;

#if NCNN_VULKAN
    if (request_gpu) {
        const int gpu_count = ncnn::get_gpu_count();
        enable_gpu = gpu_count > 0;

        LOGI(
            "NanoDet GPU requested. NCNN GPU count: %d",
            gpu_count
        );
    }
#else
    (void)request_gpu;
    LOGI("NCNN was compiled without Vulkan support");
#endif

    // Phải cấu hình option trước khi load model.
    net_.opt = ncnn::Option();
    net_.opt.num_threads = thread_count;

#if NCNN_VULKAN
    net_.opt.use_vulkan_compute = enable_gpu;
#endif

    const int param_result = net_.load_param(param_path);

    if (param_result != 0) {
        LOGE(
            "Failed to load NanoDet param. Code: %d",
            param_result
        );
        clear_unlocked();
        return -2;
    }

    const int model_result = net_.load_model(bin_path);

    if (model_result != 0) {
        LOGE(
            "Failed to load NanoDet bin. Code: %d",
            model_result
        );
        clear_unlocked();
        return -3;
    }

    loaded_ = true;
    using_gpu_ = enable_gpu;

    LOGI(
        "NanoDet model loaded successfully. Backend: %s, threads: %d",
        using_gpu_ ? "Vulkan GPU" : "CPU",
        thread_count
    );

    return 0;
}

void NanoDetEngine::unload() {
    std::lock_guard<std::mutex> guard(mutex_);

    if (loaded_) {
        LOGI("Unloading NanoDet model");
    }

    clear_unlocked();
}

bool NanoDetEngine::is_loaded() const {
    std::lock_guard<std::mutex> guard(mutex_);
    return loaded_;
}

bool NanoDetEngine::is_using_gpu() const {
    std::lock_guard<std::mutex> guard(mutex_);
    return loaded_ && using_gpu_;
}
