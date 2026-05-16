# 設定

## 状態の指定

`state.txt`に対応する数値を記述することで、その状態から始めることができる。また、各状態の`onExit()`で最新の状態に書き換える

| 数値 | 状態 |
|------|------|
| 0 | CalibrationState |
| 1 | StandbyState |
| 2 | LaunchState |
| 3 | DropState |
| 4 | EscapeState |
| 5 | DetectionState |
| 6 | RecordingState |
| 7 | ExploreState |

## ユーザ指定の値

SDカードに`config.json`ファイルを配置することで、任意の値を設定することができる

| キー | 説明 |
|------|------|
| Calibration.Timeout | CalibrationStateでのタイムアウト時間 |
| Standby.Timeout | StandbyStateでのタイムアウト時間 |
| Launch.CdS | LaunchStateでのDropState移行のためのCdSの値の閾値 |
| Launch.Timeout | LaunchStateでのタイムアウト時間 |
| Drop.Timeout | DropStateでのタイムアウト時間 |
| Escape.Distance | EscapeStateでの移行のための落下地点と現在地点の距離 |
| Escape.Timeout | EscapeStateでのタイムアウト時間 |
| Detection.MaxFailedCount | DetectionStateでの次の状態に移行するまでの物体認識の失敗回数 |
| Recording.Timeout | RecordingStateでのタイムアウト時間 |
| Recording.Time | RecordingStateでの録画時間 |

```json
{
  "Calibration": {
    "Timeout": 300000
  },
  "Standby": {
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

ファイルがない場合、ファイルに上記のキーと値が存在しない場合は、`CansatController.hpp`の`UserConfig`構造体内で指定しているデフォルト値が使用される
