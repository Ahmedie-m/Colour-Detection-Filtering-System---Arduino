# Colour Detection & Filtering System - Arduino Code

This is the Arduino part of the development for my bachelor thesis device that sorts an orange by its ripeness

## Installing Dependencies

SSH into the Arduino Yun

`ssh root@YourArduinoAddressHere`

Before moving with the installations, run an update

`opkg update`

Install uhttpd - Should already be pre-installed

`opkg install uhttpd`

Install sqlite3-cli

`opkg install sqlite3-cli`

Install PHP and relevant libraries - Latest version of PHP as of now is 7.

`opkg install php7-cli php7-cgi php7-mod-sqlite3`

Install nano

`opkg install nano`

### Editing the uhttpd configuration file

Do

`nano /etc/config/uhttpd`

Then add

`list interpreter ".php=/usr/bin/php-cgi"`

`option index_page 'index.php'`

### Creating the database file

Do

`sqlite3 /mnt/sda1/sensor.db`

Then create the table

`CREATE TABLE "sensor_data" (id INTEGER PRIMARY KEY AUTOINCREMENT, type text, red INTEGER, green INTEGER, blue INTEGER, time DATETIME DEFAULT CURRENT_TIMESTAMP);`

Then insert some filler data

`INSERT INTO sensor_data (type, red, green, blue) VALUES ('ripe', '150', '20', '20');`

Then display the contents of the table

`SELECT * FROM sensor_data`

## Development

Anything in the www folder on your Arduino Yun is your website

`mkdir www/stats/`

Create an index.php file in stats

<?php
  echo "Hello World, running PHP test!"
?>

Restart the uhttpd server

`/etc/init.d/uhttpd restart`

Your website should be on your Arduino's Address

Ex: `192.168.1.165/stats/`

## Credits

Big thanks to this guide: https://hackaday.io/project/27905-setting-up-the-linkit-c-cross-compiler/log/71201-wifi-server