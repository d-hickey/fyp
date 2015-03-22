var map = null;
function initialize() {
  var mapOptions = {
    center: { lat: 53, lng: -6},
    zoom: 8
  };
  map = new google.maps.Map(document.getElementById('map-canvas'), mapOptions);
}
google.maps.event.addDomListener(window, 'load', initialize);