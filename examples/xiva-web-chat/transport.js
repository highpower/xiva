var Xiva = {};

Xiva.Transport = function(onopen, onmessage, onclose) {
  this.ws = null;
  
  this.onopen = onopen;
  this.onmessage = onmessage;
  this.onclose = onclose;
    
  window.WEB_SOCKET_SWF_LOCATION = "web-socket-js/WebSocketMain.swf";
  window.WEB_SOCKET_DEBUG = true;
};

Xiva.Transport.prototype = {
  initSocket: function() {
    if (window.WebSocket.toString().match(/\[native code\]/) || swfobject.hasFlashPlayerVersion("9.0.0")) {
      this.initWSTransport();
    } else {
      this.initXHRTransport();
    }
  },
  
  initWSTransport: function() {
    // Connect to Web Socket.
    // Change host/port here to your own Web Socket server.
    this.ws = new WebSocket('ws://' + window.location.hostname.toString() + ':8881/?id=chat');

    // Set event handlers.
    this.ws.onopen = this.onopen;
    this.ws.onmessage = this.onmessage;
    this.ws.onclose = this.onclose;
  },
  
  initXHRTransport: function() {
  },
  
  postMessage: function(message) {
    $.ajax({
      type: 'POST',
      url: '/xiva',
      data: message
    });
  }
};
