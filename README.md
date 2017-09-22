# tophat_remote_control

Remote control box for tophat

Goal is to avoid the touch screen as much as possible.
Earlier attempts tried to use a teensy acting as a keyboard; moving to using a special serial driver on tophat (developed separetly), so we can use it over any channel offered e.g. wifi.

* few utils for dealing with xci files (xcsoar / tophat event files)
  * xci2dot - does what it says
* various xci files
* early prototype for code running on a teensy 2.0
