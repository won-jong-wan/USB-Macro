savedcmd_/home/ubuntu/USB-Macro/USB_dr/usb_macro.mod := printf '%s\n'   usb_macro.o | awk '!x[$$0]++ { print("/home/ubuntu/USB-Macro/USB_dr/"$$0) }' > /home/ubuntu/USB-Macro/USB_dr/usb_macro.mod
