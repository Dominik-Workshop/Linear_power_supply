# Linear power supply

> [!CAUTION]
> This project was initiated in 2021 when my skills were more limited. As a result, there might be areas for improvement. I occasionally enhance both the performance and readability of project files for better understanding.

### More info about this project can be found on my channel: 
<img align="center" height="32" src="images/logos/YT.png"> https://youtu.be/JCxF-o6tLgA

## Used tools:
<img align="center" height="64" src="images/logos/Eagle.png"> &nbsp;&nbsp;&nbsp;&nbsp; &nbsp;&nbsp;&nbsp;&nbsp; 
<img align="center"  height="64" src="images/logos/Fusion-360.png">
#

## Project Overview
- Homemade linear power supply capable of delivering up to 250W (0-28V, 0.01-10A)
- Features adjustable voltage and current limit, controlled by 10-turn potentiometers.
- Incorporates an on/off switch for convenient power control.
- Utilizes a relay transformer-tab switching circuit to minimize power losses by reducing the voltage differential across the linear regulator.
- Enhanced heat dissipation through a temperature-controlled fan cooling the heatsink.

<img align="center" src="images/pictures/psu_front.png"> 

> [!IMPORTANT]
> I changed the Q1 transistor from IRF9530 (P-MOSFET) to BD912 (PNP-BJT) to gratly improve the stability of this power supply and completely eliminate oscillations in the constant current mode. (Their pinouts match, so it is a drop in replacement)
> <details>
> <summary>Click to learn more</summary>
> After performing some simulations in LTspice, I figured out that simply changing the Q1 MOSFET to a BJT greatly improved the output response to input step:
> Before (with MOSFET):
> <image src="images/screenshots/old_step.png">
> After (with BJT)
> <image src="images/screenshots/new_step.png">
> (The bode plot also looks better)
> </details>

## Otput Measurements
Output voltage waveforms before changing Q1 to a BJT can be found in measurements.pdf

## Project details
I used this 12 300VA transformer, which I sligtly modified. The secondary winding consisted of 4 wires connected in parallel. I split them in 2 and connected in series, getting 24V with a center tap). Last but not least I added another winding of around 8V.
![image](https://user-images.githubusercontent.com/100617381/183007895-9f96c52b-03fa-483e-87bb-669523269e95.png)

I used an old graphics card's heatsink for cooling.
![image](https://user-images.githubusercontent.com/100617381/183007879-ed8218c8-d9f7-4f5c-a0ad-9686e3b1729e.png)

It was based on SN1534 power supply, its schematics can be found there as well.
