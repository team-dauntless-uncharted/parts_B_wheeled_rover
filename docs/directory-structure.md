# ディレクトリ構造

```
parts_B_wheeled_rover
├── parts_B_wheeled_rover.ino         # メインのArduinoスケッチ
├── libraries                         # 外部ライブラリ(git submodule)
│   ├── ArduinoJson
│   ├── BNO055Library                 # BNO055のセンサライブラリ
│   ├── PosixAvi                      # AVIでの録画
│   ├── a_parts_detection_inferencing # 物体検知
├── src
│   ├── Actuator                    # アクチュエータ
│   ├── Controller                  # Cansat制御・各State処理
│   ├── Sensor                      # センサ・カメラ
│   └── Utils                       # ログ・計算など
├── Makefile                        # ビルド、書き込み、シリアルモニタ
├── build.bat                       # Windows用
├── README.md
├── test
└── tools
    └── flash_format                # Flashフォーマット用スケッチ
```
