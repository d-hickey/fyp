var locx, locy;
var size;
var eye1x, eye1y, eye2x, eye2y;

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

var ws = new WebSocket("ws://darragh-fyp.netsoc.tcd.ie:8089", "echo-protocol");
ws.onopen = function () {
  console.log("Opened connection to websocket");
}
  
timer = setInterval(
  function () {
    ctx.drawImage(video, 0, 0, 640, 480);
    var data = canvas.get()[0].toDataURL('image/jpeg', 1.0);
    var blob = dataURItoBlob(data);
    ws.send(blob);
  }, 1000);
    
ws.addEventListener("message", function(e) {
  // The data is the head location and size
  var loc = e.data.split(",");
  //console.log(e);
  console.log(loc);
  //loc is [face-pos-x, face-pos-y, face-size, eye-1-pos-x, eye-1-pos-y, eye-2-pos-x, eye-2-pos-y]
  if(map == null && loc[0] != 0){
    locx = loc[0];
    locy = loc[1];
    size = loc[2];
    if(loc[3] < loc[5]){
      eye1x = loc[3];
      eye1y = loc[4];
      eye2x = loc[5];
      eye2y = loc[6];
    }
    else{
      eye1x = loc[5];
      eye1y = loc[6];
      eye2x = loc[3];
      eye2y = loc[4];
    }
  }
  else if(loc[0] != 0){
    var zum = map.zoom;
    var lat = map.center.lat();
    var lng = map.center.lng();
    //update zoom
    if((loc[2] - size) > 10000){
      zum = zum + 1;
    }
    else if((size - loc[2]) > 10000){
      zum = zum - 1;
    }
    //update latitude and longitude
    if(eye1x != 0 && loc[3] != 0){
      var i1x, i1y, i2x, i2y;
      if(loc[3] < loc[5]){
        i1x = loc[3];
        i1y = loc[4];
        i2x = loc[5];
        i2y = loc[6];
      }
      else{
        i1x = loc[5];
        i1y = loc[6];
        i2x = loc[3];
        i2y = loc[4];
      }
      if(((loc[0] - locx) > 10) && ((i1x - eye1x) > 20)){
        lng = lng - (90 / Math.pow(2, (zum-2)));
      }
      else if(((locx - loc[0]) > 10) && ((eye1x - i1x) > 20)){
        lng = lng +  (90 / Math.pow(2, (zum-2)));
      }
      if(((loc[1] - locy) > 10) && ((i1y - eye1y) > 10)){
        lat = lat - (90 / Math.pow(2, (zum-2)));
      }
      else if(((locy - loc[1]) > 10) && ((eye1y - i1y) > 10)){
        lat = lat + (90 / Math.pow(2, (zum-2)));
      }
    }
    else{
      if((loc[0] - locx) > 30){
        lng = lng - (90 / Math.pow(2, (zum-2)));
      }
      else if((locx - loc[0]) > 30){
        lng = lng +  (90 / Math.pow(2, (zum-2)));
      }
      if((loc[1] - locy) > 30){
        lat = lat - (90 / Math.pow(2, (zum-2)));
      }
      else if((locy - loc[1]) > 30){
        lat = lat + (90 / Math.pow(2, (zum-2)));
      }
    }
    
    map.setZoom(zum);
    console.log(lng + " " + lat + " " + zum);
    map.setCenter({lat: lat, lng: lng});
  }
  
});