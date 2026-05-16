# ビルドオプション

## Flash有効化

Flashは、簡単に行うのであれば、マイコンからしか読み書きできない、開発中などにむやみに書き込まんで寿命を縮めないようにするために、基本的にビルド時にオプションを付けないと使用しないようになっている

ビルドする際に `FEATURE_FLAG="-D USE_FLASH"` を付ける

```bash
$ make compile FEATURE_FLAG="-D USE_FLASH" # Stateの確認にFlashを使用する
```

> Flashをフォーマットしたい場合は、`tools/flash_format/flash_format.ino` を書き込み、シリアルモニタからyを送ることでできる

## GNSSの値取得待ちの有効化

Spresenseの内蔵GNSSは起動してからすぐに値を取得できないため、値を取得するまで先に進まない機能を有効にできる

ビルドする際に `FEATURE_FLAG="-D WAIT_GNSS_RECEIVE"` を付ける

```bash
$ make compile FEATURE_FLAG="-D WAIT_GNSS_RECEIVE"
```
