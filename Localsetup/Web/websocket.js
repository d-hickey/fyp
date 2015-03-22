var locx = 170;
var locy = 115;
var size = 12000;

function dataURItoBlob(dataURI) {
    // convert base64/URLEncoded data component to raw binary data held in a string
    var byteString;
    if (dataURI.split(',')[0].indexOf('base64') >= 0)
        byteString = atob(dataURI.split(',')[1]);
    else
        byteString = unescape(dataURI.split(',')[1]);

    // separate out the mime component
    var mimeString = dataURI.split(',')[0].split(':')[1].split(';')[0];

    // write the bytes of the string to a typed array
    var ia = new Uint8Array(byteString.length);
    for (var i = 0; i < byteString.length; i++) {
        ia[i] = byteString.charCodeAt(i);
    }

    return new Blob([ia], {type:mimeString});
}

var ws = new WebSocket("ws://127.0.0.1:8089", "echo-protocol");
ws.onopen = function () {
  console.log("Opened connection to websocket");
}
  
timer = setInterval(
  function () {
    ctx.drawImage(video, 0, 0, 320, 240);
    var data = canvas.get()[0].toDataURL('image/jpeg', 1.0);
    var blob = dataURItoBlob(data);
    ws.send(blob);
  }, 500);
    
ws.addEventListener("message", function(e) {
  // The data is the head location and size
  var loc = e.data.split(",");
  console.log(e);
  console.log(loc);
  
  var zum = map.zoom;
  var lat = map.center.lat();
  var lng = map.center.lng();
  if((loc[0] - locx) > 30){
    lng = lng - 1;
  }
  else if((locx - loc[0]) > 30){
    lng = lng + 1;
  }
  
  if((loc[1] - locy) > 30){
    lat = lat - 1;
  }
  else if((locy - loc[1]) > 30){
    lat = lat + 1;
  }
  
  if((loc[2] - size) > 3000){
    zum = zum + 1;
  }
  else if((size - loc[2]) > 3000){
    zum = zum - 1;
  }
  
  map.setZoom(zum);
  map.setCenter({lat: lat, lng: lng});
  
});