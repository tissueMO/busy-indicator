##########################################################################################
#   永続的に状態管理を行うAPIです。
##########################################################################################
import sqlite3
import json
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


def _get_connection() -> sqlite3.Connection:
    """SQLコネクションを返します。

    Returns:
        sqlite3.Connection: コネクションオブジェクト
    """
    return sqlite3.connect(DBPATH)


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
    cursor.execute(
        f"SELECT value FROM {DBTABLE} WHERE id = ?",
        (request["id"],)
    )
    result = cursor.fetchone()

    return "-1" if result is None else str(result[0])


if __name__ == "__main__":
    _reset()
