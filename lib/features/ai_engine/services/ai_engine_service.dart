import 'dart:ffi' as ffi;

import 'package:ffi/ffi.dart';
import 'package:flutter/foundation.dart';

import '../domain/ai_inference_result.dart';
import '../native/native_ai_bindings.dart';
import 'model_asset_service.dart';

class AIEngineService {
  final NativeAIBindings _bindings = NativeAIBindings();
  final ModelAssetService _modelAssetService = ModelAssetService();

  int _lastModelLoadCode = -999;

  bool get isNativeLoaded => _bindings.isLoaded;

  bool get isModelLoaded => _bindings.isNanoDetModelLoaded();

  int get lastModelLoadCode => _lastModelLoadCode;

  int get modelBackend => _bindings.getNanoDetBackend();

  String get modelBackendName {
    return switch (modelBackend) {
      1 => 'Vulkan GPU',
      0 => 'CPU',
      _ => 'Not loaded',
    };
  }

  int getVersion() {
    return _bindings.getEngineVersion();
  }

  bool hasVulkanGPU() {
    return _bindings.hasNcnnVulkan();
  }

  Future<bool> initializeModel({bool preferGpu = false}) async {
    if (!_bindings.isLoaded) {
      _lastModelLoadCode = -100;
      return false;
    }

    ffi.Pointer<Utf8>? paramPathPointer;
    ffi.Pointer<Utf8>? binPathPointer;

    try {
      final NanoDetModelFiles modelFiles = await _modelAssetService
          .prepareNanoDetModel();

      debugPrint('NanoDet param path: ${modelFiles.paramPath}');

      debugPrint('NanoDet bin path: ${modelFiles.binPath}');

      paramPathPointer = modelFiles.paramPath.toNativeUtf8();

      binPathPointer = modelFiles.binPath.toNativeUtf8();

      _lastModelLoadCode = _bindings.loadNanoDetModel(
        paramPathPointer.cast<ffi.Char>(),
        binPathPointer.cast<ffi.Char>(),
        preferGpu,
      );

      debugPrint('NanoDet load result: $_lastModelLoadCode');

      return _lastModelLoadCode == 0 && _bindings.isNanoDetModelLoaded();
    } catch (error, stackTrace) {
      _lastModelLoadCode = -101;

      debugPrint('NanoDet initialization error: $error');

      debugPrintStack(stackTrace: stackTrace);

      return false;
    } finally {
      if (paramPathPointer != null) {
        calloc.free(paramPathPointer);
      }

      if (binPathPointer != null) {
        calloc.free(binPathPointer);
      }
    }
  }

  void unloadModel() {
    _bindings.unloadNanoDetModel();
  }

  AIInferenceResultModel processFrame({
    required Uint8List bytes,
    required int width,
    required int height,
    required int format,
  }) {
    if (!_bindings.isLoaded) {
      return AIInferenceResultModel.empty();
    }

    final ffi.Pointer<ffi.Uint8> pointer = calloc<ffi.Uint8>(bytes.length);

    final Uint8List nativeList = pointer.asTypedList(bytes.length);

    nativeList.setAll(0, bytes);

    try {
      final nativeResult = _bindings.processFrame(
        pointer,
        width,
        height,
        format,
      );

      return AIInferenceResultModel(
        width: nativeResult.width,
        height: nativeResult.height,
        inferenceTimeMs: nativeResult.inferenceTimeMs,
        detectedClassId: nativeResult.detectedClassId,
        confidence: nativeResult.confidence,
      );
    } catch (error) {
      debugPrint('NCNN AI processing error: $error');

      return AIInferenceResultModel.empty();
    } finally {
      calloc.free(pointer);
    }
  }
}
