using System;
using System.Runtime.InteropServices;

namespace Larch.Sound
{
    public static class AudioAPI
    {
        [DllImport("kernel")]
        private static extern int syscall(int num, params object[] args);
        
        private const int SYS_AUDIO_PLAY = 100;
        private const int SYS_AUDIO_STOP = 101;
        private const int SYS_AUDIO_VOLUME = 102;
        
        public static void PlaySound(int frequency, int duration_ms)
        {
            syscall(SYS_AUDIO_PLAY, frequency, duration_ms);
        }
        
        public static void StopSound()
        {
            syscall(SYS_AUDIO_STOP);
        }
        
        public static void SetVolume(int volume_percent)
        {
            if (volume_percent < 0) volume_percent = 0;
            if (volume_percent > 100) volume_percent = 100;
            syscall(SYS_AUDIO_VOLUME, volume_percent);
        }
        
        public class AudioPlayer
        {
            private bool is_playing = false;
            
            public void PlayBeep()
            {
                PlaySound(440, 500);
                is_playing = true;
            }
            
            public void Stop()
            {
                StopSound();
                is_playing = false;
            }
        }
    }
}