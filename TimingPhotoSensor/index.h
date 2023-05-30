const char MAIN_page[] PROGMEM = R"=====(
<!DOCTYPE html>
<html>
<style>
.card{
    max-width: 800px;
     min-height: 250px;
     background: #02b875;
     padding: 30px;
     box-sizing: border-box;
     color: #FFF;
     margin:20px;
     box-shadow: 0px 2px 18px -4px rgba(0,0,0,0.75);
}
</style>
<body>

<div class="card">
  <h1>SER Zeitmessung 12h-Fahrt Thun 2023</h1><br>
  <h2>Zeit aktuelle Runde: <span id="ActTime">0</span></h1><br>
  <h2>Anzahl Runden: <span id="RoundCnt">0</span></h1><br>
  <h2>gefahrene Distanz: <span id="Distance">0</span> km</h1><br>
  <h2>Zeit letzte Runde: <span id="LastTime">0</span></h1><br>
  <h2>Zeit schnellste Runde: <span id="FastTime">0</span></h1><br>
  <h2>gefahrene Zeit: <span id="Time">0</span></h1><br>
</div>
<script>

setInterval(function() {
  
  ActTimeActRound();
  getRoundCounter();
  getDistance();
  getLastTime();
  getFastTime();
  getTime();
  
}, 450); //ms update rate

function ActTimeActRound() {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("ActTime").innerHTML =
      this.responseText;
    }
  };
  xhttp.open("GET", "readActTime", true);
  xhttp.send();
}

function getRoundCounter() {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("RoundCnt").innerHTML =
      this.responseText;
    }
  };
  xhttp.open("GET", "readRoundCnt", true);
  xhttp.send();
}

function getDistance() {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("Distance").innerHTML =
      this.responseText;
    }
  };
  xhttp.open("GET", "readDistance", true);
  xhttp.send();
}

function getLastTime() {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("LastTime").innerHTML =
      this.responseText;
    }
  };
  xhttp.open("GET", "readLastTime", true);
  xhttp.send();
}

function getFastTime() {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("FastTime").innerHTML =
      this.responseText;
    }
  };
  xhttp.open("GET", "readFastTime", true);
  xhttp.send();
}

function getTime() {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("Time").innerHTML =
      this.responseText;
    }
  };
  xhttp.open("GET", "readTime", true);
  xhttp.send();
}

</script>
</body>
</html>
)=====";
