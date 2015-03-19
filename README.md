Spark Open Source Thermostat V2
===============================

![](https://igcdn-photos-g-a.akamaihd.net/hphotos-ak-xaf1/t51.2885-15/11032996_1564809190453678_79569625_n.jpg)

This repo contains all of the software and hardware designs required to run your own open source thermostat. This is successor to [spark/thermostat](https://github.com/spark/thermostat).

## Hardware

The thermostat display is a [2.2" Serial SPI TFT Color LCD](http://www.dx.com/p/2-2-serial-spi-tft-color-lcd-module-for-arduino-red-silver-234675?Utm_rid=62061796&Utm_source=affiliate).

The temperature is sensed using a
[DS18B20](http://www.dx.com/p/ds18b20-digital-temperature-sensor-module-for-arduino-55-125-c-135047?Utm_rid=62061796&Utm_source=affiliate).

User input is recognized using [Rotary Incremental Encoder](http://www.dx.com/p/volume-control-pulse-potentiometer-knob-360-rotary-incremental-encoder-module-for-arduino-board-331559?Utm_rid=62061796&Utm_source=affiliate).

Heater/fan is being driven using [Opto-isolated Relay](http://www.dx.com/p/jtron-1-channel-opto-isolated-relay-module-isolation-module-black-270934?Utm_rid=62061796&Utm_source=affiliate).

### Schematic

The details of the circuit design and implementation can be viewed via EAGLE (a free program for PCB design, [downloadable here](http://www.cadsoftusa.com/download-eagle/)).
The eagle file is in the [hardware/EAGLE folder](hardware/EAGLE).

### Physical Design

3D model [sketch-up](http://www.sketchup.com/products/sketchup-make) files are available in [hardware/folder](hardware/CAD).
If you're viewing this on GitHub (and using a modern browser), you can [view a 3D rendering](hardware/CAD/thermostat.stl) of it too.

## Firmware

To build firmware for this product, you'll first need [Spark Dev](https://www.spark.io/dev).

Once you have that, you can simply open `/firmware` directory from this repo and compile/flash.

Alternatively, you could use the [Spark Web IDE](https://www.spark.io).

## Server Setup

The server components are:
* a Rails 3.2 app
* Sidekiq and Redis for background job processing

Development was done on OS X.

### Dependencies

* Ruby 2.0 (recommended to use rvm, rbenv, chruby, or other ruby version manager) (this app is ruby-2.0.0-p247)
* bundler: `gem install bundler`
* Install redis: `brew install redis`
* Install foreman: download package from here https://github.com/ddollar/foreman
* `bundle install`

### Run Locally

In one terminal:

    redis-server /usr/local/etc/redis.conf

(following instructions from homebrew install)

In another terminal:

    bash script/start_server.sh

* Note: You might need to change the SPARK_CORE_DEVICE_ID and SPARK_CORE_ACCESS_TOKEN env vars in this .sh file
  for this to work.

Hit `http://localhost:5000` in a browser

### Dev tips

You can put `binding.pry` pretty much anywhere in the code to bring up an interative debuging console.
Then hit a brower make or whatever, and in the terminal where the start_server.sh script ran, you can type commands (like a var name) and see their value, etc.

The "SQLite Professional Read-Only" OS X app is a nice free app to look at the data the app is generating in real time.

When developing, it's nice to be able to destroy everything and start fresh when you are hacking:

    bash script/bomb_and_rebuild.sh

### What's Next?

Get your hack on, :), have fun. If you do something cool with anything in here, consider sharing it on the [community site](http://community.sparkdevices.com).

Check out some of these other related open thermostat projects:

* https://github.com/spark/thermostat
* https://github.com/science/openthermo
* http://www.adafruit.com/blog/2014/01/13/ardustat-web-controlled-wireless-arduino-based-thermostat-featuring-adafruit-cc3000-arduino/
* http://thermostatmonitor.com/
* https://github.com/robertdolca/thermostat-web

If you want to list your cool open thermostat project here, issue a pull request.

If your are interested in continuing the effort to build an easy-to-build-and-install Spark powered open-source thermostat, please contact joe@spark.io or create a GitHub issue.
Though we'd love to do this ourselves, we don't currently have the bandwidth here at Spark. Hope to hear from you.
