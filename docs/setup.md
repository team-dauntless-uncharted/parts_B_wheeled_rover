# セットアップと実行

**※ WindowsはPowerShellで作業することを前提としているので注意**

## 0. 事前準備

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

## 1. 環境構築

このリポジトリをクローンする

```bash
$ git clone https://github.com/team-dauntless-uncharted/parts_B_wheeled_rover.git
$ git clone git@github.com:team-dauntless-uncharted/parts_B_wheeled_rover.git
```

`parts_B_wheeled_rover`直下に移動

```bash
$ cd parts_B_wheeled_rover
```

submoduleを取得

```bash
$ git submodule update --init --recursive
```

## 2. ソースコードのビルド

**macOS・Linuxの場合**

parts_B_wheeled_roverディレクトリ直下でmakeコマンドを使用する

```bash
$ make compile
```

**Windowsの場合**

parts_B_wheeled_roverディレクトリ直下でbuild.batを実行する

```
$ ./build.bat compile
```

## 3. 書き込み

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

**build.batがうまく行かない場合**

下記のコマンドを直接実行する

```bash
# ビルド
$ arduino-cli compile --fqbn SPRESENSE:spresense:spresense:Core=Main,Memory=1536 --libraries libraries --build-property build.extra_flags="-Isrc" --build-property compiler.cpp.extra_flags="-std=gnu++14" parts_B_wheeled_rover.ino
# 書き込み
$ arduino-cli upload --fqbn SPRESENSE:spresense:spresense:Core=Main,Memory=1536 -p [Spresenseを認識しているポート] parts_B_wheeled_rover.ino
# シリアルモニタ
$ arduino-cli monitor -p [Spresenseを認識しているポート] -c baudrate=115200
```

## 4. シリアルモニタ

デバイスはUSBで接続して、認識していればそのデバイスのシリアルモニタを開く

**macOS・Linuxの場合**

```bash
$ make monitor
```

**Windowsの場合**

```bash
$ ./build.bat monitor
```
