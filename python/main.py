# Uno Q Robot Car -- main.py
#
# SPDX-FileCopyrightText: Copyright (C) Arduino s.r.l. and/or its affiliated companies
#
# SPDX-License-Identifier: MPL-2.0

import time
import threading
from arduino.app_utils import App, Bridge
from arduino.app_bricks.web_ui import WebUI
from arduino.app_bricks.video_objectdetection import VideoObjectDetection
from datetime import datetime, UTC

ui = WebUI()
bridge = Bridge()
detection_stream = VideoObjectDetection(confidence=0.5, debounce_sec=0.0)

# モーター状態（0: 停止, 1: 前進）
current_state = 0
last_detection_time = 0.0
TIMEOUT_SEC = 0.5  # 0.5秒間検知が途切れたら停止とみなす

try:
    bridge.call("motor_state", current_state, timeout=5)
except Exception as e:
    print(f"Initial bridge call failed: {e}")

def update_motor(should_move: int):
    global current_state
    new_state = 1 if should_move > 0 else 0
    
    if new_state != current_state:
        current_state = new_state
        try:
            bridge.call("motor_state", new_state, timeout=5)
        except Exception as e:
            print(f"Bridge call error: {e}")

# 検知途絶を監視するバックグラウンドスレッド
def watchdog_loop():
    global last_detection_time, current_state
    while True:
        # 現在走行中かつ、最後の検知からタイムアウト時間を経過していたら停止
        if current_state == 1 and (time.time() - last_detection_time > TIMEOUT_SEC):
            update_motor(0)
        time.sleep(0.05)

# ウォッチドッグスレッドをデーモンとして起動
threading.Thread(target=watchdog_loop, daemon=True).start()

ui.on_message("override_th", lambda sid, threshold: detection_stream.override_threshold(threshold))

def send_detections_to_ui(detections: dict):
    global last_detection_time
    total_detected = sum(len(items) for items in detections.values())
    
    if total_detected > 0:
        last_detection_time = time.time()
        update_motor(1)

    # UI送信処理
    for key, values in detections.items():
        for value in values:
            entry = {
                "content": key,
                "confidence": value.get("confidence"),
                "timestamp": datetime.now(UTC).isoformat()
            }
            ui.send_message("detection", message=entry)

detection_stream.on_detect_all(send_detections_to_ui)

App.run()