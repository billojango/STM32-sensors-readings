#include "mbed.h"

Serial pc(USBTX, USBRX); // Serial connection to PC
Serial modem(PD_2, PC_12); // RX, TX for modem

int main()
{
    pc.baud(115200); // Set baud rate for PC serial
    modem.baud(115200); // Set baud rate for modem serial
    
    pc.printf("Initializing...\r\n");
    ThisThread::sleep_for(1s); // Wait for 1 second
    
    while (1) {
        modem.printf("AT+CGSN\r\n"); // Send AT command to modem
        
        char response[64]; // Allocate a buffer for the response
        int num_bytes = modem.read(response, sizeof(response) - 1); // Read bytes from modem
        if (num_bytes > 0) {
            response[num_bytes] = '\0'; // Null-terminate the response string
            pc.printf("IMEI: %s\r\n", response); // Print the IMEI to PC serial
        }
        
        ThisThread::sleep_for(5s); // Wait for 5 seconds before sending the command again
    }
}
