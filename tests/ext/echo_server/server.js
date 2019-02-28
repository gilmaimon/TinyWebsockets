var port = 8080,
  WebSocketServer = require('ws').Server,
  wss = new WebSocketServer({ port: port });

console.log('listening on port: ' + port);

wss.on('connection', function connection(ws) {
  ws.on('message', function(message) {
    ws.send('ECHO: ' + message);
  });

    console.log('new client connected!');
});
