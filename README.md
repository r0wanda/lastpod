# lastpod

uploads ipod plays to last.fm

dont forget to set up .env.template and rename it to .env

## automation
i have a raspberry pi that i plug my ipod into to upload my streams onto last.fm. i included a udev rule to automatically mount an ipod and run lastpod.

### IMPORTANT
to use the udev rule, you must configure it first.

first find your ipod's device and vendor ids. open a shell and run `lsusb`:
<pre>
$ lsusb
...
Bus 001 Device 011: ID <b style="color:red;">05ac</b>:<b style="color:green;">1262</b> Apple, Inc. iPod Nano 3.Gen
</pre>
you're looking for the `xxxx:xxxx` part (bolded above)

now, in the provided <code><b>85-ipod_lastpod_rule.rules</b></code> file:
<pre>
ACTION=="add", SUBSYSTEM=="usb", SYSFS{idVendor}=="<b style="color:red;">FIRST</b>", SYSFS{idProduct}=="<b style="color:green;">SECOND</b>", RUN+="su -l --shell='/bin/bash /home/<b style="color:yellow;">USER</b>/lastpod/lastpod.sh' <b style="color:yellow;">USER</b>"
</pre>
replace <b style="color:red;">FIRST</b> with the first number from `lsusb` (before the colon `:`). in this example that would be <b style="color:red;">05ac</b>

then, replace <b style="color:green;">SECOND</b> with the second number (after the colon). for example here that would be <b style="color:green;">1262</b></code>

then replace <b style="color:yellow;">USER</b> with your personal computer username (or whatever user you install lastpod with)

finally copy 85-ipod_lastpod_rule.rules to /etc/udev/rules.d/85-ipod_lastpod_rule.rules
