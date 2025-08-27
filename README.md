# 100kinsat neo

ARLISS 2025 TDU Team-Dauntless-Uncharted Bパーツ開発リポジトリ

参考元: [ymt117/tane2024](https://github.com/ymt117/tane2024/tree/main)

## ディレクトリ構造

```
100kinsat_neo
├── 100kinsat_neo.ino               # メインのArduinoスケッチ
├── libraries                       # 外部ライブラリ(git submodule)
│   ├── Adafruit_BNO055             # BNO055のセンサライブラリ
│   ├── Adafruit_BusIO
│   ├── Adafruit_Sensor
│   ├── PosixAvi                    # AVIでの録画
│   ├── spresense_fomo_inferencing  # 物体検知
│   └── TwelitePacket               # AパーツやケースとのTwelite通信
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

## システム構成

### ハードウェア

- **マイコン**: Arduino互換ボード
- **センサー**:
  - GNSSセンサー: 緯度、経度、高度の取得
  - IMUセンサー: 9軸（加速度、ジャイロ、磁気）センサー。機体の姿勢角（特に方位）の取得
  - カメラ: 航行中の風景撮影、着地地点の特定
  - CdSセル: 光センサー。ロケットからの放出検知
- **アクチュエーター**:
  - DCモーター x2: 左右のタイヤを駆動
  - LED: 機体の状態表示
  - スピーカー: ゴール時の音楽再生など
- **その他**:
  - SDカードスロット: ログ保存用
  - 無線通信モジュール: 地上局へのデータ送信

### ソフトウェア (状態遷移)

本ソフトウェアは、以下の状態遷移モデルに基づいて動作します。

### 1. CALIBRATION

- 動作：センサのキャリブレーションをする
- 次のモード：STANDBY
- 移行条件：キャリブレーションが終了次第

### 2. STANDBY

- 動作：高度、経過時間の監視
- 次のモード：LAUNCH
- 移行条件：高度が一定の高さを超える、もしくは一定時間経過する

### 3. LAUNCH

- 動作：CdSの値の監視
- 次のモード：DROP
- 移行条件：CdSが一定の値を下回る

### 4. DROP

- 動作：Tweliteを起動する。ケースからの信号もしくは経過時間の監視
- 次のモード：ESCAPE
- 移行条件：ケースから展開完了の信号を受け取る、もしくは一定時間経過する

### 5. ESCAPE

- 動作：ケースからの脱出
- 次のモード：DETECTION
- 移行条件：脱出できたら（GNSSでの移動検知）、もしくは一定時間経過する

### 6. DETECTION

- 動作：カメラで画像を取り、物体検知処理をする。中央に捉えるまで移動する
- 次のモード：RECORDING
- 移行条件：Aパーツをカメラの中央に捉えたら、もしくはタイムアウト

### 7. RECORDING

- 動作：カメラのモードを変更する。TweliteでAパーツと撮影準備完了のやり取りを行った後、Aパーツが発進する動画を撮影する
- 次のモード：EXPLORE
- 移行条件：Aパーツが発進した動画を撮り終わったら（一定時間録画した後）

### 8. EXPLORE

- 動作：適当に移動（散策）し、周辺環境の画像を撮影する
- 次のモード：HELPING
- 移行条件：一定時間、移動できなくなったら（緯度経度の値がほとんど変わらなかったら）

### 9. HELPING

- 動作：Tweliteで自身の位置情報を格納した救援信号をAパーツに送る
- 次のモード：？
- 移行条件：救助されたら


## セットアップと実行

**※ WindowsはPowerShellで作業することを前提としているので注意**

### 0. 事前準備

以下のツールをインストールし、コマンドを実行できるようにしておく
- [Arduino CLI](https://github.com/arduino/arduino-cli/releases)
- Git
- make (macOS、Linuxのみ)

Spresenseボードマネージャの追加
```bash
$ arduino-cli config add board_manager.additional_urls https://github.com/sony/spresense-arduino-compatible/releases/download/v1.0.0/spresense-arduino-compatible-v1.0.0.zip
$ arduino-cli core update-index
$ arduino-cli core install SPRESENSE:spresense
```

### 1. 環境構築

このリポジトリをクローンする
```bash
$ git clone https://github.com/team-dauntless-uncharted/100kinsat_neo.git
$ git clone git@github.com:team-dauntless-uncharted/100kinsat_neo.git
```

100kinsat_neo直下に移動
```bash
$ cd 100kinsat_neo
```

submoduleを取得
```bash
$ git submodule update --init --recursive
```

### 2.ソースコードのビルド

**macOS・Linuxの場合**

100kinsat_neoディレクトリ直下でmakeコマンドを使用する

```bash
$ make compile
```

**Windowsの場合**

100kinsat_neoディレクトリ直下でbuild.batを実行する
```
$ ./build.bat compile
```

### 3. 書き込み

デバイスはUSBで接続して、認識していればそれを書き込み対象とする

**macOS・Linuxの場合**

```bash
$ make upload
$ make upload PORT="/dev/ttyUSB0" # 任意のポートを指定可能
```

**Windowsの場合**

```bash
$ ./build.bat upload
```

### 4. シリアルモニタ

デバイスはUSBで接続して、認識していればそのデバイスのシリアルモニタを開く

**macOS・Linuxの場合**

```bash
$ make monitor
```

**Windowsの場合**

```bash
$ ./build.bat monitor
```

## ビルドオプション

### Flash有効化

Flashは、簡単に行うのであれば、マイコンからしか読み書きできない、開発中などにむやみに書き込まんで寿命を縮めないようにするために、基本的にビルド時にオプションを付けないと使用しないようになっている

ビルドする際に ``FEATURE_FLAG="-D USE_FLASH"`` を付ける

```bash
$ make compile FEATURE_FLAG="-D USE_FLASH" # Stateの確認にFlashを使用する
```

※ Flashをフォーマットしたい場合は、 ``tools/flash_format/flash_format.ino`` を書き込み、シリアルモニタからyを送ることでできる

### GNSSの値取得待ちの有効化

Spresenseの内蔵GNSSは起動してからすぐに値を取得できないため、値を取得するまで先に進まない機能を有効にできる

ビルドする際に ``FEATURE_FLAG="-D WAIT_GNSS_RECEIVE"`` を付ける

```bash
$ make compile FEATURE_FLAG="-D WAIT_GNSS_RECEIVE"
```

## ユーザ指定の値

SDカードにconfig.jsonファイルを配置することで、任意の値を設定することができる

Calibration.Timeout: CalibrationStateでのタイムアウト時間
Standby.Alt: StandbyStateでのLaunchState移行ための高度の閾値
Standby.Timemout: StandbyStateでのタイムアウト時間
Launch.CdS: LaucnhStateでのDropState移行ためのCdSの値の閾値
Launch.Timeout: LaunchStateでのタイムアウト時間
Drop.Timeout: DropStateでのタイムアウト時間
Escape.Distance: EscapeStateでの移行のための落下地点と現在地点の距離
Escape.Timeout: EscapeStateでのタイムアウト時間
Detection.MaxFailedCount: DetectionStateでの次の状態に移行するまでの物体認識の失敗回数
Recording.Timeout: RecordingStateでのタイムアウト時間
Recording.Time: RecordingStateでの録画時間

```json
{
  "Calibration": {
    "Timeout": 300000
  },
  "Standby": {
    "Alt": 20.0,
    "Timeout": 600000
  },
  "Launch": {
    "CdS": 400,
    "Timeout": 600000
  },
  "Drop": {
    "Timeout": 600000
  },
  "Escape": {
    "Distance": 0.5,
    "Timeout": 20000
  },
  "Detection": {
    "MaxFailedCount": 25
  },
  "Recording": {
    "Timeout": 300000,
    "Time": 30000
  }
}
```

ファイルがない場合、ファイルに上記のキーと値が存在しない場合は、CansatController.hppのUserConfig構造体内で指定しているデフォルト値が使用される
