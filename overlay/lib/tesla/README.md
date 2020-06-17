# libtesla

<p align="center">
  <img src="https://i.imgur.com/fhwyjbd.png">
</p>

libtesla is the interface between the Tesla overlay loader and user-made Overlays. It handles all layer creation, UI creation, drawing and input management.
It's main goal is to make sure all overlays look and feel similar and don't differenciate themselves from the switch's native overlays.

## Screenshots

<div>
    <img src="https://i.imgur.com/jJpxYjb.jpg" style="width: 50%; float: center">
    <img src="https://i.imgur.com/nBUc7ps.jpg" style="width: 50%; float: right">
</div>

`Overlays do NOT show up on Screenshots. These pictures were taken using a capture card`

## Example

An example for how to use libtesla can be found here: https://github.com/WerWolv/libtesla/tree/master/example
To create your own Overlay, please consider creating a new repository using the official Tesla overlay template: https://github.com/WerWolv/Tesla-Template

**Please Note:** While it is possible to create overlays without libtesla, it's highly recommended to not do so. libtesla handles showing and hiding of overlays, button combo detection, layer creation and a lot more. Not using it will lead to an inconsistent user experience when using multiple different overlays ultimately making it worse for the end user. If something's missing, please consider opening a PR here.

## Credits

- **switchbrew** for nx-hbloader which is used as basis for overlay loading
- **kardch** for the amazing icon
- **All the devs on AtlasNX, RetroNX and Switchbrew** for their feedback
- **All overlay devs** for making something awesome out of this :)

