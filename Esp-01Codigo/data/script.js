const TemObj = document.getElementById("TemDht");
const UmiObj = document.getElementById("UmiDht");
const SolObj = document.getElementById("Sol");
const ChuObj = document.getElementById("Chu");

var recording = false;
var datas = [];

var connection = new WebSocket('ws://' + location.hostname + ':81/', ['arduino']);
connection.onopen = function () {
  connection.send('Connect ' + new Date());
};
connection.onerror = function (error) {
  console.log('WebSocket Error ', error);
};
connection.onmessage = function (e) {
  console.log('Server: ', e.data);
  if(e.data == ".") {
    recording = true;    
  }
  else if(e.data == ":") {
    recording = false;
    TemObj.innerHTML = "Temperatura: " + datas[0] + "°C";
    UmiObj.innerHTML = "Umidade: " + datas[1] + "%";
    if(datas[2] == 1) {
      SolObj.innerHTML = "Solo: Seco";
    } else {
      SolObj.innerHTML = "Solo: Molhado";
    }
    if(datas[3] == 1) {
      SolObj.innerHTML = "Chuva: Não";
    } else {
      SolObj.innerHTML = "Chuva: Sim";
    }
  } else if(recording) {
    datas.push(e.data);
  }
};
connection.onclose = function () {
  console.log('WebSocket connection closed');
};

function mandarSinal(string) {
  connection.send(string);
}