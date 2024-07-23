# Incremental/quadrature encoder pulse timing for speed calculation (STM32L432KC)
 An STM32 HAL example of speed calculation using pulse timing.

 ![Incremental encoder lab set](/Assets/Images/incremental_encoder_lab_set.jpg)

A timer in the encoder mode:

 ![TIM Encoder Mode](/Assets/Images/tim_encoder.JPG)

A timer in the input capture mode:

 ![TIM Encoder Mode](/Assets/Images/tim_input_capture.JPG)

Not any two timers would do the job. You have to choose timers that are connected in such a way that the pulse detection event from the timer in the encoder mode is able to trigger the timer that operates in the input capture mode.

![TIM Encoder Mode](/Assets/Images/tim_internal_trigger_connection.JPG)

Source: RM0394 Reference manual STM32L41xxx/42xxx/43xxx/44xxx/45xxx/46xxx advanced Arm-based 32-bit MCUs

> [!TIP]
> If you want to go for very low speeds, you may need to monitor the overflow events for the timer in the input capture mode and count the number of them. Then you can use the number of overflows and the input captured value to get the time between two consecutive encoder pulses.

# Speed calculation: pulse timing vs. pulse counting
* [Chapter 7. Enhanced Quadrature Encoder Pulse (eQEP)](https://www.ti.com/lit/ug/sprui10a/sprui10a.pdf) (Texas Instruments)
* [How are encoders used for speed measurement?](https://www.motioncontroltips.com/how-are-encoders-used-for-speed-measurement/) (Motion Control Tips)
* [How to Estimate Encoder Velocity Without Making Stupid Mistakes: Part I](https://www.embeddedrelated.com/showarticle/158.php) (Embedded Related)

# Missing files?
Don't worry :slightly_smiling_face: Just hit Alt-K to generate /Drivers/CMCIS/ and /Drivers/STM32L4xx_HAL_Driver/ based on the .ioc file. After a couple of seconds your project will be ready for building.

# Exemplary hardware
* [DC motor with encoder](https://www.dfrobot.com/product-1457.html) (DFRobot)
* [Rotary encoder module ](https://sklep.msalamon.pl/produkt/modul-enkodera-z-przyciskiem/)
* [Tachometer GM8905](http://www.benetechco.net/en/products/gm8905.html) (Benetech)
* [Tachometer AT-6](https://www.cem-instruments.com/en/product-id-1313) (CEM)
* [Bidirectional 4-channel logic level shifter](https://sklep.msalamon.pl/produkt/konwerter-poziomow-logicznych-33v-5v-4-kanalowy/)
* [OLED display 1.3" (SH1106 or SSD1306)](https://sklep.msalamon.pl/produkt/wyswietlacz-oled-13-i2c-bialy/)

# Libraries
* OLED: [stm32-ssd1306](https://github.com/afiskon/stm32-ssd1306) (MIT license)

# Call for action
Create your own [home laboratory/workshop/garage](http://ufnalski.edu.pl/control_engineering_for_hobbyists/2024_dzien_otwarty_we/Dzien_Otwarty_WE_2024_Control_Engineering_for_Hobbyists.pdf)! Get inspired by [ControllersTech](https://www.youtube.com/@ControllersTech), [DroneBot Workshop](https://www.youtube.com/@Dronebotworkshop), [Andreas Spiess](https://www.youtube.com/@AndreasSpiess), [GreatScott!](https://www.youtube.com/@greatscottlab), [ElectroBOOM](https://www.youtube.com/@ElectroBOOM), [Phil's Lab](https://www.youtube.com/@PhilsLab), [atomic14](https://www.youtube.com/@atomic14), [That Project](https://www.youtube.com/@ThatProject), [Paul McWhorter](https://www.youtube.com/@paulmcwhorter), and many other professional hobbyists sharing their awesome projects and tutorials! Shout-out/kudos to all of them!

> [!WARNING]
> Control in power electronics and drives - try this at home :exclamation:

190+ challenges to start from: [Control Engineering for Hobbyists at the Warsaw University of Technology](http://ufnalski.edu.pl/control_engineering_for_hobbyists/Control_Engineering_for_Hobbyists_list_of_challenges.pdf).

Stay tuned!
