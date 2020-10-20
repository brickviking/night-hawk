#
#sox 6xx_voice.wav -t raw -e unsigned -b 8 6xx_voice.8 rate -v 11025
#play -t raw -e unsigned -b 8 -c 1 -r 11025 6xx_voice.8
sox 6xx_disarm_and_disengage.wav -t raw -e unsigned -b 8			6xx_voice.8 rate -v 11025
sox 7xx_halt_paranoid_3.wav -t raw -e unsigned -b 8				7xx_voice.8 rate -v 11025
sox 8xx_alert_alert_alert_paradroid_detected.wav -t raw -e unsigned -b 8	8xx_voice.8 rate -v 11025
sox 9xx_cmdr_under_attack_request_assistance2.wav -t raw -e unsigned -b 8	9xx_voice.8 rate -v 11025
