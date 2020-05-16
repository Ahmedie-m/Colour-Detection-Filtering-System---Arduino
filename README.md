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

Install PHP and relevant libraries - Latest version of PHP as of now is 7

`opkg install php7-cli php7-cgi php7-mod-sqlite3 php7-mod-json`

Install nano

`opkg install nano`

### Editing the uhttpd configuration file

Do

`nano /etc/config/uhttpd`

Then add

`list interpreter ".php=/usr/bin/php-cgi"`

`option index_page 'index.php'`

### Creating the SQLite database

Do

`sqlite3 /mnt/sda1/sensor.db`

Then create the table

`CREATE TABLE "sensor_data" (id INTEGER PRIMARY KEY AUTOINCREMENT, type text, red INTEGER, green INTEGER, blue INTEGER, time DATETIME DEFAULT CURRENT_TIMESTAMP);`

Then insert some filler data

`INSERT INTO sensor_data (type, red, green, blue) VALUES ('ripe', '150', '20', '20');`

Then display the contents of the table

`SELECT * FROM sensor_data;`

### Creating the PHP file that takes in the query

Create the PHP file in the following area

`nano /mnt/sda1/sensor.php`

Insert this code

```
#!/usr/bin/php-cli
<?php
$db = new SQLite3('/mnt/sda1/sensor.db');
$query = "INSERT INTO sensor_data (type, red, green, blue) VALUES( '$argv[1]', '$argv[2]', '$argv[3]', '$argv[4]' )";
$db->exec($query);
$db->close();
?>
```

### Creating the PHP file that displays your database results

Create the PHP file in

`nano /www/stats/database.php`

Type this in

```
<?php
$db = new SQLite3('/mnt/sda1/sensor.db');
$results = $db->query('select * from sensor_data');
$cols = $results->numColumns();
print "<table width='400'  border='1' cellspacing='0' cellpadding='0'>\n";
print "<tr>";
for ($i = 0; $i < $cols; $i++) {
        print "<th align='left'>".$results->columnName($i)."</th>";
}
print "</tr>\n";
while ($row = $results->fetchArray()){
        print "<tr>";
        for ($i = 0; $i < $cols; $i++) {
                print "<td>".$row[$i]."</td>";
        } //end for
        print "</tr>\n";
}
print "</table>\n";
$db->close();
?>
```

Your database will be viewed at `YourArduinoAddressHere/stats/database.php`

## Development

Anything in the www folder on your Arduino Yun is your website

`mkdir www/stats/`

Create an index.php file in stats

```
<?php
  echo "Hello World, running PHP test!"
?>
```

Restart the uhttpd server

`/etc/init.d/uhttpd restart`

Your website should be on your Arduino's Address

Ex: `192.168.1.165/stats/`


### SQLite3 Commands

To clean up the database

`DELETE FROM sensor_data;`

To quit SQLite

`.quit`

## Other

This was just created to test the capabilities of the Wi-Fi module on the Arduino Yun

The color detection and sorting device can be turned off by going to `YourArduinoAddressHere/arduino/statusfilter/0/`

And back on using The color detection and sorting device can be turned off by going to `YourArduinoAddressHere/arduino/statusfilter/1/`


## Credits

Big thanks to this guide on how to setup a server: https://hackaday.io/project/27905-setting-up-the-linkit-c-cross-compiler/log/71201-wifi-server

Amazing guide for publishing data on the Arduino Yun locally: https://forum.arduino.cc/index.php?topic=309600.0