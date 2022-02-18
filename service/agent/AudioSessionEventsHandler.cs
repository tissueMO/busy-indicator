using System;
using NAudio.CoreAudioApi.Interfaces;

namespace agent
{
    /// <summary>
    /// カスタムオーディオセッションイベントハンドラー
    /// </summary>
    public class CustomAudioSessionEventsHandler : IAudioSessionEventsHandler
    {
        private readonly Action changedCallback;

        /// <summary>
        /// コンストラクター
        /// </summary>
        /// <param name="changedCallback">セッションの状態が変わった時に呼び出されるコールバック関数</param>
        public CustomAudioSessionEventsHandler(Action changedCallback)
        {
            this.changedCallback = changedCallback;
        }

        public void OnChannelVolumeChanged(uint channelCount, IntPtr newVolumes, uint channelIndex)
        {
        }

        public void OnDisplayNameChanged(string displayName)
        {
        }

        public void OnGroupingParamChanged(ref Guid groupingId)
        {
        }

        public void OnIconPathChanged(string iconPath)
        {
        }

        public void OnSessionDisconnected(AudioSessionDisconnectReason disconnectReason)
        {
        }

        public void OnStateChanged(AudioSessionState state)
        {
            this.changedCallback?.Invoke();
        }

        public void OnVolumeChanged(float volume, bool isMuted)
        {
        }
    }
}