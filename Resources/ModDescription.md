
# SMART Wireless Lighting 

Smart Wireless Lighting takes advantage of the Lighting network from Satisfactory but allows a user to manage and connect to those lights from the SMART Wireless Lighting Control Panel.

![Multiplayer Compatible](https://github.com/deantendo/community/blob/master/com_mp_yes.png?raw=true)

### Unlocked in the Resource Sink Shop in the Organization section where the other Lighting buildables are located. BUT ITS FREEEEEE

<p>
   <img src="https://github.com/djtheseira/SmartWirelessLighting/blob/main/Resources/SMARTLP_Configure_640.png?raw=true" width="640" height="636" alt="Lights Activated with SMART Lighting Panel" />
</p>

<p>
   <img src="https://github.com/djtheseira/SmartWirelessLighting/blob/main/Resources/SMARTLP_Configuration_640.png?raw=true" width="640" height="568" alt="SMART Lighting Panel Light Configuration" />
</p>

<p>
   <img src="https://github.com/djtheseira/SmartWirelessLighting/blob/main/Resources/SMARTLP_LightSelection_640.png?raw=true" width="640" height="757" alt="Lights Activated with SMART Lighting Panel Configuration" />
</p>

<p>
   <img src="https://github.com/djtheseira/SmartWirelessLighting/blob/main/Resources/SMARTLP_Menu_640.png?raw=true" width="640" height="299" alt="Lights Activated with SMART Lighting Panel Configuration" />

</p>

# Features

- SMART Wireless Lighting Panel
   
   - This panel is built off the regular Lighting Control Panel, but adds a secondary screen that displays the Lights that are not connected to any other systems and shows the Lights that are connected to the current Control Panel.

# Known Issues

   - When connecting/disconnecting a light from a normal cable, the light list will not update. (Fix coming soon)
   - Daisy chaining doesn't work. The SMART wireless light panel ignores a light with a connection (for now). The issue above can also cause an issue where the light either appears or disappears after a different refresh. The daisy chaining with cables issue will be addressed ASAP, but the first issue is my current priority.

# New Features

   - Add a map to help identify which lights can be connected to as well as which are currently connected.
   - Add ability to change names of lights like trains naming system (maybe..)

# Changelog

### 1.0.4

   - Updated to support U6
   - Reworked almost the entire system to (hopefully) make things more efficient.
   - Removed a ton of bloated code that needed to be deprecated.
   - Fixed how the list of lights is updated so that it actually updates properly.

### 1.0.3

   - Fixed light list screen wasn't refreshing properly.
   - Small changes to decrease workload on the lists page.

### 1.0.2

   - Fixed loading issue

### 1.0.1

   - Updated to support U5

### 1.0.0

   - Updated to support the new Update 5
   - Resolve issue where list of lights wouldn't load properly when switching between the Light Details & Light Selection window.

### 0.9.6

   - If a Light is not connected to a Light Control Panel or SMART Lights Control Panel, it will be available to be connected to, as long as there are free connections.

### 0.9.5

   - Fixed distance always set to 0m after building a light.
   - Fixed issue that happened when disconnecting from a light, it wouldn't clear out the connection and the light would think it didn't have free connections.
   - Fixed Light list not updating when a different SMART Control Panel connected/disconnected from a light.

### 0.9.4:

   - I didn't update the RemoteVersionRange, so client couldn't connect to a host when using this mod. Pushing this ASAP so multiplayer actually works.

### 0.9.3:

   - The sorting I added in .2 messed up the original sorting, so swapped the logic.

### 0.9.2:

   - Changed location of item in the Resource Sink Shop.
   - Fixed the Lights selection boxes so that they're ACTUALLY scrollable.
   - Changed the sorting on Lights so that the ones that are connected are displayed first as well.

### 0.9.1:
   
   - Added a sort method onto the Light list, which sorts the closest Lights first.

### 0.9.0: 

   - Officially in beta, things are working quite nicely. 
   - Fixed multiplayer support and essentially reworked the entire system to ensure that there wasn't a major difference between single and multiplayer. 
   - Changed how connections are made, using a custom power line.
   - Fixed when removing or adding a new Light, the list is properly updated.
   - Fixed issue where destroying a SMART Lighting Panel, the Light list is updated with any Lights that were connected to the destroyed building.

### 0.1.0:

   - Original upload to ficsit.app, was very alpha and needed quite a bit of rework. 

# Warning

This mod, just like most other mods, may cause issues with save games. Due to the overall connections between this mod and the built-in fuctionalities from CSS, whenever a major update to the lighting system happens, it can cause loading issues for your world. If you have any issues or bugs related to this mod, please tag me @crouton on discord or create an issue on GitHub describing the problem IMMEDIATELY.

# Contributors

- Crouton
- Deantendo