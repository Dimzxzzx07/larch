#![no_std]

pub struct MixerChannel {
    volume: u8,
    muted: bool,
    left_gain: u8,
    right_gain: u8,
}

impl MixerChannel {
    pub const fn new() -> Self {
        MixerChannel {
            volume: 100,
            muted: false,
            left_gain: 100,
            right_gain: 100,
        }
    }
    
    pub fn set_volume(&mut self, volume: u8) {
        self.volume = if volume > 100 { 100 } else { volume };
    }
    
    pub fn set_mute(&mut self, muted: bool) {
        self.muted = muted;
    }
    
    pub fn get_volume(&self) -> u8 {
        if self.muted { 0 } else { self.volume }
    }
    
    pub fn set_gain(&mut self, left: u8, right: u8) {
        self.left_gain = if left > 100 { 100 } else { left };
        self.right_gain = if right > 100 { 100 } else { right };
    }
}

pub struct AudioMixer {
    master: MixerChannel,
    channels: [MixerChannel; 16],
    sample_rate: u32,
}

impl AudioMixer {
    pub const fn new() -> Self {
        AudioMixer {
            master: MixerChannel::new(),
            channels: [MixerChannel::new(); 16],
            sample_rate: 44100,
        }
    }
    
    pub fn mix_samples(&self, input: &[i16], output: &mut [i16], channel: usize) {
        let master_vol = self.master.get_volume() as i16;
        let channel_vol = self.channels[channel].get_volume() as i16;
        let gain = (master_vol * channel_vol / 100) as i16;
        
        for i in 0..input.len() / 2 {
            output[i * 2] += (input[i * 2] * gain) / 100;
            output[i * 2 + 1] += (input[i * 2 + 1] * gain) / 100;
        }
    }
    
    pub fn set_sample_rate(&mut self, rate: u32) {
        self.sample_rate = rate;
    }
    
    pub fn sample_rate(&self) -> u32 {
        self.sample_rate
    }
}