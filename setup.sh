#!/bin/sh

DEST=$1

if [ "$DEST" == "" ]; then
	echo "You must provide a parameter!"
	exit -1
fi

cp mpd/mpdconf $DEST/.mpdconf

if [ ! -d "$DEST/.ncmpcpp" ]; then
	mkdir "$DEST/.ncmpcpp"
fi

cd dwm-6.0
make
if [ ! -d "$DEST/bin" ]; then
	mkdir "$DEST/bin"
fi
cp dwm $DEST/bin
cd ..

cp misc/xinitrc $DEST/.xinitrc

cp scripts/startdwm $DEST/bin/

cp vim/vimrc $DEST/.vimrc

cp screen/screenrc $DEST/.screenrc

cp bash/bashrc $DEST/.bashrc

cp -r ncmpc $DEST/.ncmpc

if [ ! -d "$DEST/.pentadactyl" ]; then
	mkdir -p "$DEST/.pentadactyl/"
fi
cp -r pentadactyl/colors $DEST/.pentadactyl/
cp pentadactyl/pentadactylrc $DEST/.pentadactylrc

if [ ! -d "$DEST/.vim/colors" ]; then
	mkdir -p "$DEST/.vim/colors"
fi
cp vim/molokai.vim $DEST/.vim/colors/

if [ ! -d "$DEST/.wall.jpg" ]; then
	wget https://presentingsweden.si.se/wp-content/uploads/sites/7/2013/07/palme_castro.jpg -O $DEST/.wall.jpg
fi

if [ ! -d "$DEST/.fonts" ]; then
	mkdir "$DEST/.fonts"
fi

wget http://www.bok.net/MonteCarlo/downloads/MonteCarlo-PCF.tgz -O $DEST/.fonts/MonteCarlo-PCF.tgz
tar zxfv $DEST/.fonts/MonteCarlo-PCF.tgz -C $DEST/.fonts/
mv $DEST/.fonts/MonteCarlo-PCF/* $DEST/.fonts/
rm -rf $DEST/.fonts/MonteCarlo-PCF $DEST/.fonts/MonteCarlo-PCF.tgz
mkfontdir $DEST/.fonts

cp xmodmap/Xmodmap $DEST/.Xmodmap
cp xbindkeys/xbindkeysrc $DEST/.xbindkeysrc
