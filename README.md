# RSpace Asset Library Plugin

This plugin provides a comprehensive solution for managing and interacting with multimedia assets, including images, audios, videos, and models, in Unreal Engine. It is designed to streamline media playback, asset downloading, importing, and user interaction.

## Features

- **Video Playback**: 
  - Play video files in a custom video player (`SVideoPlayerWidget`).
  - Support for progress tracking, play/pause control, and resizing.

- **Audio Playback**:
  - Play audio files in a dedicated audio player (`SAudioPlayerWidget`).
  - Real-time progress and playback control.

- **Image Viewer**:
  - Display image files in a separate window.
  - Auto-sizing and scaling support.

- **Model Management**:
  - Download and organize 3D models.
  - Import models into the Unreal Engine asset library for immediate use.

- **Media Import**:
  - Import and register supported video, audio, image, and model formats into the Unreal Engine asset library.
  - Support for WAV, MP3, OGG audio formats; PNG, JPG image formats; and common video formats.

## Installation

1. Donwload plugin from Fab.
2. Open your Unreal Engine project and enable the plugin in the Plugins Manager.
3. Restart the editor.

## Usage

### Download Management
1. Download assets to local.
2. The assets are saved in the project’s `RSpaceImportedAssets` directory.
2. Import assets directly into Unreal Engine.


## Supported Formats

- **Video**: MP4, AVI, WMV, etc.
- **Audio**: WAV, MP3, OGG.
- **Images**: PNG, JPG.
- **Models**: FBX, OBJ.

## Dependencies

- Unreal Engine 5.2.1
- `Media Player`, `Media Texture`, and `Media Sound` components are used for playback.
- `FAssetRegistryModule` for asset management.

## Support

For issues or suggestions, please contact [meta-op@mg.xyz].

---

**Copyright (C) 2024 Hunan MangoXR Tech Co., Ltd. All Rights Reserved.**
