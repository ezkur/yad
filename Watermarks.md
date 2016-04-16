# Introduction #

This is a nautilus script for put watermarks on images selected in Nautilus

Author: Julio C. Neves <julioneves@openoffice.org>

Additional software:
  * ImageMagick (http://www.imagemagick.org/)

# Details #

```
#!/bin/bash
#
#  Put watermark on images selected in Nautilus.
#+ Dependencies:
#+		- yad
#+		- ImageMagick

Geometry='Top Left!Top Right!Bottom Left!Bottom Right!Top!Bottom!Left!Right!Center!Tiled'

# get value of decimal point in current locale
eval $(locale -ck LC_NUMERIC | grep decimal_point)

while :
do
#### Now the magic of YAD ####
	Choices=$(yad --title "Put a watermark" --form      \
	    --width 500 --dialog-sep --image xfce4-keyboard \
    	--field "Watermark file:FL"                     \
	    --field "Watermark position:CB"                 \
	    --field "% tranparency:NUM"                     \
	    --button gtk-ok:0 --button gtk-cancel:1         \
	    "" "$Geometry" '20!0..100!5') || {
	    yad --text "Abnormal end" --button gtk-ok
	    exit 1
	}
	IFS='|'
	read File Position Transparency <<< "$Choices"
	Transparency=${Transparency%$decimal_point*}
	[ -f "$File" ] && break
	yad --text "No watermark file         " \
	    --image dialog-error                \
	    --button gtk-ok:0
done

Position=$(case "$Position" in
        ("Top Left")		echo northwest	;;
        ("Top Right")		echo northeast	;;
        ("Bottom Left")		echo southwest	;;
        ("Bottom Right")	echo southeast	;;
        (Top)			echo north	;;
        (Bottom)		echo south	;;
        (Left)			echo west	;;
        (Right) 		echo east	;;
        (Center)		echo center	;;
        (*)			echo tile	;;
esac)	# Bash 4.0++ dont need to open parentesys
Position=$([[ $Position == tile ]] && echo -tile || echo -gravity $Position)
IFS='
'		# Because the name of some files has spaces

i=0
TotArqs=$(echo "$NAUTILUS_SCRIPT_SELECTED_FILE_PATHS" | wc -l) 
#  In the next for a numeric echo moves the 
#+ progress bar and an echo beginning with a # 
#+ changes the header.
#+ Now the big trick ImageMagick + nautilus + YAD
for Arq in $NAUTILUS_SCRIPT_SELECTED_FILE_PATHS
do 
    #  Os 2 echo a seguir são para atualizar o progress
    echo $((++i * 100 / $TotArqs))
    echo "# Putting a watermark on ${Arq##*/}"
    Arq=$(sed 's/ /\\ /g' <<< "$Arq") # Escape blanks (because of eval)
    eval composite -watermark $Transparency $Position "$File" "$Arq" "$Arq"
	sleep 1
done | yad --progress --title "Wait, ImageMagick working..." --text ' ' --width 500 --auto-close --auto-kill
```