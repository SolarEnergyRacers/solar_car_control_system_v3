const char MAIN_page[] PROGMEM = R"=====(
<!DOCTYPE html>
<html>
<style>

* {
    font-family: Dosis, Arial, sans-serif;
}

/* Start RESPONSIVE DESIGN mit zwei Breaking-Points: bei 700px und 1024px */
/* Screens mit Bildschirmbreite < 700px  */
@media only screen and (max-width: 700px) {

    * {
        font-size: 0.98em;
    }

    .half-col {
        width: calc(100% - 22px);
        border: 1px solid blue;
        background: linear-gradient(rgba(0, 0, 250, 0.2), white);
        margin-top: 10px;
    }
}

/* Screens mit Bildschirmbreite > 1024px */
@media only screen and (min-width: 1024px) {

    body {
        max-width: 1100px;
        margin: auto;
    }

    .abschnitt {
        width: calc(50% - 32px);
        float: left;
    }
}

.Main {
    max-width: calc(100%);
    background-color: white;
    color: black;
    border: 0px solid #aaa;
    margin: 10px;
    padding: 30px;
    border-radius: 0.4em;
}

.half-col {
    width: calc(50% - 22px);
    min-height: 140px;
    float: left;
    border: green solid 0px;
    background: #eee;
    margin: 5px;
    padding: 5px;
}


.Data{
    max-width: 100%;
    background: #eee;
    padding: 30px;
    border-radius: 0.4em;
    box-sizing: border-box;
    color: #FF;
    border: 1px solid #aaa;
    margin:0px;
    box-shadow: 0px 2px 18px -4px rgba(0,0,0,0.2);
}
</style>
<body>

<div class="Main">
    
        <h1>SolarEnergyRacers</h1>
        <h2>Zeitmessung Rekordversuch Thun 18/06/2023 07:00 - 19:00 Uhr </h2>
               
                   
        </div>

        <div class="Data">
            <font size="5" face="Courier New" ><b>
                <table border="0" width=100% height="100">
                
				<tr>
                <td width=50% height="80"> Anzahl Runden: </td>
                <td width=50% height="80"> <span id="RoundCnt">0</span> <br> </td>
                </tr>
                
                <tr>
                <td width=50% height="80"> gefahrene Distanz: </td>
                <td width=50% height="80"> <span id="Distance">0</span> km<br> </td>
                </tr>
                
				<tr>
                <td width=50% height="80"> Zeit aktuelle Runde: </td>
                <td width=50% height="80"> <span id="ActTime">0</span> <br> </td>
                </tr>

                <tr>
                <td width=50% height="80"> gefahrene Zeit: </td>
                <td width=50% height="80"> <span id="Time">0</span><br> </td>
                </tr>
                
                <tr>
                <td width=50% height="80"> Zeit letzte Runde: </td>
                <td width=50% height="80"> <span id="LastTime">0</span><br> </td>
                </tr>
                
                <tr>
                <td width=50% height="80"> Zeit schnellste Runde: </td>
                <td width=50% height="80"> <span id="FastTime">0</span><br> </td>
                </tr>
                                
            </font></b>
            
            </table>
            
        </div>

        <div class="clearfix"></div>
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
