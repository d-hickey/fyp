var http = require('http');
var server = http.createServer(function (req, res) {
  res.writeHead(200, {'Content-Type': 'text/plain'});
  res.end('Hello Node.js\n');
})
server.listen(8089, "dar-fyp.netsoc.tcd.ie");


var WebSocketServer = require('websocket').server;

wsServer = new WebSocketServer({
    httpServer: server,
    maxReceivedFrameSize: 400000,
    maxReceivedMessageSize: 400000
});

var count = 0;
var clients = {};

wsServer.on('request', function(r){
    var connection = r.accept('echo-protocol', r.origin);
    // Specific id for this client & increment count
    var id = count++;
    // Store the connection method so we can loop through & contact all clients
    clients[id] = connection
    console.log((new Date()) + ' Connection accepted [' + id + ']');
    
    connection.on('message', function(data) {

      //console.log(data);
      //console.log(data.binaryData)
      var base = Math.random().toString(36).substring(7);
      var filename = "media/" + base + ".jpeg";
      var outfile = "out/" + base + ".jpg";
      var fs = require('fs');
      fs.writeFile(filename, data.binaryData, function(err) {
        if(err) {
          console.log(err);
        } else {
          //console.log("The file was saved!");
          var childProcess = require('child_process');
          var command = "a.out " + filename + " " + outfile;
          //var retstring = "";
          var face = childProcess.exec(command, function (error, stdout, stderr) {
            if (error) {
              console.log(error.stack);
              console.log('Error code: '+error.code);
              console.log('Signal received: '+error.signal);
            }
            if(clients[id]){

              clients[id].sendUTF(stdout);
            }
            console.log('Child Process STDOUT: '+stdout);
            console.log('Child Process STDERR: '+stderr);
          });

          face.on('exit', function (code) {
            console.log('Child process exited with exit code '+code);
            /*if(clients[id]){
              console.log(retstring);
              clients[id].sendUTF(retstring);
            }*/
            fs.unlink(outfile, function (err){
              if (err) throw err;
            });
            fs.unlink(filename, function (err) {
              if (err) throw err;
            });
            
            
          });
        }
      });
    });
    
    connection.on('close', function(reasonCode, description) {
      delete clients[id];
      console.log((new Date()) + ' Peer ' + connection.remoteAddress + ' disconnected. ' + reasonCode + " " + description);
    });
});
console.log('Server running at http://darragh-fyp.netsoc.tcd.ie:8089/');

