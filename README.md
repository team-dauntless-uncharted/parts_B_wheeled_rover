# 100kinSAT NEO CanSat Controller

これは、ARLISS (A Rocket Launch for International Student Satellites) 競技向けに開発されたCanSat（缶サット）の制御ソフトウェアです。
GNSSとIMUセンサーを用いて自己位置を推定し、設定されたゴール地点まで自律航行することを目的としています。

## 機能

- **状態遷移**: キャリブレーションからゴールまで、ミッションのフェーズをステートマシンで管理します。
- **自律航行**: GNSSで取得した緯度経度とIMUで取得した方位角を元に、ゴールへ向けてモーターを制御します。
- **センサーデータ取得**: GNSS、IMU（加速度、ジャイロ、磁気）、CdS（光センサー）からデータを定期的に取得します。
- **放出・着地検知**: CdSセンサーによる放出検知、IMUの加速度による着地検知を行います。
- **パラシュート切り離し**: 着地後にヒーター（ニクロム線）を加熱し、パラシュートを切り離します。
- **データロギング**: センサーデータや機体の状態をSDカードにCSV形式で記録します。
- **無線通信**: 機体の状態を地上局へ無線（`Serial2`）で送信します。

## ディレクトリ構造

```
100kinsat_neo/
├── 100kinsat_neo.ino       # メインのArduinoスケッチ
├── platformio.ini          # (PlatformIOを使用する場合)
└── src/
    ├── CansatController.cpp    # メインコントローラーの実装
    ├── CansatController.hpp    # メインコントローラーのヘッダー
    ├── Sensor/                 # センサー関連クラス
    │   ├── Gnss/
    │   ├── Imu/
    │   └── CdS/
    ├── Actuator/               # アクチュエーター関連クラス
    │   ├── Motor/
    │   ├── Led/
    │   ├── Speaker/
    │   └── Heater/
    └── Utils/                  # ユーティリティクラス
        ├── GeoUtils/           # 測地線計算
        └── Logger/             # ロギング
```

## システム構成

### ハードウェア

- **マイコン**: Arduino互換ボード
- **センサー**:
  - GNSSセンサー: 緯度、経度、高度の取得
  - IMUセンサー: 9軸（加速度、ジャイロ、磁気）センサー。機体の姿勢角（特に方位）の取得
  - CdSセル: 光センサー。ロケットからの放出検知
- **アクチュエーター**:
  - DCモーター x2: 左右のタイヤを駆動
  - LED: 機体の状態表示
  - スピーカー: ゴール時の音楽再生など
  - ヒーター（ニクロム線）: パラシュート切り離し
- **その他**:
  - SDカードスロット: ログ保存用
  - 無線通信モジュール: 地上局へのデータ送信

### ソフトウェア (状態遷移)

本ソフトウェアは、以下の状態遷移モデルに基づいて動作します。

1.  **`CALIBRATION`**: 起動直後。IMUなどのセンサーキャリブレーションを行います。
2.  **`STAND_BY`**: 打ち上げ待機状態。ロケットが一定高度に達するか、一定時間が経過すると `LAUNCH` 状態に遷移します。
3.  **`LAUNCH`**: ロケットに搭載されている状態。CdSセンサーが強い光を検知する（=機体が放出される）と `DROP` 状態に遷移します。
4.  **`DROP`**: パラシュートで降下中の状態。IMUの加速度から着地の衝撃を検知すると `LANDING` 状態に遷移します。
5.  **`LANDING`**: 着地後の状態。パラシュートをヒーターで切り離し、`NAVIGATION` 状態に遷移します。
6.  **`NAVIGATION`**: 自律航行状態。ゴール地点に向かって移動します。ゴールに到達すると `GOAL` 状態に遷移します。
7.  **`GOAL`**: ゴール達成。スピーカーで音楽を鳴らすなどのゴールパフォーマンスを行い、待機します。

## セットアップと実行

### 1. 設定

`100kinsat_neo.ino` ファイル内の `setup()` 関数で、ユーザー設定（`userConfig`）を環境に合わせて変更します。

```cpp
// 100kinsat_neo.ino

// ユーザ設定
cansat.userConfig.goalLat = 35.7487860;      // ゴールの緯度
cansat.userConfig.goalLng = 139.8070711;     // ゴールの経度
cansat.userConfig.altThreshold = 20;         // LAUNCH遷移高度 [m]
cansat.userConfig.cdsThreshold = 400;        // DROP遷移CdSしきい値
cansat.userConfig.accThreshold = 0;          // LANDING遷移加速度しきい値
cansat.userConfig.distanceThreshold = 5;     // GOAL遷移距離 [m]
cansat.userConfig.timeThreshold = 30000;     // LAUNCH遷移時間 [ms]
```

### 2. ビルドと書き込み

1.  Arduino IDEまたはPlatformIOなどの開発環境でこのプロジェクトを開きます。
2.  必要なライブラリをインストールします。（ライブラリマネージャで追加してください）
3.  ボードとポートを正しく設定します。
4.  マイコンにプログラムを書き込みます。
5.  シリアルモニター（ボーレート: 115200）を開くと、デバッグメッセージを確認できます。

## ログフォーマット

SDカードには、以下のカラムを持つCSV形式でログが記録されます。

1.  `Time`: 起動からの経過時間 [ms]
2.  `Date`: 日付 [YYYYMMDD]
3.  `State`: 機体の状態 (Enum値)
4.  `Latitude`: 緯度
5.  `Longitude`: 経度
6.  `Altitude`: 高度 [m]
7.  `DistanceToGoal`: ゴールまでの距離 [m]
8.  `DirectionToGoal`: ゴールまでの方位 [度]
9.  `MotorR_PWM`: 右モーターのPWM値
10. `MotorL_PWM`: 左モーターのPWM値
11. `MotorOutputTime`: モーター出力時間 [ms]
12. `CdSValue`: CdSセンサーの値
13. `AccX`, `AccY`, `AccZ`: 加速度
14. `GyroX`, `GyroY`, `GyroZ`: 角速度
15. `MagX`, `MagY`, `MagZ`: 磁気
16. `Roll`, `Pitch`, `Heading`: 姿勢角