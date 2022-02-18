using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Net.Http;
using System.Windows.Forms;
using NAudio.CoreAudioApi;
using NAudio.CoreAudioApi.Interfaces;

namespace agent
{
    public partial class Main : Form
    {
        /// <summary>
        /// HTTP クライアント
        /// </summary>
        private static readonly HttpClient httpClient = new HttpClient();

        /// <summary>
        /// サーバーに状態変更を要求するための実行キュー
        /// </summary>
        public Queue<bool> BusyRequestQueue = new Queue<bool>();

        /// <summary>
        /// イベント捕捉用マイクデバイスオブジェクト
        /// </summary>
        private MMDevice microphoneDevice;

        /// <summary>
        /// マイクデバイスが使用中かどうか
        /// </summary>
        private bool UsedMicrophone
        {
            get
            {
                var device = new MMDeviceEnumerator().GetDefaultAudioEndpoint(DataFlow.Capture, Role.Multimedia);
                var state = Enumerable.Range(0, device.AudioSessionManager.Sessions.Count)
                    .ToList()
                    .Any(i => device.AudioSessionManager.Sessions[i].State == AudioSessionState.AudioSessionStateActive);
                this.lastMicrophoneState = state;
                return state;
            }
        }

        /// <summary>
        /// 前回のマイクデバイス使用状態
        /// </summary>
        private bool? lastMicrophoneState;

        /// <summary>
        /// 現在開いているダイアログ
        /// </summary>
        private Form openingDialog;

        /// <summary>
        /// コンストラクター
        /// </summary>
        public Main()
        {
            this.InitializeComponent();
            this.initializeMicrophoneDevice();
            this.startTimer();
            Debug.WriteLine($"マイクの使用状態: {(this.lastMicrophoneState.HasValue ? (this.lastMicrophoneState.Value ? "使用中" : "未使用") : "---")}");
        }

        /// <summary>
        /// 一定時間ごとにキューから取り出してサーバーに状態変更要求を行います。
        /// </summary>
        private void onTimer(object sender, EventArgs e)
        {
            if (BusyRequestQueue.Count > 0)
            {
                BusyRequestQueue.Dequeue();
                this.sendChangeState();
            }
        }

        /// <summary>
        /// 設定画面を開きます。
        /// </summary>
        private void notifyIconDoubleClicked(object sender, EventArgs e)
        {
            this.openSettingsDialog();
        }

        /// <summary>
        /// 設定画面を開きます。
        /// </summary>
        private void openSettingsMenuItemClicked(object sender, EventArgs e)
        {
            this.openSettingsDialog();
        }

        /// <summary>
        /// 強制的に状態変更を要求します。
        /// </summary>
        private void forceChangeMenuItemClicked(object sender, EventArgs e)
        {
            this.sendChangeState();
        }

        /// <summary>
        /// アプリケーションを終了します。
        /// </summary>
        private void exitApplicationMenuItemClicked(object sender, EventArgs e)
        {
            Application.Exit();
        }

        /// <summary>
        /// サーバーへの状態変更要求を開始します。
        /// </summary>
        private void startTimer()
        {
            this.timer.Enabled = !string.IsNullOrEmpty(Properties.Settings.Default.ManagerEndpointUri);

            if (!this.timer.Enabled)
            {
                this.updateNotifyText(null);
                MessageBox.Show("設定に不備があるため、通話中インジケーターは動作を停止しています。", Properties.Resources.AppName, MessageBoxButtons.OK, MessageBoxIcon.Exclamation);
            }
            else
            {
                this.updateNotifyText(this.UsedMicrophone);
            }
        }

        /// <summary>
        /// マイクデバイス使用状況の監視を始めます。
        /// </summary>
        private void initializeMicrophoneDevice()
        {
            this.microphoneDevice = new MMDeviceEnumerator().GetDefaultAudioEndpoint(DataFlow.Capture, Role.Multimedia);

            var handler = new CustomAudioSessionEventsHandler(() =>
            {
                var lastState = this.lastMicrophoneState;
                var state = this.UsedMicrophone;
                if (!lastState.HasValue || lastState.Value != state)
                {
                    Debug.WriteLine($"マイクの使用状態が変更されました: {(lastState.HasValue ? (lastState.Value ? "使用中" : "未使用") : "---")} -> {(state ? "使用中" : "未使用")}");
                    BusyRequestQueue.Enqueue(state);
                }
            });

            Enumerable.Range(0, microphoneDevice.AudioSessionManager.Sessions.Count)
                .ToList()
                .ForEach(i => microphoneDevice.AudioSessionManager.Sessions[i].RegisterEventClient(handler));
            microphoneDevice.AudioSessionManager.OnSessionCreated += (sender, e)
                => e.RegisterAudioSessionNotification(new AudioSessionEventsCallback(handler));
        }

        /// <summary>
        /// 設定画面を開きます。
        /// </summary>
        private void openSettingsDialog()
        {
            if (this.openingDialog?.Visible ?? false)
            {
                this.openingDialog.Activate();
                return;
            }

            this.openingDialog = new Settings();
            if (this.openingDialog.ShowDialog() == DialogResult.OK)
            {
                this.startTimer();
            }
        }

        /// <summary>
        /// サーバーに状態変更を要求します。
        /// </summary>
        private async void sendChangeState()
        {
            try
            {
                var request = new HttpRequestMessage(HttpMethod.Get, Properties.Settings.Default.ManagerEndpointUri);
                var response = await httpClient.SendAsync(request);
                this.updateNotifyText(this.UsedMicrophone);
                Debug.WriteLine($"送信結果: {(int) response.StatusCode} ({response.StatusCode})");
            }
            catch (Exception ex)
            {
                this.updateNotifyText(null);
                Debug.WriteLine($"送信結果: {ex.Message}");
                Debug.WriteLine(ex.StackTrace);
            }
        }

        /// <summary>
        /// タスクトレイアイコンのテキストを更新します。
        /// </summary>
        private void updateNotifyText(bool? state)
        {
            if (state.HasValue)
            {
                this.notifyIcon.Text = $"{Properties.Resources.AppName}: マイク{(state.Value ? "使用中" : "未使用")}";
            }
            else
            {
                this.notifyIcon.Text = $"{Properties.Resources.AppName}: エラー";
            }
        }
    }
}
