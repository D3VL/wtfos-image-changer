# wtfos - Image Changer
Sets the screensaver and splash screen to wtfos images

## Want to change the images? 

### The Easy way
Install `image-configurator` from the fpv.wtf repository and follow the instructions there.

### The Manual way

Before you begin, you'll need FFMPEG installed on your pc, a microsd card, and some images to use.<br>
Make sure the images are 16:9 or be prepared to see some stretchy horror! 

```bash
# Generate the splash screen
ffmpeg -i "/path/to/your/input.image" -vf "scale=1440:810,pad=w=1536:h=810:x=0:y=0:color=black" -pix_fmt nv12 splashscreen.yuv
```

```bash
# Generate the screensavers
ffmpeg -i "/path/to/your/input.image" -vf "scale=1440:810" -pix_fmt bgra "screensaver.rgb"
```

After generating the Screensaver files, you'll need to rename them to "screensaver01.data" and "screensaver02.data".

Pop all 3 files into the root of your SD card, insert it into the goggles, reboot and see your marvelous images in action!