//IoT Motor Control
//Author: Royston Sanctis
//Date: 08 Feb 2023
//Email: roystonsanctis@gmail.com

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

const char* ssid = "Rebecca";
const char* password = "Roy@2001";

const int speedPin = D1;
const int delayPin = D2;
const int onDurationPin = D3;
const int offDurationPin = D4;

const int motorPin = D1;
int onTime = 0;
int offTime = 0;
bool isRunning = false;

ESP8266WebServer server(80);

//=========================================
//HTML + CSS + JavaScript codes for webpage
//=========================================
const char webpageCode[] =
  R"=====(
<!DOCTYPE html>
<html>
<!------------------------------C S S--------------------------------->
<head>
 <title>DC Motor Control</title>
    <style>
       #control-panel {
  width: 400px;
  margin: 0 auto;
  text-align: center;
}

.form-group {
  margin-bottom: 20px;
}

label {
  display: block;
  font-weight: bold;
  margin-bottom: 10px;
}
    </style>
</head>
<!----------------------------H T M L--------------------------------->
<body>
    
      
    <div id="control-panel">
  <h2>DC Motor Control</h2>
  <div class="form-group">
    <label for="speed-slider"></label>
    <p><strong>Speed:</strong> <span id="speed-display">50</span> %</p>
    <input type="range" min="0" max="100" value="50" class="form-control" id="speed-slider">
  </div>
  <div class="form-group">
    <label for="on-duration">ON Duration (sec):</label>
    <input type="number" min="0" value="5" class="form-control" id="on-duration">
  </div>
  <div class="form-group">
    <label for="off-duration">OFF Duration (sec):</label>
    <input type="number" min="0" value="5" class="form-control" id="off-duration">
  </div>
  <div class="form-group">
    <label for="delay">Delay (sec):</label>
    <input type="number" min="0" value="0" class="form-control" id="delay">
  </div>
  <button id="start-button" class="btn btn-primary">Start</button>
  <button id="stop-button" class="btn btn-danger">Stop</button>
</div>
<!---------------------------JavaScript------------------------------->
<script>
    let speed = document.querySelector("#speed-slider").value;
  let onDuration = document.querySelector("#on-duration").value;
  let offDuration = document.querySelector("#off-duration").value;
  const speedDisplay = document.getElementById("speed-display");
  const speedSlider = document.getElementById("speed-slider");
  let delay = document.querySelector("#delay").value;

  let isRunning = false;
  let intervalId;
  
  speedSlider.addEventListener("input", function() {
  speedDisplay.innerHTML = this.value;
});

  document.querySelector("#speed-slider").addEventListener("input", function() {
    speed = this.value;
  });

  document.querySelector("#on-duration").addEventListener("input", function() {
    onDuration = this.value;
  });

  document.querySelector("#off-duration").addEventListener("input", function() {
    offDuration = this.value;
  });

  document.querySelector("#delay").addEventListener("input", function() {
    delay = this.value;
  });

  document.querySelector("#start-button").addEventListener("click", function() {
    if (!isRunning) {
      isRunning = true;
      intervalId = setInterval(function() {
        // Send fetch request to control the DC motor
        fetch("/control-dc-motor", {
          method: "POST",
          headers: {
            "Content-Type": "application/json"
          },
          body: JSON.stringify({
            speed,
            onDuration,
            offDuration,
            delay
          })
        });
      }, delay);
    }
  });
</script>
</body>
</html>
)=====";



void handleRoot() {
  String html = "<html><head><style>button{width:100px;height:50px;margin:20px;}</style></head><body><center>";
  html += "<h1>DC Motor Control</h1>";
  html += "<form action='/motor'><button name='state' value='start'>Start</button><button name='state' value='stop'>Stop</button><br><br>";
  html += "On Time (s): <input type='text' name='on_time'><br><br>";
  html += "Off Time (s): <input type='text' name='off_time'><br><br>";
  html += "<button type='submit'>Submit</button></form>";
  html += "</center></body></html>";
  server.send(200, "text/html", webpageCode);
}

void handleMotor() {
  if (server.arg("state") == "start") {
    isRunning = true;
    onTime = server.arg("on_time").toInt();
    offTime = server.arg("off_time").toInt();
     Serial.print("ON time:");
     Serial.print(onTime);
     Serial.print("/tOFF time:");
     Serial.println(offTime);
     
  }
  if (server.arg("state") == "stop") {
    isRunning = false;
  }
  server.send(200, "text/plain", "Motor state changed.");
}

void setup() {
  Serial.begin(9600);
  pinMode(speedPin, OUTPUT);
  pinMode(delayPin, OUTPUT);
  pinMode(onDurationPin, OUTPUT);
  pinMode(offDurationPin, OUTPUT);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
  Serial.println("");
  Serial.println("WiFi connected..!");
  Serial.print("Got IP: ");
  Serial.println(WiFi.localIP());

  server.on("/", handleRoot);
  server.on("/motor", handleMotor);

  server.begin();
  Serial.println("Server started");

  // server.on("/control-dc-motor", []() {
  //   int speed = 0;
  //   int onDuration = 0;
  //   int offDuration = 0;
  //   int delay = 0;

  //   int speedStartIndex = server.arg("speed").toInt();
  //   int onDurationStartIndex = server.arg("onDuration").toInt();
  //   int offDurationStartIndex = server.arg("offDuration").toInt();
  //   int delayStartIndex = server.arg("delay").toInt();

  //   Serial.print("Delay:");
  //   Serial.println(delayStartIndex);

  //   analogWrite(speedPin, map(speedStartIndex, 0, 100, 0, 255));
  //   digitalWrite(delayPin, delayStartIndex > 0 ? HIGH : LOW);
  //   digitalWrite(onDurationPin, onDurationStartIndex > 0 ? HIGH : LOW);
  //   digitalWrite(offDurationPin, offDurationStartIndex > 0 ? HIGH : LOW);

  //   // server.send(200, "text/plain", "DC Motor Control Request Received");
  //   server.send(200, "text/html", webpageCode);
  // });
}

void loop() {
  server.handleClient();
}
