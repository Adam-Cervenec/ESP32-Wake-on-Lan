# ESP family WOL

Wake up your PC (or server) wirelessly using an ESP32.  

**Quick Start (for experienced ESP users):**  
1. Fill in the top of the code:
   - `SSID` / `password`
   - `BUTTON_PIN`
   - `BROADCAST_IP`, `PC_IP`
   - `MAC` address of the PC
   - Adjust `LED` logic if needed
2. Upload code via Arduino IDE to your ESP32.  
3. On startup:
   - 2 quick blinks = ESP started
   - 5 blinks = WiFi connected  
4. Press the button:
   - LED blinks once = WOL packet sent
   - Check Serial Monitor for status
5. Ensure ESP is powered separately from the PC to wake it up.

**Notes:**  
- Works with multiple ESP devices.  
- LED debug helps if Serial Monitor isn’t used.  

For full instructions, hardware setup, and troubleshooting, see the [Wiki](https://github.com/Adam-Cervenec/ESP32-Wake-on-Lan/wiki)).
