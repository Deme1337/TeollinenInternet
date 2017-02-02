var net = require('net');
var app = require('http').createServer(handler);
var express = require('express');
var expApp = express();

var io = require('socket.io')(app);
var fs = require('fs');

var server = net.createServer();

var RaspData = [];
var index = 0; 

app.listen(8080);

function handler (req, res) {
  fs.readFile(__dirname + '/index.html',
  function (err, data) {
    if (err) {
      res.writeHead(500);
      return res.end('Error loading index.html');
    }

    res.writeHead(200);
    res.end(data);
  });
}

var bodyParser = require('body-parser');

expApp.use(bodyParser.json());
expApp.use(bodyParser.urlencoded({
  extended: false
}));

expApp.post('/rasp', function(req,res){
    if(index > 20)
      index = 0;
    
    console.log(req.body.body);
    
    
    RaspData[index] = req.body.body;
    index++;
});

expApp.listen(6540, function(){
  console.log("express listening to port 6540");
});


io.on('connection', function (socket) {
  socket.on('getdata', function(msg){
      //lähetetään takasin viimeisin kirjaus
      var dback1 = new Date().toString();
      if(index == 0)
        var dback2 = "No data";
      else
        var dback2 = RaspData[index-1].substring(0,2) + "°C";

      io.emit('databack1',dback1);
      io.emit('databack2',dback2);
  });



  socket.on('disconnect', function(){
    console.log("Someone disconnected");
  })

});


/*
//Kuuntelee socketia joka vastaaottaa dataa raspberryltä
server.on('connection', handleConnection);

server.listen(6540, function() {  
  console.log('server listening to %j', server.address());
});


function handleConnection(conn) {  
  var remoteAddress = conn.remoteAddress + ':' + conn.remotePort;
  console.log('new client connection from %s', remoteAddress);

  conn.on('data', onConnData);
  conn.once('close', onConnClose);
  conn.on('error', onConnError);

  function onConnData(d) {
    if(index > 20)
        index = 0;
    
    d = d.toString();
    RaspData[index] = d;
    index++;
    console.log('connection data from %s: %s', remoteAddress, d);
  }

  function onConnClose() {
    console.log('connection from %s closed', remoteAddress);
  }

  function onConnError(err) {
    console.log('Connection %s error: %s', remoteAddress, err.message);
  }
}*/