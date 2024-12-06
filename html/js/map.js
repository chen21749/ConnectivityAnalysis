var message;
//---地图默认操作加载
const map = new ol.Map({
    layers: [
        new ol.layer.Tile({
            source: new ol.source.OSM({
                projection: 'EPSG:4326',
                url: '//{a-d}.basemaps.cartocdn.com/light_all/{z}/{x}/{y}.png',
              }),
            opacity: 0.4
        }),
    ],
    target: 'map',
    view: new ol.View({
        projection: 'EPSG:4326',
        center: [116.40174080009785, 39.89128889996283],
        zoom: 12,
    }),
});

let source = new ol.source.Vector({
    features: new ol.format.GeoJSON().readFeatures(road)
})

var geoLayer = new ol.layer.Vector({
    source: source,
    // 设置样式，边框和填充
    style: new ol.style.Style({
        stroke: new ol.style.Stroke({
            color: 'rgba(0, 205, 205, 1)',
            width: 2
        })
    }),
})

let source1 = new ol.source.Vector({

})
var geoLayer1 = new ol.layer.Vector({
    source: source1,
    // 设置样式，边框和填充
    style: new ol.style.Style({
        stroke: new ol.style.Stroke({
            color: 'rgba(255, 0, 0, 1)',
            width: 2
        })
    }),
})
// 添加GeoJSON到map上
 // 添加GeoJSON到map上
map.addLayer(geoLayer)
map.addLayer(geoLayer1)


//---地图默认操作加载---end


//读取节点数据，后面可优化到C++操作
let serchSource = new ol.source.Vector({
    features: new ol.format.GeoJSON().readFeatures(roadPoint)
})

let serchVector = new ol.layer.Vector({
    source: serchSource,
    style: new ol.style.Style({
        image: new ol.style.Circle({
            radius: 3,
            fill: new ol.style.Fill({
                color: 'rgba(147, 112, 219, 1)',
            })
        })
    }),
})
map.addLayer(serchVector);



const container = document.getElementById('popup');
const content = document.getElementById('popup-content');
const closer = document.getElementById('popup-closer');
const overlay = new ol.Overlay({
    element: container,
    autoPan: {
      animation: {
        duration: 250,
      },
    },
  });

  closer.onclick = function () {
    overlay.setPosition(undefined);
    closer.blur();
    return false;
  };

map.addOverlay(overlay);

map.on('click', function(evt) {
    var coordinate = evt.coordinate; 
  var feature = map.forEachFeatureAtPixel(evt.pixel, function(feature, layerVetor) { return feature; });
   if (feature) { //捕捉到要素
       var id = feature.getProperties().ID;
       if(id != undefined){
        message = id;
        overlay.setPosition(coordinate);
       }
   }
 })






var roadObjectArray;//存取路线对象信息，初始化为全局
function initRoadObject(roads, startPoint){
      roadObjectArray = [];
      //通过id计算经纬度点
      for (var i = 0; i < roads.length; i++) {
          var temp = [];
          for (var j = 0; j < roads[i].length; j++) {
              temp.push(getCoordinateByID(roads[i][j]));
          }
          var obj = {
              "id":roads[i][0],//线路唯一标识
              "ids":roads[i],
              "oricoordinate":temp,
              "flatcoodinate":[],
              "idinflatindex":[],
              "iscomplete":false,
              "ismoving":false,
              "moveindex":0,
              "index":0,
              "feature":null,
              "child":[]
          }
          roadObjectArray.push(obj);
      }
    
    //平滑点以及是否是完整路线
    for(var i = 0; i < roadObjectArray.length; i++){//平滑点
        var flatcoodinate = [roadObjectArray[i].oricoordinate[0]];
         
        var idinflatindex = 0;
        roadObjectArray[i].idinflatindex.push(idinflatindex);//记录原始点在平滑点中的位置

        for(var j = 0; j < roadObjectArray[i].oricoordinate.length - 1; j++){
            var flatArray = flatCoordinate(roadObjectArray[i].oricoordinate[j], roadObjectArray[i].oricoordinate[j+1], 0.00001);
            idinflatindex += flatArray.length;
            roadObjectArray[i].idinflatindex.push(idinflatindex);

            flatcoodinate = flatcoodinate.concat(flatArray);
        }
        roadObjectArray[i].flatcoodinate = flatcoodinate;

        //是否是完整路线
        if(roadObjectArray[i].ids[0] == startPoint){
            roadObjectArray[i].iscomplete = true;
            roadObjectArray[i].ismoving = true;//完整路线可以动
        }
    }

    //查询子线路
    //step1:查询出不完整路线
    var incompleteRoadId = [];
    for(var i = 0; i < roadObjectArray.length; i++){
       if(roadObjectArray[i].iscomplete == false){
         incompleteRoadId.push(roadObjectArray[i].id);
       }
    }

    //step2:不完整的线是谁的子路线
    for(var i = 0; i < roadObjectArray.length; i++){
        for(var j = 0; j < roadObjectArray[i].ids.length; j++){
             var currentId = roadObjectArray[i].ids[j];
             for(var k = 0; k < incompleteRoadId.length; k++){
                if(currentId == incompleteRoadId[k] && currentId != roadObjectArray[i].id){//是他的子节点
                    roadObjectArray[i].child.push(incompleteRoadId[k]);
                }
             }
        }

     }
    
    cereateGemetry();
}

function getCoordinateByID(id) {
    for (var i = 0; i < serchSource.getFeatures().length; i++) {
        if (serchSource.getFeatures()[i].getProperties().ID == id) {
           return serchSource.getFeatures()[i].getGeometry().flatCoordinates;
        }
    }
}

function cereateGemetry(){

    for(var i = 0; i < roadObjectArray.length; i++){
        var gemo = new ol.geom.LineString([]);
        var feature = new ol.Feature({
            geometry:gemo
        });

        roadObjectArray[i].feature = feature;
        source1.addFeature(roadObjectArray[i].feature);
    }

    //动画效果
    map.on('postrender', moveFeature);
    map.render();
}


//动画开始
let speed = 0;
var animation = true;
function moveFeature(event) {
    if(speed % 1 == 0){
                // //单线测试
                // debugger
                // roadObjectArray[2].moveindex = roadObjectArray[2].index % roadObjectArray[2].flatcoodinate.length;
                // var currentPath = roadObjectArray[2].flatcoodinate.slice(0, roadObjectArray[2].moveindex+1);
                // roadObjectArray[2].feature.getGeometry().setCoordinates(currentPath);
                // roadObjectArray[2].index++;
        
        for(var i = 0; i<roadObjectArray.length; i++){
            //是否能动
            if(roadObjectArray[i].ismoving){
                roadObjectArray[i].moveindex = roadObjectArray[i].index % roadObjectArray[i].flatcoodinate.length;
                var currentPath = roadObjectArray[i].flatcoodinate.slice(0, roadObjectArray[i].moveindex+1);
                roadObjectArray[i].feature.getGeometry().setCoordinates(currentPath);
                
                //是否有子路线
                if(roadObjectArray[i].child.length != 0){
                   for(var j=0; j<roadObjectArray[i].child.length;j++){
                       
                       var idsIndex = roadObjectArray[i].ids.indexOf(roadObjectArray[i].child[j]);
                       var flatIndex = roadObjectArray[i].idinflatindex[idsIndex]//子路线在什么时候分支
                       if(roadObjectArray[i].moveindex == flatIndex){//走到这里了;
                             for(var k = 0; k<roadObjectArray.length; k++){
                                  if(roadObjectArray[i].child[j] == roadObjectArray[k].id){
                                    roadObjectArray[k].ismoving = true;
                                  }
                             }
                       }
                   }
                }
                if(roadObjectArray[i].moveindex == roadObjectArray[i].flatcoodinate.length-1){
                    roadObjectArray[i].ismoving = false;//停止
                }
                roadObjectArray[i].index++;
            }
        }
        
        //检查有无动画
        var stop = true;
        for(var i = 0; i<roadObjectArray.length; i++){
            if(roadObjectArray[i].ismoving == true){
                stop = false;
            }
        }
        if(stop){
            map.un('postrender', moveFeature);
        }
    }
    speed++;
    map.render();
}
//动画--end



//平滑点: 间隔distance，这个距离是笛卡尔坐标系距离
function flatCoordinate(start, end, deltaDis){
    dx = Math.abs(start[0] - end[0]);
    dy = Math.abs(start[1] - end[1]);
    var count = Math.trunc(Math.sqrt(Math.pow(dx,2)+Math.pow(dy,2)) / deltaDis);
    return addLdPoint(start, end, count);
}


//计算点(计算两点之间)不包括起点，但包括终点
function addLdPoint(pointFist, pointLast, count) {//(目标数组、目标数组长度、返回数组、之间点的个数)
    var array = [];
    var absoluteX = parseFloat(pointLast[0]) - parseFloat(pointFist[0]);
    var absoluteY = parseFloat(pointLast[1]) - parseFloat(pointFist[1]);
    for (var k = 1,n = 0; k<=count; k++,n++) {
        var lon = parseFloat(pointFist[0]) + absoluteX * (k / count);
        var lat = parseFloat(pointFist[1]) + absoluteY * (k / count);
        array[n] = [lon, lat];
    }
    return array;
}


