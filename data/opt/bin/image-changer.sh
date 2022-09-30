#!/system/bin/sh

echo "[image-changer] start";

function checkImageSize {
    SIZE=$(wc -c "$1" | awk '{print $1}')
    if [[ $SIZE -ne $2 ]]; then
        echo "[image-changer] $1 is equal to $2";
        mv "$1" "$1.wrongsize"
        return 1
    fi
    return 0
}

# Check the SD card for splashscreen.yuv; if it exists then copy it to /data/splashscreen.yuv
SD_CARD=$(ls /mnt/media_rw);
if [[ SD_CARD ]]; then 
    echo "[image-changer] SD card found: $SD_CARD"; 

    if [[ -f "/mnt/media_rw/$SD_CARD/screensaver01.data" ]]; then
        echo "[image-changer] screensaver01.data found on SD card";

        checkImageSize "/mnt/media_rw/$SD_CARD/screensaver01.data" 4665600 &&
        cp "/mnt/media_rw/$SD_CARD/screensaver01.data" /data/screensaver01.data;
    fi

    if [[ -f "/mnt/media_rw/$SD_CARD/screensaver02.data" ]]; then
        echo "[image-changer] screensaver02.data found on SD card";

        checkImageSize "/mnt/media_rw/$SD_CARD/screensaver02.data" 4665600 &&
        cp "/mnt/media_rw/$SD_CARD/screensaver02.data" /data/screensaver02.data;
    fi

    if [[ -f "/mnt/media_rw/$SD_CARD/splashscreen.yuv" ]]; then
        echo "[image-changer] splashscreen.yuv found on SD card";

        checkImageSize "/mnt/media_rw/$SD_CARD/splashscreen.yuv" 1866240 &&
        cp "/mnt/media_rw/$SD_CARD/splashscreen.yuv" /data/splashscreen.yuv;
    fi

    echo "[image-changer] SD card functions complete";
fi


echo "[image-changer] searching for screensavers";
if [[ -f "/data/screensaver01.data" ]]; then
    echo "[image-changer] screensaver01.data found";
    draw_to_screensaver /system/gui/screen01.data
else
    draw_to_screensaver /system/gui/screen01.data
fi

if [[ -f "/data/screensaver02.data" ]]; then
    echo "[image-changer] screensaver02.data found";
    draw_to_screensaver /system/gui/screen02.data
else
    draw_to_screensaver /system/gui/screen02.data
fi


echo "[image-changer] searching for splashscreen";
if [[ -f /data/splashscreen.yuv ]]; then
    echo "[image-changer] found custom splashscreen";
    draw_to_splash /data/splashscreen.yuv > /dev/null
else
    echo "[image-changer] no custom splashscreen found";
    draw_to_splash /opt/share/image-changer/splashscreen.yuv > /dev/null
fi
