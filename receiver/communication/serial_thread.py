import threading
import serial
import re
from views import text_info, motor, map, altitude

serial_port = '/dev/ttyUSB0'  # あなたのシリアルポートに合わせて変更してください
baud_rate = 115200

tlm_serial_data = None
tlm_state = None
tlm_count = []
tlm_altitude = []
tlm_motor = None
tlm_lat = []
tlm_lng = []
data_to_display = ""

# 保持するカウンタの数
max_data_count = 20
# 保持する高度データの数
max_data_altitude = 20

# 新しいメッセージフォーマット: 日付,state,緯度,経度,高度,右モーター,左モーター
DATETIME  = 0
STATE     = 1
LATITUDE  = 2
LONGITUDE = 3
ALTITUDE  = 4
MOTOR_R   = 5  # 右モーター (mr_pwm)
MOTOR_L   = 6  # 左モーター (ml_pwm)

def extract_csv_data(raw_data):
    """
    受信した複雑なデータからCSV部分を抽出する
    フォーマット例: ;U;04167;120;0x810A5ABB;168;038;2025/07/21 15:07:07Z,5,35.722344,139.895350,10.47,0,0;66;
    """
    # 日付時刻で始まるCSVパターンを検索
    # パターン: YYYY/MM/DD HH:MM:SSZ,数値,数値,数値,数値,数値,数値
    csv_pattern = r'(\d{4}/\d{2}/\d{2} \d{2}:\d{2}:\d{2}Z,[-+]?\d+,[-+]?\d*\.?\d+,[-+]?\d*\.?\d+,[-+]?\d*\.?\d+,[-+]?\d+,[-+]?\d+)'
    
    match = re.search(csv_pattern, raw_data)
    if match:
        return match.group(1)
    
    return None

def parse_csv_data(csv_string):
    """
    CSVデータを解析して各値を返す
    """
    try:
        parts = csv_string.split(',')
        if len(parts) >= 7:  # 7つの値を期待
            # タイムスタンプの処理
            datetime_str = parts[DATETIME]  # "2025/07/21 15:07:07Z"
            
            # 簡単のため、現在時刻のミリ秒を使用（カウンタとして）
            import time
            count_value = int(time.time() * 1000)
            
            state = int(parts[STATE])
            latitude = float(parts[LATITUDE])
            longitude = float(parts[LONGITUDE])
            altitude = float(parts[ALTITUDE])
            motor_r = int(parts[MOTOR_R])
            motor_l = int(parts[MOTOR_L])
            
            return datetime_str, state, latitude, longitude, altitude, motor_r, motor_l, count_value
    except (ValueError, IndexError) as e:
        print(f"CSV解析エラー: {e}, データ: {csv_string}")
        return None
    
    return None

def _serial_thread():
    global tlm_serial_data
    global tlm_state
    global tlm_count
    global tlm_altitude
    global tlm_motor
    global tlm_lat
    global tlm_lng
    
    with serial.Serial(serial_port, baud_rate, timeout=1) as ser:
        while True:
            try:
                tlm_serial_data = ser.readline().decode('utf-8').strip()
                
                # 空のデータや短すぎるデータは無視
                if not tlm_serial_data or len(tlm_serial_data) < 10:
                    continue
                
                # デバッグ用：受信データを表示
                print(f"受信データ: {tlm_serial_data}")
                
                # CSVデータを抽出
                csv_data = extract_csv_data(tlm_serial_data)
                if csv_data is None:
                    print("CSVデータが見つかりません")
                    continue
                
                print(f"抽出されたCSV: {csv_data}")
                
                # CSVデータを解析
                parsed_data = parse_csv_data(csv_data)
                if parsed_data is None:
                    continue
                
                datetime_str, state, lat, lng, alt, mr_pwm, ml_pwm, count_value = parsed_data
                
                # データを格納
                tlm_state = state
                
                # カウントデータ（時系列用）
                tlm_count.append(count_value)
                if len(tlm_count) > max_data_count:
                    tlm_count.pop(0)  # 古いデータを削除する
                
                # 高度データを追加
                tlm_altitude.append(alt)
                if len(tlm_altitude) > max_data_altitude:
                    tlm_altitude.pop(0)  # 古いデータを削除する
                
                # モーターデータ（右、左の順）
                tlm_motor = [mr_pwm, ml_pwm]
                
                # 位置データ
                tlm_lat = [lat]
                tlm_lng = [lng]
                
                print(f"解析完了 - 日時:{datetime_str}, 状態:{state}, 位置:({lat},{lng}), 高度:{alt}, モーター:({mr_pwm},{ml_pwm})")
                
            except Exception as e:
                print(f"シリアルデータ処理エラー: {e}")
                pass

def start_thread(app_instance):
    thread = threading.Thread(target=_serial_thread)
    thread.daemon = True
    thread.start()
    # アプリケーションインスタンスをスレッド内で使用する場合
    # 以下のようにしてグローバル変数として格納することも可能
    # global app
    # app = app_instance

def update_text_info(n):
    return text_info.update_text_info(tlm_state, tlm_lat, tlm_lng)

def update_graph_motor(n):
    return motor.update_graph_motor(tlm_motor)

def update_map(n):
    return map.update_map(tlm_lat, tlm_lng)

def update_graph_altitude(n):
    return altitude.update_graph_altitude(tlm_altitude, tlm_count)

def update_serial_console(n):
    global tlm_serial_data
    global data_to_display
    if tlm_serial_data is not None:
        data_to_display += tlm_serial_data + "\n"
        tlm_serial_data = ""  # 受信データをクリア
        return data_to_display
    else:
        return ""