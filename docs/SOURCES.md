# Upstream sources

## Flutter Android AI Gateway

- Source: https://github.com/Mr-QB/android_ai_gateway
- Commit copied: f28f430e06c0b6dd26d15e5b3e6b5bc34f659342
- Role in this project:
  - Flutter application structure
  - Camera preview
  - Dart FFI bindings
  - Native C/C++ bridge
  - NCNN test pipeline

The active Flutter and Android project at the repository root was initially
copied from this source.

## NCNN Android NanoDet

- Source: https://github.com/nihui/ncnn-android-nanodet
- Commit copied: 76fd1c34515bcec44db5189a62cc0308c647a30d
- Role in this project:
  - NanoDet C++ inference reference
  - NCNN model files
  - Preprocessing and post-processing reference
  - Original Android JNI and NDK camera example

The original NanoDet source is stored under:

    upstream/ncnn_android_nanodet/

Files in upstream/ are reference materials and are not currently connected
to the Flutter build.
