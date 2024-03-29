/*
  Twitter Client with Strings

 This sketch connects to Twitter using an Ethernet shield. It parses the XML
 returned, and looks for <text>this is a tweet</text>

 You can use the Arduino Ethernet shield, or the Adafruit Ethernet shield,
 either one will work, as long as it's got a Wiznet Ethernet module on board.

 This example uses the DHCP routines in the Ethernet library which is part of the
 Arduino core from version 1.0 beta 1

 This example uses the String library, which is part of the Arduino core from
 version 0019.

 Circuit:
  * Ethernet shield attached to pins 10, 11, 12, 13

 created 21 May 2011
 by Tom Igoe

 This code is in the public domain.

 */
#include <SPI.h>
#include <Ethernet.h>


// Enter a MAC address and IP address for your controller below.
// The IP address will be dependent on your local network:
byte mac[] = {
  0x00, 0xAA, 0xBB, 0xCC, 0xDE, 0x01 };
IPAddress ip(192,168,1,24);

// initialize the library instance:
EthernetClient client;

const int requestInterval = 60000;  // delay between requests

char serverName[] = "docs.google.com";  // twitter URL

boolean requested;                   // whether you've made a request since connecting
long lastAttemptTime = 0;            // last time you connected to the server, in milliseconds

String currentLine = "";            // string to hold the text from server
String tweet = "";                  // string to hold the tweet
boolean readingTweet = false;       // if you're currently reading the tweet

void setup() {
  // reserve space for the strings:
  currentLine.reserve(256);
  tweet.reserve(150);

// initialize serial:
  Serial.begin(9600);
  Serial.println("Booting up...");
  // attempt a DHCP connection:
  if (!Ethernet.begin(mac)) {
    Serial.println("DHCP failed.");
    // if DHCP fails, start with a hard-coded address:
    Ethernet.begin(mac, ip);
  }
  // connect to Twitter:
  connectToServer();
}



void loop()
{
  if (client.connected()) {
    if (client.available()) {
      // read incoming bytes:
      char inChar = client.read();

      // add incoming byte to end of line:
      currentLine += inChar;

      // if you get a newline, clear the line:
      if (inChar == '\n') {
        currentLine = "";
      }
      // if the current line ends with <text>, it will
      // be followed by the tweet:
      if ( currentLine.endsWith("<text>")) {
        // tweet is beginning. Clear the tweet string:
        readingTweet = true;
        tweet = "";
      }
      // if you're currently reading the bytes of a tweet,
      // add them to the tweet String:
      if (readingTweet) {
        if (inChar != '<') {
          tweet += inChar;
        }
        else {
          // if you got a "<" character,
          // you've reached the end of the tweet:
          readingTweet = false;
          Serial.println(tweet);
          // close the connection to the server:
          client.stop();
        }
      }
    }
  }
  else if (millis() - lastAttemptTime > requestInterval) {
    // if you're not connected, and two minutes have passed since
    // your last connection, then attempt to connect again:
    connectToServer();
  }
}

void connectToServer() {
  // attempt to connect, and wait a millisecond:
  Serial.println("connecting to server...");
  if (client.connect(serverName, 80)) {
    Serial.println("making HTTP request...");
  // make HTTP GET request to twitter:
    //client.println("GET /1/statuses/user_timeline.xml?screen_name=arduino&count=1 HTTP/1.1");
    //client.println("HOST: api.twitter.com");
    client.println("POST /spreadsheet/formResponse?pli=1&formkey=dEQ3WG4zVHJHTXRSSFg5TjBZUTF0cnc6MQ&ifq HTTP/1.1");
    client.println("Host: docs.google.com");
    client.println("Connection: close");
    client.println("Content-Type: application/x-www-form-urlencoded");
    client.println("Content-Length: 457");
    client.println();
    client.print("entry.0.single=2012-03-24T04%3A00%3A57Z&entry.1.single=00%3AAA%3ABB%3ACC%3ADE%3A01&entry.2.single=temperature&entry.3.single=23.4&entry.4.single=humidity&entry.5.single=43&entry.6.single=light&entry.7.single=50&entry.8.single=battery&entry.9.single=90&entry.10.single=voltage&entry.11.single=3.3&entry.12.single=i2c&entry.13.single=23&entry.14.single=lux&entry.15.single=40&entry.16.single=voltage&entry.17.single=100&pageNumber=0&backupCache=&submit=Submit");
  }
  // note the time of this connect attempt:
  lastAttemptTime = millis();
}
