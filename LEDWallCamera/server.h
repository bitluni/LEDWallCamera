#pragma once
#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <WiFiAP.h>

const char* ssid = "LEDWall";
const char* password = "wallmeup";

void calibrateWall();

WebServer server(80);

void handleRoot() 
{
  server.send(200, "text/html", "<meta name='viewport' content='width=device-width, initial-scale=1.0, user-scalable=no'><a href='/calibrate'>Calibrate Wall</a><br>"
  "<a href='/0'>Video</a><br>"
  "<a href='/1'>Difference</a><br>"
  "<a href='/2'>Distance</a><br>"
  "<a href='/3'>Pulsing Shape</a><br>"
  "<a href='/4'>Flames</a><br>"
  "<a href='/5'>Shadow</a>");
}

void initServer()
{
  WiFi.softAP(ssid, password);
  IPAddress myIP = WiFi.softAPIP();
  Serial.println("");
  Serial.print("IP address: ");
  Serial.println(WiFi.softAPIP());

  server.on("/", handleRoot);
  server.on("/calibrate", []() 
  {
    calibrateWall();
    handleRoot();
  });
  server.on("/0", []() 
  {
    setEffect(0);
    handleRoot();
  });
  server.on("/1", []() 
  {
    setEffect(1);
    handleRoot();
  });
  server.on("/2", []() 
  {
    setEffect(2);
    handleRoot();
  });
  server.on("/3", []() 
  {
    setEffect(3);
    handleRoot();
  });
  server.on("/4", []() 
  {
    setEffect(4);
    handleRoot();
  });
  server.on("/5", []() 
  {
    setEffect(5);
    handleRoot();
  });
  server.begin();
  Serial.println("HTTP server started");
}

void handleServer()
{
  server.handleClient();
}
