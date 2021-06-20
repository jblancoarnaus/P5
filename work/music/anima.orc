# 2: synth keyboard ~ soft bell (lead)
2	SenoFM    max_level=0.11825; I2=1;N1=1;N2=12;setting=0.999875;   #exp envelope setting

# 3: synth woodwind
3	SenoFM	ADSR_A=0.02; ADSR_D=0.05; ADSR_S=0.3; ADSR_R=0.55; ADSR_S2=1; max_level=0.085; N=40; I1=0; I2=2; N2=3; N1=1; setting=0; setting2=-1;

# 4: santoor (piano-ish lead around 1:12)
4	SenoFM	max_level=0.155;I2=3.75;N1=4;N2=12;setting=0.99983; file=work/music/tables/piano_1cycle.wav;

# 5: synth organ (table synthesis)
5	Strings	ADSR_A=0.02; ADSR_S=0.1875; ADSR_R=0.5; file=work/music/tables/synth_bass_1cycle.wav;

# 6: synth organ 2
6	SenoFM	ADSR_A=0.15; ADSR_S=0.1105; ADSR_S2=0.4; ADSR_R=0.35;  N=40; I1=0; I2=2; N2=3; N1=1; setting=-1;

# 7: synth brass
7	SenoFM	ADSR_A=0.1; ADSR_D=0.2; ADSR_S=0.8;ADSR_R=0.2;ADSR_A2=0.12;ADSR_S2=0.25; ADSR_R2=0.2; max_level=0.03075; N=40; I1=0; I2=5; N2=1; N1=1; setting=0;

# 8: synth flute
8	SenoFM	ADSR_A=0.05; ADSR_D=0.025; ADSR_S=0.7;ADSR_R=0.15;ADSR_A2=0.12;ADSR_S2=0.25; ADSR_R2=0.2; max_level=0.0545; N=40;I1=0.5; I2=0; N2=1; N1=1; setting=0;

# 9: drum kick
9	PercussionSample    file=work/music/tables/snare_drum_kick_sample.wav; volume=0.119;

# 10: far-away bell
10	SenoFM    max_level=0.0605; I1=0;I2=9;N1=1;N2=1.4;setting=0.999972;   #exp envelope setting

# 11: drum kick 2
11	PercussionSample    file=work/music/tables/drum_kick_sample.wav; volume=0.06825;

# 12: drum cymbal
12	PercussionSample    file=work/music/tables/cymbal_sample.wav; volume=0.1354;

# 13: rising noise
13	PercussionSample    file=work/music/tables/rising_noise_sample.wav; volume=0.3475; interrupt=0.999945;
