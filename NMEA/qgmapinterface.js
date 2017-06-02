var map = null;
var socket = null;
var connectedflag = false;
var listener = [];
var occupyflag = false;

var markers = [];
var polylines = [];
var polygons = [];

window.onload = function(){
    var CICLatlng = {lat: 40.443786, lng: -79.946549};
    var mapOptions = {
        center : CICLatlng,
        zoom : 15,
        draggableCursor: 'crosshair',
        draggingCursor: 'closed-handed'
    };
    map = new google.maps.Map(document.getElementById("map_canvas"),mapOptions);
}

function disconnect(){
    if(connectedflag){
        if(occupyflag){
            qgmapinterface.jslotReleaseEditor(getClientId());
        }
        qgmapinterface.jslotWebSocketDisconnected(getClientId());
    }
}

window.onbeforeunload = function() {
    disconnect();
    socket.close();
};

function getClientId(){
    return document.getElementById("clientid").value;
}

function isTarget(clientid){
    return clientid === "" || clientid === getClientId();
}

function getJsonMarker(marker){
    var jsonMarker={
        id: markers.indexOf(marker),
        label: marker.getLabel(),
        title: marker.getTitle(),
        position: marker.getPosition().toJSON()
    };
    return jsonMarker;
}

function getJsonMarkerConfig(marker){
    var jsonMarkerConfig={
        scale: marker.getIcon().scale,
        strokeColor: marker.getIcon().strokeColor,
        strokeWeight: marker.getIcon().strokeWeight,
        fillColor: marker.getIcon().fillColor,
        fillOpacity: marker.getIcon().fillOpacity
    }
    return jsonMarkerConfig;
}

function getJsonPolyline(polyline){
    var jsonPolyline={
        id: polylines.indexOf(polyline),
        vertices:polyline.getPath().getArray()
    };
    return jsonPolyline;
}

function getJsonPolylineConfig(polyline){
    var jsonPolylineConfig={
        strokeColor: polyline.get("strokeColor"),
        strokeWeight:polyline.get("strokeWeight"),
        strokeOpacity:polyline.get("strokeOpacity")
    };
    return jsonPolylineConfig;
}

function getJsonPolygon(polygon){
    var jsonPolygon={
        id: polygons.indexOf(polygon),
        vertices:polygon.getPath().getArray()
    };
    return jsonPolygon;
}

function getJsonPolygonConfig(polygon){
    var jsonPolygonConfig={
        strokeColor: polygon.get("strokeColor"),
        strokeWeight:polygon.get("strokeWeight"),
        strokeOpacity:polygon.get("strokeOpacity"),
        fillColor:polygon.get("fillColor"),
        fillOpacity:polygon.get("fillOpacity")
    };
    return jsonPolygonConfig;
}

function connectWebSocket(btn){
    if(connectedflag){
        disconnect();
        socket.close();
    }else{
        var address = document.getElementById("address").value;
        var port = document.getElementById("port").value;
        if(getClientId().length>0 && address.length>0 && port.length>0)
        {
            baseUrl = "ws://" + address + ":" + port;
            socket = new WebSocket(baseUrl);

            socket.onclose = function(){
                console.error("Web channel closed");
                if(connectedflag){
                    document.getElementById("occupy").value="Occupy";
                    document.getElementById("owner").value="";
                    occupyflag = false;
                    for(var lid in listener){
                        google.maps.event.removeListener(listener[lid]);
                    }
                    listener=[];
                    connectedflag = false;
                    document.getElementById("connect").value = "Connect";
                    document.getElementById("clientid").disabled=connectedflag;
                    document.getElementById("address").disabled=connectedflag;
                    document.getElementById("port").disabled=connectedflag;
                    document.getElementById("occupy").disabled=!connectedflag;
                    for(var i=0;i<markers.length;i++)
                    {
                        markers[i].setMap(null);
                    }
                    markers=[];
                    for(var i=0;i<polylines.length;i++)
                    {
                        polylines[i].setMap(null);
                    }
                    polylines=[];
                    for(var i=0;i<polygons.length;i++)
                    {
                        polygons[i].setMap(null);
                    }
                    polygons=[];
                }
            }

            socket.onerror = function(error){
                console.error("Web channel error: " + error);
                socket.close();
            };
            socket.onopen = function(){
                console.info("Web channel connected, setting up QWebChannel.");
                new QWebChannel(socket,function(channel){
                    window.qgmapinterface = channel.objects.qgmapinterface;
                    var lid=0;

                    qgmapinterface.jsignalDisconnectClient.connect(function(clientId){
                       if(isTarget(clientId)){
                           if(connectedflag){
                               disconnect();
                               socket.close();
                           }
                       }
                    });

                    qgmapinterface.jsignalIsMapInitialized.connect(function(clientid){
                        if(isTarget(clientid)){
                            qgmapinterface.jslotIsMapInitialized(map !== null,getClientId());
                        }
                    });

                    qgmapinterface.jsignalEditorOwnerChanged.connect(function(ownerClientId,clientid){
                        if(isTarget(clientid)){
                            document.getElementById("owner").value=ownerClientId;
                            occupyflag=getClientId()===ownerClientId;
                            if(occupyflag){
                                document.getElementById("occupy").value="Release";
                            }else{
                                document.getElementById("occupy").value="Occupy";
                            }
                            for(var i=0;i<markers.length;i++){
                                if(markers[i]!==null){
                                    markers[i].setDraggable(occupyflag);
                                }
                            }
                            for(var i=0;i<polylines.length;i++){
                                if(polylines[i]!==null){
                                    polylines[i].setEditable(occupyflag);
                                }
                            }
                            for(var i=0;i<polygons.length;i++){
                                if(polygons[i]!==null){
                                    polygons[i].setEditable(occupyflag);
                                }
                            }
                        }
                    });

                    qgmapinterface.jsignalGetMapConfig.connect(function(clientid){
                       if(isTarget(clientid)){
                           var center = map.getCenter();
                           var type = map.getMapTypeId();
                           var zoom = map.getZoom();
                           qgmapinterface.jslotGetMapConfig(center.lat(),center.lng(),type,zoom,getClientId());
                       }
                    });

                    qgmapinterface.jsignalSetMapConfig.connect(function(lat,lng,type,zoom,clientid){
                       if(isTarget(clientid)){
                           var latlng = new google.maps.LatLng(lat,lng);
                           map.setCenter(latlng);
                           map.setMapTypeId(type);
                           map.setZoom(zoom);
                       }
                    });

                    qgmapinterface.jsignalGetMapType.connect(function(clientid){
                       if(isTarget(clientid)){
                           var type = map.getMapTypeId();
                           qgmapinterface.jslotGetMapType(type,getClientId());
                       }
                    });

                    qgmapinterface.jsignalSetMapType.connect(function(type,clientid){
                       if(isTarget(clientid)){
                           map.setMapTypeId(type);
                       }
                    });

                    qgmapinterface.jsignalGetMapCenter.connect(function(clientid){
                       if(isTarget(clientid)){
                           var center = map.getCenter();
                           qgmapinterface.jslotGetMapCenter(center.lat(),center.lng(),getClientId());
                       }
                    });

                    qgmapinterface.jsignalSetMapCenter.connect(function(lat,lng,animated,clientid){
                       if(isTarget(clientid)){
                           var latlng = new google.maps.LatLng(lat,lng);
                           if(animated)
                               map.panTo(latlng);
                           else
                               map.setCenter(latlng);
                       }
                    });

                    qgmapinterface.jsignalGetMapBounds.connect(function(clientid){
                        if(isTarget(clientid))
                        {
                            var bounds = map.getBounds();
                            var ne = bounds.getNorthEast();
                            var sw = bounds.getSouthWest();
                            qgmapinterface.jslotGetMapBounds(ne.lng(),sw.lng(),ne.lat(),sw.lat(),getClientId());
                        }
                    });

                    qgmapinterface.jsignalSetMapBounds.connect(function(north,south,east,west,fit,clientid){
                       if(isTarget(clientid)){
                           var ne = new google.maps.LatLng(north, east);
                           var sw = new google.maps.LatLng(south, west);
                           var bounds=new google.maps.LatLngBounds(sw, ne);
                           if(fit){
                               map.fitBounds(bounds);
                           }else{
                               map.panToBounds(bounds);
                           }
                       }
                    });

                    qgmapinterface.jsignalGetMapZoom.connect(function(clientid){
                       if(isTarget(clientid)){
                           var zoom = map.getZoom();
                           qgmapinterface.jslotGetMapZoom(zoom,getClientId());
                       }
                    });

                    qgmapinterface.jsignalSetMapZoom.connect(function(zoom,clientid){
                       if(isTarget(clientid)){
                           map.setZoom(zoom);
                       }
                    });

                    qgmapinterface.jsignalGetMapHeading.connect(function(clientid){
                       if(isTarget(clientid)){
                           var heading = map.getHeading();
                           qgmapinterface.jslotGetMapHeading(heading,getClientId());
                       }
                    });

                    qgmapinterface.jsignalSetMapHeading.connect(function(heading,clientid){
                       if(isTarget(clientid)){
                           map.setHeading(heading);
                       }
                    });

                    qgmapinterface.jsignalGetMapTilt.connect(function(clientid){
                       if(isTarget(clientid)){
                           var tilt = map.getTilt();
                           qgmapinterface.jslotGetMapTilt(tilt,getClientId());
                       }
                    });

                    qgmapinterface.jsignalSetMapTilt.connect(function(tilt,clientid){
                       if(isTarget(clientid)){
                           map.setTilt(tilt);
                       }
                    });

                    listener[lid++]=google.maps.event.addListener(map, "idle", function() {
                        qgmapinterface.jslotMapBecameIdle(getClientId());
                    });
                    listener[lid++]=google.maps.event.addListener(map, "tilesloaded", function() {
                        qgmapinterface.jslotTilesLoaded(getClientId());
                    });
                    listener[lid++]=google.maps.event.addListener(map, "maptypeid_changed", function() {
                        qgmapinterface.jslotMapTypeChanged(map.getMapTypeId(),getClientId());
                    });
                    listener[lid++]=google.maps.event.addListener(map, "center_changed", function() {
                        var center = map.getCenter();
                        qgmapinterface.jslotCenterChanged(center.lat(), center.lng(),getClientId());
                    });
                    listener[lid++]=google.maps.event.addListener(map, "bounds_changed", function() {
                        var bounds = map.getBounds();
                        var ne = bounds.getNorthEast();
                        var sw = bounds.getSouthWest();
                        qgmapinterface.jslotBoundsChanged(ne.lat(),sw.lat(),ne.lng(),sw.lng(),getClientId());
                    });
                    listener[lid++]=google.maps.event.addListener(map, "zoom_changed", function() {
                        qgmapinterface.jslotZoomChanged(map.getZoom(),getClientId());
                    });
                    listener[lid++]=google.maps.event.addListener(map, "heading_changed", function() {
                        qgmapinterface.jslotHeadingChanged(map.getHeading(),getClientId());
                    });
                    listener[lid++]=google.maps.event.addListener(map, "tilt_changed", function() {
                        qgmapinterface.jslotTiltChanged(map.getTilt(),getClientId());
                    });
                    listener[lid++]=google.maps.event.addListener(map, "click", function(e) {
                        qgmapinterface.jslotMouseClicked(e.latLng.lat(), e.latLng.lng(),getClientId());
                    });
                    listener[lid++]=google.maps.event.addListener(map, "dblclick", function(e) {
                        qgmapinterface.jslotMouseDoubleClicked(e.latLng.lat(), e.latLng.lng(),getClientId());
                    });
                    listener[lid++]=google.maps.event.addListener(map, "rightclick", function(e) {
                        qgmapinterface.jslotMouseRightClicked(e.latLng.lat(), e.latLng.lng(),getClientId());
                    });
                    listener[lid++]=google.maps.event.addListener(map, "dragstart", function() {
                        qgmapinterface.jslotMouseDragStarted(getClientId());
                    });
                    listener[lid++]=google.maps.event.addListener(map, "drag", function() {
                        qgmapinterface.jslotMouseDragged(getClientId());
                    });
                    listener[lid++]=google.maps.event.addListener(map, "dragend", function() {
                        qgmapinterface.jslotMouseDragEnded(getClientId());
                    });
                    listener[lid++]=google.maps.event.addListener(map, "mousemove", function(e) {
                        qgmapinterface.jslotMouseMoved(e.latLng.lat(), e.latLng.lng(),getClientId());
                    });
                    listener[lid++]=google.maps.event.addListener(map, "mouseover", function(e) {
                        qgmapinterface.jslotMouseEntered(e.latLng.lat(), e.latLng.lng(),getClientId());
                    });
                    listener[lid++]=google.maps.event.addListener(map, "mouseout", function(e) {
                        qgmapinterface.jslotMouseLeft(e.latLng.lat(), e.latLng.lng(),getClientId());
                    });

                    qgmapinterface.jsignalSetMarker.connect(function(jsonMarkers,jsonMarkerConfigs,clientid){
                        if(isTarget(clientid)){
                            for(var i=0;i<jsonMarkers.length;i++){
                                var id=jsonMarkers[i].id;
                                var label=jsonMarkers[i].label;
                                var title=jsonMarkers[i].title;
                                var position=jsonMarkers[i].position;
                                var icon={path:google.maps.SymbolPath.CIRCLE,scale:jsonMarkerConfigs[i].scale,
                                    strokeColor: jsonMarkerConfigs[i].strokeColor, strokeWeight: jsonMarkerConfigs[i].strokeWeight,
                                    fillColor: jsonMarkerConfigs[i].fillColor, fillOpacity: jsonMarkerConfigs[i].fillOpacity};
                                if(id>=0&&id<markers.length&&markers[id]!==null){
                                    markers[id].setOptions({
                                                               abel: label,
                                                               title: title,
                                                               position: position,
                                                               icon: icon,
                                                               draggable: occupyflag,
                                                               clickable: true,
                                                               map: map
                                                           });
                                }else{
                                    var marker = new google.maps.Marker({
                                                                            label: label,
                                                                            title: title,
                                                                            position: position,
                                                                            icon: icon,
                                                                            draggable: occupyflag,
                                                                            clickable: true,
                                                                            map: map
                                                                        });

                                    if(id>=0){
                                        markers[id]=marker;
                                    }else{
                                        markers.push(marker);
                                    }

                                    google.maps.event.addListener(marker, "click", function(e) {
                                        qgmapinterface.jslotMarkerClicked(JSON.stringify(getJsonMarker(marker)),JSON.stringify(getJsonMarkerConfig(marker)),getClientId());
                                    });

                                    google.maps.event.addListener(marker,"dblclick",function(e){
                                        qgmapinterface.jslotMarkerDoubleClicked(JSON.stringify(getJsonMarker(marker)),JSON.stringify(getJsonMarkerConfig(marker)),getClientId());
                                    });

                                    google.maps.event.addListener(marker,"rightclick",function(e){
                                        qgmapinterface.jslotMarkerRightClicked(JSON.stringify(getJsonMarker(marker)),JSON.stringify(getJsonMarkerConfig(marker)),getClientId());
                                    });

                                    google.maps.event.addListener(marker,"dragstart",function(e){
                                        qgmapinterface.jslotMarkerDragStart(JSON.stringify(getJsonMarker(marker)),JSON.stringify(getJsonMarkerConfig(marker)),getClientId());
                                    });

                                    google.maps.event.addListener(marker,"drag",function(e){
                                        qgmapinterface.jslotMarkerDrag(JSON.stringify(getJsonMarker(marker)),JSON.stringify(getJsonMarkerConfig(marker)),getClientId());
                                    });

                                    google.maps.event.addListener(marker,"dragend",function(e){
                                        qgmapinterface.jslotMarkerDragEnd(JSON.stringify(getJsonMarker(marker)),JSON.stringify(getJsonMarkerConfig(marker)),getClientId());
                                    });

                                    google.maps.event.addListener(marker,"mousedown",function(e){
                                        qgmapinterface.jslotMarkerMouseDown(JSON.stringify(getJsonMarker(marker)),JSON.stringify(getJsonMarkerConfig(marker)),getClientId());
                                    });

                                    google.maps.event.addListener(marker,"mouseup",function(e){
                                        qgmapinterface.jslotMarkerMouseUp(JSON.stringify(getJsonMarker(marker)),JSON.stringify(getJsonMarkerConfig(marker)),getClientId());
                                    });

                                    google.maps.event.addListener(marker,"mouseover",function(e){
                                        qgmapinterface.jslotMarkerMouseOver(JSON.stringify(getJsonMarker(marker)),JSON.stringify(getJsonMarkerConfig(marker)),getClientId());
                                    });

                                    google.maps.event.addListener(marker,"mouseout",function(e){
                                        qgmapinterface.jslotMarkerMouseOut(JSON.stringify(getJsonMarker(marker)),JSON.stringify(getJsonMarkerConfig(marker)),getClientId());
                                    });

                                    google.maps.event.addListener(marker,"position_changed",function(e){
                                        qgmapinterface.jslotMarkerPositionChanged(JSON.stringify(getJsonMarker(marker)),JSON.stringify(getJsonMarkerConfig(marker)),getClientId());
                                    });
                                }
                            }
                        }
                    });

                    qgmapinterface.jsignalRemoveMarker.connect(function(id,length,clientid){
                       if(isTarget(clientid)){
                           if(id<0){
                               id=0;
                               length=markers.length;
                           }
                           if(id<markers.length){
                               if(id+length>markers.length){
                                   length=markers.length-id;
                               }
                               for(var i=0;i<length;i++){
                                   if(markers[id+i]!==null){
                                    markers[id+i].setMap(null);
                                   }
                               }
                               markers.splice(id,length);
                           }
                       }
                    });

                    qgmapinterface.jsignalSetMarkerVisible.connect(function(id,length,visible,clientid){
                        if(isTarget(clientid)){
                            if(id<0){
                                id=0;
                                length=markers.length;
                            }
                            if(id<markers.length){
                                if(id+length>markers.length){
                                    length=markers.length-id;
                                }
                                for(var i=0;i<length;i++){
                                    if(markers[id+i]!==null){
                                        markers[id+i].setVisible(visible);
                                    }
                                }
                            }
                        }
                    });

                   qgmapinterface.jsignalSetPolyline.connect(function(jsonPolylines,jsonPolylineConfigs,clientid){
                       if(isTarget(clientid)){
                           for(var i=0;i<jsonPolylines.length;i++){
                               var id=jsonPolylines[i].id;
                               var vertices=jsonPolylines[i].vertices;
                               if(id>=0&&id<polylines.length&&polylines[id]!==null){
                                   polylines[id].setOptions({
                                                                path: vertices,
                                                                strokeColor: jsonPolylineConfigs[i].strokeColor,
                                                                strokeWeight: jsonPolylineConfigs[i].strokeWeight,
                                                                strokeOpacity: jsonPolylineConfigs[i].strokeOpacity,
                                                                editable: occupyflag,
                                                                geodesic: true,
                                                                map: map
                                                            });
                                   google.maps.event.addListener(polylines[id].getPath(),"insert_at",function(vertexid){
                                       qgmapinterface.jslotPolylinePathInsertAt(JSON.stringify(getJsonPolyline(polylines[id])),JSON.stringify(getJsonPolylineConfig(polylines[id])),vertexid,getClientId());
                                   });

                                   google.maps.event.addListener(polylines[id].getPath(),"remove_at",function(vertexid,vertex){
                                       qgmapinterface.jslotPolylinePathRemoveAt(JSON.stringify(getJsonPolyline(polylines[id])),JSON.stringify(getJsonPolylineConfig(polylines[id])),vertexid,getClientId());
                                   });

                                   google.maps.event.addListener(polylines[id].getPath(),"set_at",function(vertexid,vertex){
                                       qgmapinterface.jslotPolylinePathSetAt(JSON.stringify(getJsonPolyline(polylines[id])),JSON.stringify(getJsonPolylineConfig(polylines[id])),vertexid,getClientId());
                                   });
                               }else{
                                   var polyline=new google.maps.Polyline({
                                                                             path: vertices,
                                                                             strokeColor: jsonPolylineConfigs[i].strokeColor,
                                                                             strokeWeight: jsonPolylineConfigs[i].strokeWeight,
                                                                             strokeOpacity: jsonPolylineConfigs[i].strokeOpacity,
                                                                             editable: occupyflag,
                                                                             geodesic: true,
                                                                             map: map
                                                                         });

                                   if(id>=0){
                                       polylines[id]=polyline;
                                   }else{
                                       polylines.push(polyline);
                                   }

                                   google.maps.event.addListener(polyline,"click",function(e){
                                       if(e.vertex===undefined){return;}
                                       qgmapinterface.jslotPolylineClicked(JSON.stringify(getJsonPolyline(polyline)),JSON.stringify(getJsonPolylineConfig(polyline)),e.vertex,getClientId());
                                   });

                                   google.maps.event.addListener(polyline,"dblclick",function(e){
                                       if(e.vertex===undefined){return;}
                                       qgmapinterface.jslotPolylineDoubleClicked(JSON.stringify(getJsonPolyline(polyline)),JSON.stringify(getJsonPolylineConfig(polyline)),e.vertex,getClientId());
                                   });

                                   google.maps.event.addListener(polyline,"rightclick",function(e){
                                       if(e.vertex===undefined){return;}
                                       qgmapinterface.jslotPolylineRightClicked(JSON.stringify(getJsonPolyline(polyline)),JSON.stringify(getJsonPolylineConfig(polyline)),e.vertex,getClientId());
                                   });

                                   google.maps.event.addListener(polyline,"dragstart",function(e){
                                       qgmapinterface.jslotPolylineRightClicked(JSON.stringify(getJsonPolyline(polyline)),JSON.stringify(getJsonPolylineConfig(polyline)),getClientId());
                                   });

                                   google.maps.event.addListener(polyline,"drag",function(e){
                                       qgmapinterface.jslotPolylineDrag(JSON.stringify(getJsonPolyline(polyline)),JSON.stringify(getJsonPolylineConfig(polyline)),getClientId());
                                   });

                                   google.maps.event.addListener(polyline,"dragend",function(e){
                                       qgmapinterface.jslotPolylineDragEnd(JSON.stringify(getJsonPolyline(polyline)),JSON.stringify(getJsonPolylineConfig(polyline)),getClientId());
                                   });

                                   google.maps.event.addListener(polyline,"mousedown",function(e){
                                       if(e.vertex===undefined){return;}
                                       qgmapinterface.jslotPolylineMouseDown(JSON.stringify(getJsonPolyline(polyline)),JSON.stringify(getJsonPolylineConfig(polyline)),e.vertex,getClientId());
                                   });

                                   google.maps.event.addListener(polyline,"mouseup",function(e){
                                       if(e.vertex===undefined){return;}
                                       qgmapinterface.jslotPolylineMouseUp(JSON.stringify(getJsonPolyline(polyline)),JSON.stringify(getJsonPolylineConfig(polyline)),e.vertex,getClientId());
                                   });

                                   google.maps.event.addListener(polyline,"mouseover",function(e){
                                       if(e.vertex===undefined){return;}
                                       qgmapinterface.jslotPolylineMouseOver(JSON.stringify(getJsonPolyline(polyline)),JSON.stringify(getJsonPolylineConfig(polyline)),e.vertex,getClientId());
                                   });

                                   google.maps.event.addListener(polyline,"mouseout",function(e){
                                       if(e.vertex===undefined){return;}
                                       qgmapinterface.jslotPolylineMouseOut(JSON.stringify(getJsonPolyline(polyline)),JSON.stringify(getJsonPolylineConfig(polyline)),e.vertex,getClientId());
                                   });

                                   google.maps.event.addListener(polyline.getPath(),"insert_at",function(vertexid){
                                       qgmapinterface.jslotPolylinePathInsertAt(JSON.stringify(getJsonPolyline(polyline)),JSON.stringify(getJsonPolylineConfig(polyline)),vertexid,getClientId());
                                   });

                                   google.maps.event.addListener(polyline.getPath(),"remove_at",function(vertexid,vertex){
                                       qgmapinterface.jslotPolylinePathRemoveAt(JSON.stringify(getJsonPolyline(polyline)),JSON.stringify(getJsonPolylineConfig(polyline)),vertexid,getClientId());
                                   });

                                   google.maps.event.addListener(polyline.getPath(),"set_at",function(vertexid,vertex){
                                       qgmapinterface.jslotPolylinePathSetAt(JSON.stringify(getJsonPolyline(polyline)),JSON.stringify(getJsonPolylineConfig(polyline)),vertexid,getClientId());
                                   });
                               }
                           }
                       }
                   });

                    qgmapinterface.jsignalRemovePolyline.connect(function(id,length,clientid){
                        if(isTarget(clientid)){
                            if(id<0){
                                id=0;
                                length=polylines.length;
                            }
                            if(id<polylines.length){
                                if(id+length>polylines.length){
                                    length=polylines.length-id;
                                }
                                for(var i=0;i<length;i++){
                                    if(polylines[id+i]!==null){
                                        polylines[id+i].setMap(null);
                                    }
                                }
                                polylines.splice(id,length);
                            }
                        }
                    });

                    qgmapinterface.jsignalSetPolylineVisible.connect(function(id,length,visible,clientid){
                        if(isTarget(clientid)){
                            if(id<0){
                                id=0;
                                length=polylines.length;
                            }
                            if(id<polylines.length){
                                if(id+length>polylines.length){
                                    length=polylines.length-id;
                                }
                                for(var i=0;i<length;i++){
                                    if(polylines[id+i]!==null){
                                        polylines[id+i].setVisible(visible);
                                    }
                                }
                            }
                        }
                    });

                    qgmapinterface.jsignalSetPolygon.connect(function(jsonPolygons,jsonPolygonConfigs,clientid){
                        if(isTarget(clientid)){
                            for(var i=0;i<jsonPolygons.length;i++){
                                var id=jsonPolygons[i].id;
                                var vertices=jsonPolygons[i].vertices;
                                if(id>=0&&id<polygons.length&&polygons[id]!==null){
                                    polygons[id].setOptions({
                                                                path: vertices,
                                                                strokeColor: jsonPolygonConfigs[i].strokeColor,
                                                                strokeWeight: jsonPolygonConfigs[i].strokeWeight,
                                                                strokeOpacity: jsonPolygonConfigs[i].strokeOpacity,
                                                                fillColor:jsonPolygonConfigs[i].fillColor,
                                                                fillOpacity:jsonPolygonConfigs[i].fillOpacity,
                                                                editable: occupyflag,
                                                                map: map
                                                            });
                                    google.maps.event.addListener(polygons[id].getPath(),"insert_at",function(vertexid){
                                        qgmapinterface.jslotPolygonPathInsertAt(JSON.stringify(getJsonPolygon(polygons[id])),JSON.stringify(getJsonPolygonConfig(polygons[id])),vertexid,getClientId());
                                    });

                                    google.maps.event.addListener(polygons[id].getPath(),"remove_at",function(vertexid,vertex){
                                        qgmapinterface.jslotPolygonPathRemoveAt(JSON.stringify(getJsonPolygon(polygons[id])),JSON.stringify(getJsonPolygonConfig(polygons[id])),vertexid,getClientId());
                                    });

                                    google.maps.event.addListener(polygons[id].getPath(),"set_at",function(vertexid,vertex){
                                        qgmapinterface.jslotPolygonPathSetAt(JSON.stringify(getJsonPolygon(polygons[id])),JSON.stringify(getJsonPolygonConfig(polygons[id])),vertexid,getClientId());
                                    });
                                }else{
                                    var polygon=new google.maps.Polygon({
                                                                            path: vertices,
                                                                            strokeColor: jsonPolygonConfigs[i].strokeColor,
                                                                            strokeWeight: jsonPolygonConfigs[i].strokeWeight,
                                                                            strokeOpacity: jsonPolygonConfigs[i].strokeOpacity,
                                                                            fillColor:jsonPolygonConfigs[i].fillColor,
                                                                            fillOpacity:jsonPolygonConfigs[i].fillOpacity,
                                                                            editable: occupyflag,
                                                                            map: map
                                                                        });

                                    if(id>=0){
                                        polygons[id]=polygon;
                                    }else{
                                        polygons.push(polygon);
                                    }

                                    google.maps.event.addListener(polygon,"click",function(e){
                                        if(e.vertex===undefined){return;}
                                        qgmapinterface.jslotPolygonClicked(JSON.stringify(getJsonPolygon(polygon)),JSON.stringify(getJsonPolygonConfig(polygon)),e.vertex,getClientId());
                                    });

                                    google.maps.event.addListener(polygon,"dblclick",function(e){
                                        if(e.vertex===undefined){return;}
                                        qgmapinterface.jslotPolygonDoubleClicked(JSON.stringify(getJsonPolygon(polygon)),JSON.stringify(getJsonPolygonConfig(polygon)),e.vertex,getClientId());
                                    });

                                    google.maps.event.addListener(polygon,"rightclick",function(e){
                                        if(e.vertex===undefined){return;}
                                        qgmapinterface.jslotPolygonRightClicked(JSON.stringify(getJsonPolygon(polygon)),JSON.stringify(getJsonPolygonConfig(polygon)),e.vertex,getClientId());
                                    });

                                    google.maps.event.addListener(polygon,"dragstart",function(e){
                                        qgmapinterface.jslotPolygonRightClicked(JSON.stringify(getJsonPolygon(polygon)),JSON.stringify(getJsonPolygonConfig(polygon)),getClientId());
                                    });

                                    google.maps.event.addListener(polygon,"drag",function(e){
                                        qgmapinterface.jslotPolygonDrag(JSON.stringify(getJsonPolygon(polygon)),JSON.stringify(getJsonPolygonConfig(polygon)),getClientId());
                                    });

                                    google.maps.event.addListener(polygon,"dragend",function(e){
                                        qgmapinterface.jslotPolygonDragEnd(JSON.stringify(getJsonPolygon(polygon)),JSON.stringify(getJsonPolygonConfig(polygon)),getClientId());
                                    });

                                    google.maps.event.addListener(polygon,"mousedown",function(e){
                                        if(e.vertex===undefined){return;}
                                        qgmapinterface.jslotPolygonMouseDown(JSON.stringify(getJsonPolygon(polygon)),JSON.stringify(getJsonPolygonConfig(polygon)),e.vertex,getClientId());
                                    });

                                    google.maps.event.addListener(polygon,"mouseup",function(e){
                                        if(e.vertex===undefined){return;}
                                        qgmapinterface.jslotPolygonMouseUp(JSON.stringify(getJsonPolygon(polygon)),JSON.stringify(getJsonPolygonConfig(polygon)),e.vertex,getClientId());
                                    });

                                    google.maps.event.addListener(polygon,"mouseover",function(e){
                                        if(e.vertex===undefined){return;}
                                        qgmapinterface.jslotPolygonMouseOver(JSON.stringify(getJsonPolygon(polygon)),JSON.stringify(getJsonPolygonConfig(polygon)),e.vertex,getClientId());
                                    });

                                    google.maps.event.addListener(polygon,"mouseout",function(e){
                                        if(e.vertex===undefined){return;}
                                        qgmapinterface.jslotPolygonMouseOut(JSON.stringify(getJsonPolygon(polygon)),JSON.stringify(getJsonPolygonConfig(polygon)),e.vertex,getClientId());
                                    });

                                    google.maps.event.addListener(polygon.getPath(),"insert_at",function(vertexid){
                                        qgmapinterface.jslotPolygonPathInsertAt(JSON.stringify(getJsonPolygon(polygon)),JSON.stringify(getJsonPolygonConfig(polygon)),vertexid,getClientId());
                                    });

                                    google.maps.event.addListener(polygon.getPath(),"remove_at",function(vertexid,vertex){
                                        qgmapinterface.jslotPolygonPathRemoveAt(JSON.stringify(getJsonPolygon(polygon)),JSON.stringify(getJsonPolygonConfig(polygon)),vertexid,getClientId());
                                    });

                                    google.maps.event.addListener(polygon.getPath(),"set_at",function(vertexid,vertex){
                                        qgmapinterface.jslotPolygonPathSetAt(JSON.stringify(getJsonPolygon(polygon)),JSON.stringify(getJsonPolygonConfig(polygon)),vertexid,getClientId());
                                    });
                                }
                            }
                        }
                    });

                     qgmapinterface.jsignalRemovePolygon.connect(function(id,length,clientid){
                         if(isTarget(clientid)){
                             if(id<0){
                                 id=0;
                                 length=polygons.length;
                             }
                             if(id<polygons.length){
                                 if(id+length>polygons.length){
                                     length=polygons.length-id;
                                 }
                                 for(var i=0;i<length;i++){
                                     if(polygons[id+i]!==null){
                                         polygons[id+i].setMap(null);
                                     }
                                 }
                                 polygons.splice(id,length);
                             }
                         }
                     });

                     qgmapinterface.jsignalSetPolygonVisible.connect(function(id,length,visible,clientid){
                         if(isTarget(clientid)){
                             if(id<0){
                                 id=0;
                                 length=polygons.length;
                             }
                             if(id<polygons.length){
                                 if(id+length>polygons.length){
                                     length=polygons.length-id;
                                 }
                                 for(var i=0;i<length;i++){
                                     if(polygons[id+i]!==null){
                                         polygons[id+i].setVisible(visible);
                                     }
                                 }
                             }
                         }
                     });

                    qgmapinterface.jslotWebSocketConnected(getClientId(),function(finalclientid){
                        document.getElementById("clientid").value=finalclientid;
                        qgmapinterface.jslotClientIdConfirmed(getClientId());
                    });
                    console.info("Connected to WebChannel.");
                });
            };
            btn.value="Disconnect";
            connectedflag=!connectedflag;
            document.getElementById("clientid").disabled=connectedflag;
            document.getElementById("address").disabled=connectedflag;
            document.getElementById("port").disabled=connectedflag;
            document.getElementById("occupy").disabled=!connectedflag;
        }
    }
}

function occupyEditor(btn){
    if(occupyflag){
        qgmapinterface.jslotReleaseEditor(getClientId());
    }else{
        qgmapinterface.jslotOccupyEditor(getClientId());
    }
}
