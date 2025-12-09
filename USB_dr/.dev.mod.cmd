savedcmd_/home/ubuntu/USB_dr/dev.mod := printf '%s\n'   dev.o | awk '!x[$$0]++ { print("/home/ubuntu/USB_dr/"$$0) }' > /home/ubuntu/USB_dr/dev.mod
