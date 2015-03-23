var map = null;

function switchToMap() {
  $( "#videoin" ).css("display", "none")
  initialise();
  
}

function initialise() {
  var mapOptions = {
    center: { lat: 53, lng: -6},
    zoom: 8
  };
  map = new google.maps.Map(document.getElementById('map-canvas'), mapOptions);
}
//google.maps.event.addDomListener(window, 'load', initialize);