# parts_B_wheeled_rover

ARLISS 2025 TDU Team-Dauntless-Uncharted Bパーツ（ホイールドローバー）開発リポジトリ

参考元: [ymt117/tane2024](https://github.com/ymt117/tane2024/tree/main)  

## システム概要

Aパーツ（浮遊ローバー）と連携する2機構成のCanSatシステムにおける地上走行機。  
ロケット放出後、自律的にケースから脱出し、Aパーツの発進シーンを撮影した後、周辺環境の探索を行う。

## ハードウェア構成

| カテゴリ | コンポーネント |
|---------|--------------|
| マイコン | Sony SPRESENSE |
| カメラ | SPRESENSE カメラモジュール |
| IMU | BNO055 9軸センサ（センサフュージョン内蔵） |
| GNSS | SPRESENSE 内蔵GNSS |
| 光センサ | CdSセル（GL5528）- ロケット放出検知用 |
| モーター | FEETECH FM90 ギヤードモーター x2 |
| モータードライバ | TB6612 |
| 無線 | TWE-Lite 2.4GHz（Aパーツとの通信） |
| スピーカー | 圧電スピーカー |
| ストレージ | microSDカード |
| 電源 | 単三電池 x4（6V）+ 3.3V/5V レギュレータ |

## 主な機能

### ソフトウェア状態遷移

State Patternで実装した8段階のミッション管理（詳細: [状態遷移](docs/state-machine.md)）

| 状態 | 内容 |
|------|------|
| CALIBRATION | センサキャリブレーション |
| STANDBY | 打ち上げ待機 |
| LAUNCH | CdSによる放出検知 |
| DROP | パラシュート降下中の監視 |
| ESCAPE | ケースからの脱出（4種の動作パターン） |
| DETECTION | Edge AIによるAパーツの検知・追尾 |
| RECORDING | Aパーツ発進シーンの動画撮影（30fps QVGA, 30秒） |
| EXPLORE | 自律走行・周辺環境の撮影 |

### Edge AI（物体検知）

- Edge Impulse FOMO モデルによるAパーツ検知
- 精度: F1スコア 95.0%（Precision: 0.92 / Recall: 0.98）
- 96×96 RGB画像をオンデバイスで推論

### データ管理

- SD・Flashの二重記録によるクラッシュリカバリ
- センサログ（GPS座標・9軸IMU・CdS・電圧）を200msごとにCSV記録
- 4KBリングバッファでSD書き込みを効率化
- `config.json` でパラメータをリコンパイルなしに変更可能

## ディレクトリ構造

```
parts_B_wheeled_rover
├── parts_B_wheeled_rover.ino   # メインのArduinoスケッチ
├── src                         # ソースコード
├── libraries                   # 外部ライブラリ (git submodule)
├── docs                        # ドキュメント
├── tools                       # 各種ツール
├── test
├── Makefile
└── build.bat                   # Windows用
```

## ドキュメント

| ドキュメント | 内容 |
|------------|------|
| [ディレクトリ構造](docs/directory-structure.md) | リポジトリのディレクトリ構成 |
| [ハードウェア](docs/hardware.md) | センサー・アクチュエーターなどのハードウェア構成 |
| [状態遷移](docs/state-machine.md) | ソフトウェアの状態遷移モデル |
| [セットアップ](docs/setup.md) | 環境構築・ビルド・書き込み手順 |
| [ビルドオプション](docs/build-options.md) | Flash有効化などのビルドオプション |
| [設定](docs/configuration.md) | 状態の指定・ユーザ設定値（config.json） |

## 受賞

- **2025年 SPRESENSE™ 活用コンテスト ルーキー賞** ([受賞一覧](https://elchika.com/promotion/spresense2025/winner/#nav))  
  受賞記事: [elchika - SPRESENSE CanSat車輪ローバ](https://elchika.com/article/d951b791-76ad-4697-b674-6267119dfe52/)

- **ARLISS 2025 Best Mission Award 2位 / Technical System Award 3位** ([結果発表](https://readyfor.jp/projects/unisec-arliss2025/announcements/400850))