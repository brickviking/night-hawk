#
#sox 6xx_voice.wav -t raw -e unsigned -b 8 6xx_voice.8 rate -v 11025
#play -t raw -e unsigned -b 8 -c 1 -r 11025 6xx_voice.8
sox 6xx_voice.wav -t raw -e unsigned -b 8		6xx_voice.8 rate -v 11025
sox door_open.wav -t raw -e unsigned -b 8		door_open.8 rate -v 11025
sox droid_evase.wav -t raw -e unsigned -b 8		droid_evase.8 rate -v 11025
sox droid_hit2.wav -t raw -e unsigned -b 8		droid_hit2.8 rate -v 11025
sox floor_complete.wav -t raw -e unsigned -b 8		floor_complete.8 rate -v 11025
sox laser_crocoite_benzol.wav -t raw -e unsigned -b 8	laser_crocoite_benzol.8 rate -v 11025
sox laser_linarite.wav -t raw -e unsigned -b 8		laser_linarite.8 rate -v 11025
sox laser_tiger_eye.wav -t raw -e unsigned -b 8		laser_tiger_eye.8 rate -v 11025
sox laser_uvarovite.wav -t raw -e unsigned -b 8		laser_uvarovite.8 rate -v 11025
sox low_shields.wav -t raw -e unsigned -b 8		low_shields.8 rate -v 11025
sox no_weapon.wav -t raw -e unsigned -b 8		no_weapon.8 rate -v 11025
sox power_up.wav -t raw -e unsigned -b 8		power_up.8 rate -v 11025
sox ship_complete.wav -t raw -e unsigned -b 8		ship_complete.8 rate -v 11025
#sox trans_terminated.wav -t raw -e unsigned -b 8	trans_terminated.8 rate -v 11025
sox trans_terminated.wav -t raw -e unsigned -b 8	trans_terminated.8
