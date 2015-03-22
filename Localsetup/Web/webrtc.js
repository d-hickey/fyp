function test(){
  map.setCenter({lat:53.3, lng:-6})
  map.setZoom(10)
}
    
var video = $("#live").get()[0];
var canvas = $("#canvas");
var ctx = canvas.get()[0].getContext('2d');

var errorCallback = function(e) {
  console.log('Error in loading video', e);
};

navigator.getUserMedia  = navigator.getUserMedia ||
                          navigator.webkitGetUserMedia ||
                          navigator.mozGetUserMedia ||
                          navigator.msGetUserMedia;


if (navigator.getUserMedia) {
  navigator.getUserMedia({video: true}, function(stream) {
    video.src = window.URL.createObjectURL(stream);
  }, errorCallback);
} else {
  alert('Your browser does not support getUserMedia()');
  video.src = 'dance.mp4'; // fallback.
}