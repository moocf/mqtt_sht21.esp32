var $rh = document.querySelector('#rh');
var $temp = document.querySelector('#temp');
var $wifi = document.querySelector('#wifi form');
var $ssid = document.querySelector('#ssid');
var $password = document.querySelector('#password');
var $mqtt = document.querySelector('#mqtt form');
var $uri = document.querySelector('#uri');


function request(method, url, data, fn) {
  var xhr = new XMLHttpRequest();
  xhr.onload = function() {
    fn(JSON.parse(this.responseText));
  };
  xhr.open(method, url);
  xhr.setRequestHeader('Content-Type', 'application/json;charset=UTF-8');
  xhr.send(data? JSON.stringify(data):data);
}

setInterval(() => {
  request('GET', '/sht21', null, (res) => {
    $rh.value = res.rh;
    $temp.value = res.temp;
  });
}, 5000);
request('GET', '/wifi_config_sta', null, (res) => {
  $ssid.value = res.ssid;
  $password.value = res.password;
});
request('GET', '/mqtt_config', null, (res) => {
  $url.value = res.uri;
});
