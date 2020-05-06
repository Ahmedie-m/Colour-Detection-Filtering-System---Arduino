# Colour Detection & Filtering System - Arduino Code

This is the Arduino part of the development for my bachelor thesis device that sorts an orange by its ripeness

## Installing Dependencies

SSH into the Arduino Yun
`ssh root@YourArduinoAddressHere`

Before moving with the installations, run an update

`opkg update`

Install uhttpd

`opkg install uhttpd`

Install sqlite3-cli

`opkg install sqlite3-cli`

Install PHP and relevant libraries

`opkg install php7-cli php7-mod-sqlite3`

Latest version of PHP as of now is 7.

Install nano

`opkg install nano`