##########################################################################################
#   永続的に状態管理を行うAPIです。
##########################################################################################
import sqlite3
import json
import requests
from datetime import datetime as dt

# 定数定義
DATETIME_FORMAT = "%Y-%m-%d %H:%M:%S"

# 設定値ロード
import configparser
config = configparser.ConfigParser()
config.read("./config.ini", "UTF-8")

DBPATH = config.get("status", "path")
DBTABLE = config.get("status", "table")
ID_LIST = json.loads(config.get("status", "ids"))
ENABLED_IFTTT = config.has_option("ifttt", "enabled") \
    and config.getboolean("ifttt", "enabled")
IFTTT_KEY = config.get("ifttt", "key") if ENABLED_IFTTT else None
SECONDARY_EVENT_MAP = json.loads(config.get("ifttt", "secondary_event_map"))


def _get_connection() -> sqlite3.Connection:
    """SQLコネクションを返します。

    Returns:
        sqlite3.Connection: コネクションオブジェクト
    """
    return sqlite3.connect(DBPATH)


def _turn_ifttt(device_id: int, state: bool) -> None:
    """IFTTTのWebhookレシピを実行します。

    Args:
        device_id (int): デバイスID
        state (bool): 更新後の状態
    """
    if not ENABLED_IFTTT:
        return

    device_id = int(device_id)
    device_number = ID_LIST.index(device_id) + 1

    url = f"https://maker.ifttt.com/trigger/switchbot_onair_{device_number}_"
    if state:
        url += "on"
    else:
        url += "off"
    url += f"/with/key/{IFTTT_KEY}"

    requests.post(
        url,
        data={},
        headers={"Content-Type": "application/json"}
    ) \
        .raise_for_status()

    # セカンダリーイベント実行
    if str(device_id) in SECONDARY_EVENT_MAP:
        url = f"https://maker.ifttt.com/trigger/{SECONDARY_EVENT_MAP[str(device_id)]}/with/key/{IFTTT_KEY}"

        requests.post(
            url,
            data={},
            headers={"Content-Type": "application/json"}
        ) \
            .raise_for_status()

def reset():
    """データベースを初期化します。
    """
    conn = _get_connection()
    cursor = conn.cursor()
    cursor.execute(f"DROP TABLE IF EXISTS {DBTABLE}")
    cursor.execute(
        f"CREATE TABLE IF NOT EXISTS {DBTABLE} (id INTEGER PRIMARY KEY, value int, updated TEXT)")

    for i, id in enumerate(ID_LIST):
        cursor.execute(
            f"INSERT INTO {DBTABLE} VALUES (?, ?, ?)",
            (id, 0, dt.now().strftime(DATETIME_FORMAT))
        )

        # IFTTTレシピ実行
        _turn_ifttt(id, False)

    conn.commit()
    return {}


def turn(request):
    """フラグを切り替えます。

    Arguments:
        request: HTTPリクエストデータ
    """
    current_status = get(request)
    next_status = 0 if current_status != "0" else 1

    conn = _get_connection()
    cursor = conn.cursor()
    cursor.execute(
        f"UPDATE {DBTABLE} SET value = ?, updated = ? WHERE id = ?",
        (next_status, dt.now().strftime(DATETIME_FORMAT), request["id"])
    )
    conn.commit()

    # IFTTTレシピ実行
    _turn_ifttt(request["id"], next_status)

    return {}


def get(request):
    """ステータスを確認します。

    Arguments:
        request: HTTPリクエストデータ
    Return:
        int: ステータス値
    """
    conn = _get_connection()
    cursor = conn.cursor()

    if "id" in request:
        # 指定されたレコードの状態を返す
        cursor.execute(
            f"SELECT value FROM {DBTABLE} WHERE id = ?",
            (request["id"],)
        )
        result = cursor.fetchone()

        return "-1" if result is None else str(result[0])

    else:
        # すべてのレコードの状態を1文字ずつ区切って返す
        result = cursor.execute(
            f"SELECT value FROM {DBTABLE} ORDER BY id ASC",
        )

        output = ""
        for state in result:
            output += str(state[0])

        return output


if __name__ == "__main__":
    reset()
