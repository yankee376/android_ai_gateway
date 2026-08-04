# edgeAI_android

Flutter Android application for running on-device object detection through
Dart FFI, C++, and NCNN.

## Current status

The active application is based on the Android AI Gateway project.

Current active pipeline:

    Flutter UI
    -> Dart FFI
    -> C++ native bridge
    -> NCNN test processing

NanoDet source code and models have been copied into:

    upstream/ncnn_android_nanodet/

The NanoDet source is currently for reference only and has not yet been
connected to the Flutter application.

## Planned architecture

    Flutter camera
    -> Dart FFI
    -> C++ NanoDet inference
    -> NCNN
    -> detection results
    -> Flutter overlay

## Source attribution

See:

    docs/SOURCES.md
