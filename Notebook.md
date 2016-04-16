# Introduction #

This script demostrate the usage of notebook dialog. Three tabs with different settings of gtk+-2.0


# Details #

```
#! /bin/sh
# -*- mode: sh -*-
#
# Author: Victor Ananjevsky <ananasik@gmail.com>, 2013
#

KEY="12345"

res1=$(mktemp --tmpdir iface1.XXXXXXXX)
res2=$(mktemp --tmpdir iface2.XXXXXXXX)
res3=$(mktemp --tmpdir iface3.XXXXXXXX)

rc_file="${XDG_CONFIG_HOME:-$HOME/.config}/gtk-2.0/gtkrc"

# parse rc file
PARSER='
BEGIN { FS="="; OFS="\n"; }
/^gtk-theme-name/ {printf "GTKTHEME=%s\n", $2}
/^gtk-key-theme-name/ {printf "KEYTHEME=%s\n", $2}
/^gtk-icon-theme-name/ {printf "ICONTHEME=%s\n", $2}
/^gtk-cursor-theme-name/ {printf "CURSORTHEME=%s\n", $2}
/^gtk-toolbar-style/ {printf "TBSTYLE=%s\n", $2}
/^gtk-toolbar-icon-size/ {printf "TBICONSIZE=%s\n", $2}
/^gtk-button-images/ {printf "BUTTONIMAGE=%s\n", ($2 == 1) ? "TRUE" : "FALSE"}
/^gtk-menu-images/ {printf "MENUIMAGE=%s\n", ($2 == 1) ? "TRUE" : "FALSE"}
/^gtk-font-name/ {printf "FONTDESCR=%s\n", $2}
/^gtk-xft-antialiasing/ {printf "ANTIALIASING=%s\n", ($2 == 1) ? "TRUE" : "FALSE"}
/^gtk-xft-hinting/ {printf "HINTING=%s\n", ($2 == 1) ? "TRUE" : "FALSE"}
/^gtk-xft-hintstyle/ {printf "HINTSTYLE=%s\n", $2}
/^gtk-xft-rgba/ {printf "RGBA=%s\n", $2}
/^gtk-color-scheme/ {printf "COLORSCHEME=%s\n", $2}
'
eval $(sed -r "s/[ \t]*=[ \t]*/=/" $rc_file | awk "$PARSER")

# create list of gtk themes
themelist=
keythemelist="Default"
for d in /usr/share/themes/*; do
    theme=${d##*/}
    if [[ -e $d/gtk-2.0/gtkrc ]]; then
        [[ $themelist ]] && themelist="$themelist!"
        [[ $theme == $GTKTHEME ]] && theme="^$theme"
        themelist+="$theme"
    fi
    if [[ -e $d/gtk-2.0-key/gtkrc ]]; then
        [[ $theme == $KEYTHEME ]] && theme="^$theme"
        keythemelist+="!$theme"
    fi
done

# create list of icon and cursor themes
iconthemelist="Default"
cursorthemelist="Default"
for d in /usr/share/icons/*; do
    theme=${d##*/}
    if [[ -e $d/index.theme ]]; then
        [[ $theme == $ICONTHEME ]] && theme="^$theme"
        iconthemelist+="!$theme"
    fi
    if [[ -d $d/cursors ]]; then
        [[ $theme == $CURSORTHEME ]] && theme="^$theme"
        cursorthemelist+="!$theme"
    fi
done

# create toolbar styles list
tbstylelist="Icons only"!
[[ $TBSTYLE == GTK_TOOLBAR_TEXT ]] && tbstylelist+="^"
tbstylelist+="Text only"!
[[ $TBSTYLE == GTK_TOOLBAR_BOTH ]] && tbstylelist+="^"
tbstylelist+="Text below icons"!
[[ $TBSTYLE == GTK_TOOLBAR_BOTH_HORIZ ]] && tbstylelist+="^"
tbstylelist+="Text beside icons"

# create list of toolbar icons sizes
tbiconsizelist="Menu"!
[[ $TBSTYLE == GTK_ICON_SIZE_SMALL_TOOLBAR ]] && tbiconsizelist+="^"
tbiconsizelist+="Small toolbar"!
[[ $TBSTYLE == GTK_ICON_SIZE_LARGE_TOOLBAR ]] && tbiconsizelist+="^"
tbiconsizelist+="Large toolbar"!
[[ $TBSTYLE == GTK_ICON_SIZE_BUTTON ]] && tbiconsizelist+="^"
tbiconsizelist+="Buttons"!
[[ $TBSTYLE == GTK_ICON_SIZE_DND ]] && tbiconsizelist+="^"
tbiconsizelist+="DND"!
[[ $TBSTYLE == GTK_ICON_SIZE_DIALOG ]] && tbiconsizelist+="^"
tbiconsizelist+="Dialog"

# theme page
yad --plug=$KEY --tabnum=1 --form --separator='\n' --quoted-output \
    --field="GTK+ theme::cb" "$themelist" \
    --field="GTK+ key theme::cb" "$keythemelist" \
    --field="Icons theme::cb" "$iconthemelist" \
    --field="Cursor theme::cb" "$cursorthemelist" \
    --field="Toolbar style::cb" "$tbstylelist" \
    --field="Toolbar icon size::cb" "$tbiconsizelist" \
    --field="Button images:chk" "$BUTTONIMAGE" \
    --field="Menu images:chk" "$MENUIMAGE" > $res1 &

# create list of hinting styles
hintstylelist="None"!
[[ $HINTSTYLE == hintslight ]] && hintstylelist+="^"
hintstylelist+="Slight"!
[[ $HINTSTYLE == hintmedium ]] && hintstylelist+="^"
hintstylelist+="Medium"!
[[ $HINTSTYLE == hintfull ]] && hintstylelist+="^"
hintstylelist+="Full"

# create list of rgba types
rgbalist="None"!
[[ $RGBA == rgb ]] && rgbalist+="^"
rgbalist+="RGB"!
[[ $RGBA == gbr ]] && rgbalist+="^"
rgbalist+="BGR"!
[[ $RGBA == vrgb ]] && rgbalist+="^"
rgbalist+="VRGB"!
[[ $RGBA == vgbr ]] && rgbalist+="^"
rgbalist+="VBGR"

# fonts page
yad --plug=$KEY --tabnum=2 --form --separator='\n' --quoted-output \
    --field="Font::fn" "$FONTDESCR" --field=":lbl" "" \
    --field="Use antialiasing:chk" "$ANTIALIASING" \
    --field="Use hinting:chk" "$HINTING" \
    --field="Hinting style::cb" "$hintstylelist" \
    --field="RGBA type::cb" "$rgbalist" > $res2 &

# parse color scheme
eval $(echo -e $COLORSCHEME | tr ';' '\n' | sed -r 's/:(.*)$/="\1"/')

# colors page
yad --plug=$KEY --tabnum=3 --form --separator='\n' --quoted-output \
    --field="Foreground::clr" $fg_color \
    --field="Background::clr" $bg_color \
    --field="Text::clr" $text_color \
    --field="Base::clr" $base_color \
    --field="Selected fore::clr" $selected_fg_color \
    --field="Selected back::clr" $selected_bg_color \
    --field="Tooltip fore::clr" $tooltip_fg_color \
    --field="Tooltip back::clr" $tooltip_bg_color  > $res3 &

# run main dialog
yad --notebook --key=$KEY --tab="Theme" --tab="Fonts" --tab="Colors" \
    --title="Interface settings" --image=gnome-settings-theme \
    --width=400 --image-on-top --text="Common interface settings"

# recreate rc file
if [[ $? -eq 0 ]]; then
    eval TAB1=($(< $res1))
    eval TAB2=($(< $res2))
    eval TAB3=($(< $res3))

    echo -e "\n# This file was generated automatically\n" > $rc_file

    echo "gtk-theme-name = \"${TAB1[0]}\"" >> $rc_file
    [[ ${TAB1[1]} != Default ]] && echo "gtk-key-theme-name = \"${TAB1[1]}\"" >> $rc_file
    [[ ${TAB1[2]} != Default ]] && echo "gtk-icon-theme-name = \"${TAB1[2]}\"" >> $rc_file
    [[ ${TAB1[3]} != Default ]] && echo "gtk-cursor-theme-name = \"${TAB1[3]}\"" >> $rc_file
    echo >> $rc_file

    case ${TAB1[4]} in
        "Icons only") echo "gtk-toolbar-style = GTK_TOOLBAR_ICONS" >> $rc_file ;;
        "Text only") echo "gtk-toolbar-style = GTK_TOOLBAR_TEXT" >> $rc_file ;;
        "Text below icons") echo "gtk-toolbar-style = GTK_TOOLBAR_BOTH" >> $rc_file ;;
        "Text beside icons") echo "gtk-toolbar-style = GTK_TOOLBAR_BOTH_HORIZ" >> $rc_file ;;
    esac
    case ${TAB1[5]} in
        "Menu") echo "gtk-toolbar-icon-size = GTK_ICON_SIZE_MENU" >> $rc_file ;;
        "Small toolbar") echo "gtk-toolbar-icon-size = GTK_ICON_SIZE_SMALL_TOOLBAR" >> $rc_file ;;
        "Large toolbar") echo "gtk-toolbar-icon-size = GTK_ICON_SIZE_LARGE_TOOLBAR" >> $rc_file ;;
        "Buttons") echo "gtk-toolbar-icon-size = GTK_ICON_SIZE_BUTTON" >> $rc_file ;;
        "DND") echo "gtk-toolbar-icon-size = GTK_ICON_SIZE_DND" >> $rc_file ;;
        "Dialog") echo "gtk-toolbar-icon-size = GTK_ICON_SIZE_DIALOG" >> $rc_file ;;
    esac
    echo >> $rc_file

    echo -n "gtk-button-images = " >> $rc_file
    [[ ${TAB1[6]} == TRUE ]] && echo 1 >> $rc_file || echo 0 >> $rc_file
    echo -n "gtk-menu-images = " >> $rc_file
    [[ ${TAB1[7]} == TRUE ]] && echo 1 >> $rc_file || echo 0 >> $rc_file
    echo >> $rc_file

    echo -e "gtk-font-name = \"${TAB2[0]}\"\n" >> $rc_file

    echo -n "gtk-xft-antialiasing = " >> $rc_file
    [[ ${TAB2[2]} == TRUE ]] && echo 1 >> $rc_file || echo 0 >> $rc_file
    echo -n "gtk-xft-hinting = " >> $rc_file
    [[ ${TAB2[3]} == TRUE ]] && echo 1 >> $rc_file || echo 0 >> $rc_file
    if [[ ${TAB2[4]} != $"None" ]]; then
        case ${TAB2[4]} in
            "Slight") echo 'gtk-xft-hintstyle = "hintslight"' >> $rc_file ;;
            "Medium") echo 'gtk-xft-hintstyle = "hintmedium"' >> $rc_file ;;
            "Full") echo 'gtk-xft-hintstyle = "hintful"' >> $rc_file ;;
        esac
    fi
    if [[ ${TAB2[5]} != $"None" ]]; then
        case ${TAB2[5]} in
            "RGB") echo 'gtk-xft-rgba = "rgb"' >> $rc_file ;;
            "BGR") echo 'gtk-xft-rgba = "bgr"' >> $rc_file ;;
            "VRGB") echo 'gtk-xft-rgba = "vrgb"' >> $rc_file ;;
            "VBGR") echo 'gtk-xft-rgba = "vbgr"' >> $rc_file ;;
        esac
    fi
    echo >> $rc_file

    echo "gtk-color-scheme = \"fg_color:${TAB3[0]};bg_color:${TAB3[1]};text_color:${TAB3[2]};base_color:${TAB3[3]};selected_fg_color:${TAB3[4]};selected_bg_color:${TAB3[5]};tooltip_fg_color:${TAB3[6]};tooltip_bg_color:${TAB3[7]}\"" >> $rc_file

    echo -e "\n# Custom settings\ninclude \"$rc_file.mine\"" >> $rc_file
fi

# cleanup
rm -f $res1 $res2 $res3
```