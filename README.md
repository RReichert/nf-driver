nf-drvier
=========

the intent of this project is to develop a linux driver for the Novint Falcon product so as to allow individuals to freely develop applications for the mechanical robot. although there are drivers out there that would allow me to do so, i've found various limitations that deterred me from using them. the products that i've come across are:

  * **Novint Falcon SDK**: the libraries exposed by the SDK are quite limited in that they don't provide direct access to the motors sensor data and give the ability to actuate motors independently. furthermore the fact that it was developed on the Windows OS system means that you are unable to (to the best of my knowledge) develop applications for embedded systems.
  * **libnifalcon**: although the cross platform library works amazingly well (check it out as it is quite impressive), i've noticed some issues with it when porting it to the Linux system. the operational baud rate was 20% less then its designed capacity, this could be due to user-space communication via the libusb library. although i haven't tested it, i theorize that a lot of overhead might be in place using this approach and therefore would be unsuitable for embedded systems or at least non-optimal.

be aware that i'm developing this code for my own amusement and to satisfy my insatiable desire to learn, so please don't use my code for something extremely important (ex: major university project). at this point in the project, i simply want to play around with the driver and learn a thing or two. who knows, maybe later on it becomes something more.
