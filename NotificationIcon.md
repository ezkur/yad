# Using the Notification API #
Yad has a significantly more powerful notification API than it's predecessor Zenity. As well as creating notifications to display in the system tray using GTK, it is also possible to modify the tray icon and it's tray menu.

**NOTE:** To make use of these features, it is necessary to use version 0.12.2 of the library or greater.

## Introduction ##
Yad is a program designed to be run from a shell environment or as an external process called by an application. As with all of Yad's APIs, the option flags can be looked up through the "`--help-all`" option or more specifically with the "`--help-<api>`" option. In the case of the Notification API this is "`--help-notification`".

For the purpose of these examples `#>` displays running a command from the terminal.

e.g.
```
#> yad --help-notification
Usage:
  yad [OPTION...] Yet another dialoging program

Notification icon options
  --notification                                 Display notification
  --command=CMD                                  Set left-click action
  --listen                                       Listen for commands on stdin
  --separator=SEPARATOR                          Set separator character for menu values
  --item-separator=SEPARATOR                     Set separator character for menu items
```

## Creating a Notification ##
Creating a notification is as simple as:
```
#> yad --notification &
```

This will display Yad's default tray notification, executed as a separate process. By default the notification looks like a small science beaker (seen on the Google Code page top left as the logo). The default event handler for the left click (and middle click) from the mouse is to quit (stop yad). The right click handler does nothing.

To add a left click action:
```
#> yad --notification --command="echo 'Hello World'"
```
_NOTE:_ This example does not execute yad as a separate process.

To create Yad with a specific tray icon use:
```
#> yad --notification --image="gtk-help"
```

Unlike Zenity, the icon naming convention does not look for stock icon names but the names used by the current gtk+ icon theme. To see a list of the icon names currently available in your icon
theme, use yad-icon-browser (if built) or see [Icon Themes](http://live.gnome.org/GnomeArt/Tutorials/IconThemes).

A (mildly) more complex example combining a few more option flags is:
```
#> yad --notification --image="gtk-help" --command="echo 'Hello World'" --text="The notification's tooltip"
```

## Updating a Notification ##
A powerful feature of Yad is the ability to reconfigure a notification, that's already being displayed. This can be done using the "listen" option flag. Once done, commands are listened for on stdin.

To change the notification icon use:
```
#> yad --notification --image="gtk-help" --listen
#> icon:gnome-media-play-ltr
#> quit
```

A full list of the commands can be found on the manpage. Possible commands are `icon`, `tooltip`, `visible`, `action`, `menu` and `quit`.

It's great to be able to control the notification settings from stdin but more useful if the process can manage them and modify them itself. This is possible by redirecting the I/O stream in a shell script.

An example shell script:
```
#!/usr/bin/env bash

# create a FIFO file, used to manage the I/O redirection from shell
PIPE=$(mktemp -u --tmpdir ${0##*/}.XXXXXXXX)
mkfifo $PIPE

# attach a file descriptor to the file
exec 3<> $PIPE

# add handler to manage process shutdown
function on_exit() {
    echo "quit" >&3
    rm -f $PIPE
}
trap on_exit EXIT

# add handler for tray icon left click
function on_click() {
    echo "clicked"
}
export -f on_click

# create the notification icon
yad --notification                  \
    --listen                        \
    --image="gtk-help"              \
    --text="Notification tooltip"   \
    --command="bash -c on_click" <&3
```

## Bigger Example ##
For a bigger example of the notification API, see the [USBFlash wiki](http://code.google.com/p/yad/wiki/USBFlash)