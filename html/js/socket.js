  // 网络通信代码
  var socket;

  var ws = new WebSocket("ws://127.0.0.1:9002");

  socket = ws;

  ws.onopen = function () {
      console.log('连接成功');

  };

  var roads;
  ws.onmessage = function (evt) {
      source1.clear();
      roads = [];
      var received_msg = evt.data.split("road:");
      for (var i = 0; i < received_msg.length; i++) {
          if (received_msg[i] != "") {
              var strRoad = received_msg[i].split(',');
              var road = [];
              for (var j = 0; j < strRoad.length; j++) {
                  if (strRoad[j] != "") {
                      road.push(parseInt(strRoad[j]));
                  }
              }
              roads.push(road)
          }
      }
    initRoadObject(roads, message);
  };

  ws.onclose = function () {
      alert("断开了连接");
  };

  document.getElementById("btn_submit").addEventListener('click', function () {
      socket.send(message);
      overlay.setPosition(undefined);
  }, false);