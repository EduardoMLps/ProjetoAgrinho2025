const TemObj = document.getElementById("TemDht");
const UmiObj = document.getElementById("UmiDht");
const SolObj = document.getElementById("Sol");
const ChuObj = document.getElementById("Chu");

const Canvas1 = document.getElementById("UmiCanvas");
const Canvas2 = document.getElementById("TemCanvas");
//const Canvas3 = document.getElementById("SolCanvas");
//const Canvas4 = document.getElementById("ChuCanvas");
const ctx1 = Canvas1.getContext("2d");
const ctx2 = Canvas2.getContext("2d");
//const ctx3 = Canvas3.getContext("2d");
//const ctx4 = Canvas4.getContext("2d");

const timeOnLoaded = Date.now();
var recording = false;
var datas = [
  
];

//Canvas

const marginLeft = 30;
const marginRight = 50;
const marginBottom = 10;

function resetAllCanvas() {
  setGraphicCanvas(Canvas1, ctx1, 6, 15, 20, "%", "s", 2);
  setGraphicCanvas(Canvas2, ctx2, 8, 15, 5, "C°", "s", 1);
}

function setGraphicCanvas(canvas, ctx, rows, columns, res, char1, char2, id) {
  ctx.clearRect(0,0,canvas.width, canvas.height)
  //set the grid
  ctx.font = '10px Arial'
  ctx.beginPath();
  for(var i = 1; i<=rows; i++) {
    var rowHeight = i/rows * canvas.height - marginBottom;
    ctx.strokeStyle = "#000";
    ctx.textAlign = "start";
    fillStyle = "#000";
    ctx.fillText(String(res * (rows - i)) + char1, 0, rowHeight + 5);
    
    ctx.strokeStyle = "#ddd";
    ctx.moveTo(0 + marginLeft, rowHeight);
    ctx.lineTo((canvas.width - marginRight) + marginLeft, rowHeight);
    ctx.stroke();
  }
  for(var i = 0; i<columns; i++) {
    var dataTime = columns - i;
    var dataInfo = 0;
    var dataInfoPos = valueYPosition(0, canvas, rows, res);
    if(datas[(datas.length - columns) + i - 1]) {
      //dataTime = datas[columns - i - 1][0];
      dataInfo = datas[(datas.length - columns) + i - 1][id];
      dataInfoPos = valueYPosition(dataInfo, canvas, rows, res);
    }
    var radius = 4;
    
    var columPos = valueXPosition(i, canvas, columns);
    
    ctx.textAlign = "center";
    ctx.fillStyle = "#000";
    if(dataTime) {
      ctx.fillText(String(dataTime) + char2, columPos, canvas.height);
    } else {
      ctx.fillText(String(0) + char2, columPos, canvas.height);
    }
    
    ctx.strokeStyle = "#ddd"
    ctx.moveTo(columPos, 0);
    ctx.lineTo(columPos, canvas.height - marginBottom);
    
    ctx.fillStyle = "#4ba8fa"
    ctx.moveTo(columPos + radius, dataInfoPos);
    ctx.arc(columPos, dataInfoPos, radius, 0, Math.PI * 2);
    ctx.fill();
    ctx.fillStyle = "#000"
    ctx.stroke();
  }
  ctx.closePath();

  ctx.beginPath();
  ctx.moveTo(valueXPosition(0, canvas, columns), valueYPosition(0, canvas, rows, res));
  ctx.strokeStyle = "#4ba8fa";
  for(var i = 0; i<columns; i++) {
    //get data value.
    var data = 0;
    if(datas[(datas.length - columns) + i - 1]) {
      data = datas[(datas.length - columns) + i - 1][id];
    }

    //get corresponding position.
    var Xpos = valueXPosition(i, canvas, columns);
    var Ypos = valueYPosition(data, canvas, rows, res);

    //draw the line.
    //if(i = 0) {
    //  ctx.moveTo(valueXPosition(Xpos, canvas, columns), valueYPosition(Ypos, canvas, rows, res));
    //}
    ctx.lineTo(Xpos, Ypos);
    ctx.stroke();
  }
  ctx.closePath();
}

function valueYPosition(data, canvas, rows, res) {
  var val = (canvas.height - marginBottom) - data * canvas.height/rows/res;
  return val;
}
function valueXPosition(data, canvas, columns) {
  var val = data/columns * (canvas.width - marginRight) + marginLeft;
  return val;
}

resetAllCanvas();

//ESP connection

var newArray = [];

var connection = new WebSocket('ws://' + location.hostname + ':81/', ['arduino']);
connection.onopen = function () {
  connection.send('Connect ' + new Date());
};
connection.onerror = function (error) {
  console.log('WebSocket Error ', error);
};
connection.onmessage = function (e) {
  //array para o display
  console.log(e.data);
  //sinal para começar a gravar o que recebe do esp
  if(e.data == ".") {
    recording = true;
    newArray.map(parseFloat);
    newArray.push(Date.now());
  }

  //caso termine de gravar então mostrar a informação
  else if(e.data == ":") {
    recording = false;
    //console.log(newArray);
    datas.push(newArray);
    TemObj.innerHTML = "Temperatura: " + newArray[1] + "°C";
    UmiObj.innerHTML = "Umidade: " + newArray[2] + "%";
    if(newArray[3] == 1) {
      SolObj.innerHTML = "Solo: Seco";
    } else {
      SolObj.innerHTML = "Solo: Molhado";
    }
    if(newArray[4] == 1) {
      ChuObj.innerHTML = "Chuva: Não";
    } else {
      ChuObj.innerHTML = "Chuva: Sim";
    }
    resetAllCanvas();
    newArray = [];
  } 

  //caso começe a gravar então colocar no array
  else if(recording) {
    newArray.push(e.data);
  }
};
connection.onclose = function () {
  console.log('WebSocket connection closed');
};

function mandarSinal(string) {
  connection.send(string);
}
