NAME = $(shell cat ./control/control | grep Package | cut -d" " -f2)
ARCH = $(shell cat ./control/control | grep Architecture | cut -d" " -f2)
VERSION = $(shell cat ./control/control | grep Version | cut -d" " -f2)
IPK_NAME = "${NAME}_${VERSION}_${ARCH}.ipk"


jni:
	cd ./jni && ndk-build
	cp ./jni/libs/armeabi-v7a/draw_to_splash ./data/opt/bin
	chmod +x ./data/opt/bin/draw_to_splash

render:
	ffmpeg -i "./images/splashscreen.png" -vf "scale=1440:810,pad=w=1536:h=810:x=0:y=0:color=black" -pix_fmt nv12 "./images/splashscreen.yuv"
	ffmpeg -i "./images/screensaver01.png" -vf "scale=1440:810" -pix_fmt bgra "./images/screensaver01.rgb"
	ffmpeg -i "./images/screensaver02.png" -vf "scale=1440:810" -pix_fmt bgra "./images/screensaver02.rgb"
	mv ./images/splashscreen.yuv ./data/opt/share/image-changer/splashscreen.yuv
	mv ./images/screensaver01.rgb ./data/opt/share/image-changer/screensaver01.data
	mv ./images/screensaver02.rgb ./data/opt/share/image-changer/screensaver02.data

all: jni render

	chmod +x ./control/postinst
	chmod +x ./control/prerm
	chmod +x ./data/opt/bin/image-changer.sh

	mkdir -p ipk
	echo "2.0" > ipk/debian-binary
	cp -r data ipk/
	cp -r control ipk/
	cd ipk/control && tar --owner=0 --group=0 -czvf ../control.tar.gz .
	cd ipk/data && tar --owner=0 --group=0 -czvf ../data.tar.gz .
	cd ipk/ && tar --owner=0 --group=0 -czvf "./${IPK_NAME}" ./control.tar.gz ./data.tar.gz ./debian-binary

clean:
	rm -rf ipk
