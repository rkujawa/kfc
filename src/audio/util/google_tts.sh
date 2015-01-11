for i in `printf '%s\n' {0..9}` ; do wget -U "Lynx 1.2.3.4" -O $i.mp3 'http://translate.google.com/translate_tts?tl=pl&q='$i; done
