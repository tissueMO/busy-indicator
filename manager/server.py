##########################################################################################
#    内蔵アプリケーションサーバーを起動します。
##########################################################################################
from flask import Flask, request
import api.main as main

app = Flask(__name__)

@app.route("/turn", methods=["GET", "POST"])
def set():
    return main.turn(_get_request_json())

@app.route("/get", methods=["GET", "POST"])
def get_status():
    return main.get(_get_request_json())


def _get_request_json():
    """GET/POST両方に対応した形式でリクエストデータを変換します。

    Returns:
        list: リクエストデータ
    """
    if request.method == "POST":
        return request.json
    else:
        return request.args


if __name__ == "__main__":
    # DB初期化
    main.reset()

    # ローカルサーバー起動
    app.run(
        debug=False,
        host="0.0.0.0",
        port=8080
    )
