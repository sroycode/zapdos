<html>
	<head>
		<title>JdMaps Example</title>
		<meta charset="utf-8">
		<meta http-equiv="X-UA-Compatible" content="IE=edge">
		<meta name="viewport" content="width=device-width, initial-scale=1.0, maximum-scale=1.0, user-scalable=no" />
		<link rel="stylesheet" href="http://cdn.jsdelivr.net/leaflet/0.7.3/leaflet.css" type="text/css">
		<link rel="stylesheet" href="pano.css" type="text/css">
		<link rel="stylesheet" href="page.css" type="text/css">
		<script src="//cdn.jsdelivr.net/leaflet/0.7.3/leaflet.js"></script>
		<script type="text/javascript" src="pano.js"></script>
		<script>
    API_URL = '//YOUR_WEBSITE/api/?';
    TILELAYER = '//a.mtiles.jdmaps.com/tiles/{z}/{x}/{y}.png';
    CENTER = [13.039680,77.580214];
    MAXZOOM = 18;
		var searchPoints = L.geoJson(null, {
				onEachFeature: function (feature, layer) {
					layer.bindPopup(feature.properties.name);
				}
			});
		function showSearchPoints (geojson) {
			searchPoints.clearLayers();
			searchPoints.addData(geojson);
		}
		function init() {
			var map = L.map('map', {scrollWheelZoom: false, zoomControl: false, panoControl: true, panoControlOptions: {resultsHandler: showSearchPoints, placeholder: 'Try me…', position: 'topleft', url: API_URL}});
			map.setView(CENTER, 12);
			searchPoints.addTo(map);
			var tilelayer = L.tileLayer(TILELAYER, {maxZoom: MAXZOOM, tms: true}).addTo(map);
			var zoomControl = new L.Control.Zoom({position: 'topright'}).addTo(map);
		}
		</script>
	</head>
	<body onload="init()">
		<div id="map" style="width:500px;height:500px;float:left;margin-left:20px;margin-top:20px">
		</div>
	</body>
</html>

