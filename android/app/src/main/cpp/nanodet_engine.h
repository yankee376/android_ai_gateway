#ifndef NANODET_ENGINE_H
#define NANODET_ENGINE_H

#include <mutex>

#include "net.h"

class NanoDetEngine {
public:
    NanoDetEngine() = default;
    ~NanoDetEngine();

    NanoDetEngine(const NanoDetEngine&) = delete;
    NanoDetEngine& operator=(const NanoDetEngine&) = delete;

    // Trả về:
    //   0  = thành công
    //  -1  = đường dẫn không hợp lệ
    //  -2  = không load được file .param
    //  -3  = không load được file .bin
    int load(
        const char* param_path,
        const char* bin_path,
        bool request_gpu
    );

    void unload();

    bool is_loaded() const;
    bool is_using_gpu() const;

private:
    void clear_unlocked();

    mutable std::mutex mutex_;
    ncnn::Net net_;

    bool loaded_ = false;
    bool using_gpu_ = false;
};

#endif // NANODET_ENGINE_H
