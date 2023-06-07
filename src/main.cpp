#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>

#include <TFT_eSPI.h> // Graphics and font library for ST7735 driver chip
#include <SPI.h>
TFT_eSPI tft = TFT_eSPI(); // Invoke library, pins defined in User_Setup.h

const char *ssid = "W";            // Wifi credentials
const char *password = "12121212"; //

void handle_NotFound();  //
void handle_led1off();   //
void handle_led2off();   // handles
void handle_led2on();    //
void handle_led1on();    //
void handle_OnConnect(); //
void handle_sleep();     //

String SendHTML(uint8_t led1stat, uint8_t led2stat); // function to generate and send the webpage
void displayLoop();                                  // a function to display the data on the integrated display

WebServer server(80); // server object constructor

uint8_t LED1pin = BUILTIN_LED;
bool LED1status = LOW;

uint8_t LED2pin = 5;
bool LED2status = LOW;

void setup()
{
  Serial.begin(9600);
  pinMode(LED1pin, OUTPUT);
  pinMode(LED2pin, OUTPUT);

  WiFi.begin(ssid, password);           // initiate the WiFi connection
  while (WiFi.status() != WL_CONNECTED) // wait until WiFi is connected
    ;

  Serial.println("WiFi connected"); // prints that the WiFi is connected to serial for debugging
  Serial.println("IP address: ");   //
  Serial.println(WiFi.localIP());   // prints the current IP address to serial for debugging

  server.on("/", handle_OnConnect);
  server.on("/led1on", handle_led1on);
  server.on("/led1off", handle_led1off);
  server.on("/led2on", handle_led2on);
  server.on("/led2off", handle_led2off);
  server.on("/deepsleep", handle_sleep);
  server.onNotFound(handle_NotFound);

  server.begin();
  Serial.println("HTTP server started");

  tft.init();
  tft.setRotation(1);
  displayLoop();
  delay(300);
}
void loop()
{
  bool lastLED1status = LED1status;
  bool lastLED2status = LED2status;
  server.handleClient();
  if (LED1status != lastLED1status or LED2status != lastLED2status)
  {
    displayLoop();
  }
}

void handle_OnConnect()
{
  LED1status = LOW;
  LED2status = LOW;
  Serial.println("GPIO4 Status: OFF | GPIO5 Status: OFF");
  server.send(200, "text/html", SendHTML(LED1status, LED2status));
}

void handle_led1on()
{
  LED1status = HIGH;
  Serial.println("GPIO4 Status: ON");
  server.send(200, "text/html", SendHTML(true, LED2status));
}

void handle_led1off()
{
  LED1status = LOW;
  Serial.println("GPIO4 Status: OFF");
  server.send(200, "text/html", SendHTML(false, LED2status));
}

void handle_led2on()
{
  LED2status = HIGH;
  Serial.println("GPIO5 Status: ON");
  server.send(200, "text/html", SendHTML(LED1status, true));
}

void handle_led2off()
{
  LED2status = LOW;
  Serial.println("GPIO5 Status: OFF");
  server.send(200, "text/html", SendHTML(LED1status, false));
}

void handle_NotFound()
{
  server.send(404, "text/plain", "Not found");
}

void handle_sleep()
{
  Serial.print("sleeping");
  esp_deep_sleep_start();
}

String SendHTML(uint8_t led1stat, uint8_t led2stat)
{
  String HTML = "<!DOCTYPE html> <html>\n";
  HTML += "<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, user-scalable=no\">\n";
  HTML += "<title>simple IoT webserver</title>\n";
  HTML += "<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}\n";
  HTML += "body{margin-top: 50px; background-color: black; color: white;} h1 {color: #444444;margin: 50px auto 30px;} h3 {color: #444444;margin-bottom: 50px;}\n";
  HTML += ".button {display: block;width: 80px;background-color: #3498db;border: none;color: white;padding: 13px 30px;text-decoration: none;text-align:center;font-size: 25px;margin: 0px auto 35px;cursor: pointer;border-radius: 4px;}\n";
  HTML += ".button-on {background-color: #8a1111;}\n";
  HTML += ".button-on:active {background-color: #9e1313;}\n";
  HTML += ".button-off {background-color: #0f8c26;}\n";
  HTML += ".button-off:active {background-color: #12a32d;}\n";
  HTML += "p {font-size: 14px;color: #888;margin-bottom: 10px;}\n";
  HTML += "</style>\n";
  HTML += "</head>\n";
  HTML += "<body>\n";
  HTML += "<h1><font color=\" #f5f5f5 \">ESP32 Web Server</font></h1>\n";
  // HTML += "<h3>Using Access Point(AP) Mode</h3>\n";
  HTML += "<p> this is a very simple demo for the IoT course by:<br> <font color=\" #6d93c9 \"> Abdullah Al-Daigani </font> and <font color=\" #6d93c9 \">Bader Al-Dakhil Allah</font></p>";
  HTML += "<p> <br> </p>";
  if (led1stat)
  {
    HTML += "<p>LED1 Status: ON</p><a class=\"button button-off\" href=\"/led1off\">turn OFF</a>\n";
  }
  else
  {
    HTML += "<p>LED1 Status: OFF</p><a class=\"button button-on\" href=\"/led1on\">turn ON</a>\n";
  }

  if (led2stat)
  {
    HTML += "<p>LED2 Status: ON</p><a class=\"button button-off\" href=\"/led2off\">turn OFF</a>\n";
  }
  else
  {
    HTML += "<p>LED2 Status: OFF</p><a class=\"button button-on\" href=\"/led2on\">turn ON</a>\n";
  }

  HTML += "<p>turn off the ESP32</p><a class=\"button button-off\" href=\"/deepsleep\">Shutdown</a>\n";

  HTML += "</body>\n";
  HTML += "</html>\n";

  return HTML;
}

void displayLoop()
{
  tft.fillScreen(TFT_BLACK); // background black

  tft.setCursor(0, 0, 2); // curser 0,0 font 2
  tft.setTextColor(TFT_WHITE);
  tft.setTextSize(1);
  tft.println("  IoT course by:");
  tft.println();
  tft.println("Abdullah Al-Daigani");
  tft.println("         &");
  tft.println("Bader Al-Dakhil Allah");
  tft.println();
  tft.println();
  tft.print("ip: ");
  tft.println(WiFi.localIP());

  tft.setTextSize(2);

  if (LED1status)
  {
    tft.setCursor(180, 30, 2);
    tft.setTextColor(TFT_BLACK, TFT_GREEN);
    tft.println("on");
  }
  else
  {
    tft.setCursor(175, 30, 2);
    tft.setTextColor(TFT_BLACK, TFT_RED);
    tft.println("off");
  }

  if (LED2status)
  {
    tft.setCursor(180, 70, 2);
    tft.setTextColor(TFT_BLACK, TFT_GREEN);
    tft.println("on");
  }
  else
  {
    tft.setCursor(175, 70, 2);
    tft.setTextColor(TFT_BLACK, TFT_RED);
    tft.println("off");
  }

  yield(); // We must yield() to stop a watchdog timeout.
}