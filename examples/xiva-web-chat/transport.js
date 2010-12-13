var Xiva = {};

Xiva.Transport = function(onopen, onmessage, onclose) {
  this.ws = null;
  
  // this.ws_url = window.location.hostname.toString() + ':8881/?id=chat';
  this.ws_url = 'xiva.opensource.yandex.net/?id=chat';
  
  this.onopen = onopen;
  this.onmessage = onmessage;
  this.onclose = onclose;
  
  this.connectionError = 0;
    
  window.WEB_SOCKET_SWF_LOCATION = "web-socket-js/WebSocketMain.swf";
  window.WEB_SOCKET_DEBUG = true;
};

Xiva.Transport.prototype = {
  initSocket: function() {
    // Even though WS works in IE, connection setup is really slow, so it's probably better to use the XHR transport.
    if (((window.WebSocket && window.WebSocket.toString().match(/\[native code\]/)) || swfobject.hasFlashPlayerVersion("9.0.0")) && !$.browser.msie) {
      this.initWSTransport();
    } else {
      this.initXHRTransport();
      this.onopen();
    }
  },
  
  initWSTransport: function() {
    // Connect to Web Socket.
    // Change host/port here to your own Web Socket server.
    this.ws = new WebSocket('ws://' + this.ws_url);

    // Set event handlers.
    this.ws.onopen = this.onopen;
    this.ws.onmessage = this.onmessage;
    this.ws.onclose = this.onclose;
  },
  
  initXHRTransport: function() {
    var self = this;
        
    // A very simple JSON-P transport. Won't care about message ordering, stalled connections, or whatever.
    $.ajax({
      url: 'http://' + this.ws_url + '&single',
      dataType: 'jsonp',
      context: this,
      success: function(data) {
        this.connectionError = 0;
        this.onmessage({data: JSON.stringify(data)}); // Simulate the WS response object
        this.initXHRTransport(); // Reconnect
      },
      error: function() {
        // Some trivial error protection
        this.connectionError++;

        if (connectionError < 10) {
          window.setTimeout(function() {
            this.initXHRTransport();
          }, 100 * this.connectionError);
        }
      }
    });
  },
  
  postMessage: function(message) {
    $.ajax({
      type: 'POST',
      url: '/xiva',
      data: message
    });
  },
  
  // Encodes a string into a sequence of \uXXXX codes
  encodeMessage: function(str) {
    var encodedStr = '';

    function pad(chr) {
      while (chr.length < 4) {
        chr = '0' + chr;
      }

      return chr;
    }
    
    for (var i=0; i<str.length; i++) {
      var chr = '\\u';

      chr += pad(str.charCodeAt(i).toString(16));

      encodedStr += chr;
    }

    return encodedStr;
  },

  // Decodes a sequence of \uXXXX codes
  decodeMessage: function(str) {
    // This is not an encoded string
    if (str.indexOf('\\u') == -1) {
      return str;
    }
    
    var decodedStr = '';

    for (var i=0; i<str.length; i++) {
      var charCode = '';

      charCode += str[i+2];
      charCode += str[i+3];
      charCode += str[i+4];
      charCode += str[i+5];

      decodedStr += String.fromCharCode(parseInt(charCode, 16));

      i+=5;
    }

    return decodedStr.replace(/^[0]+}/, '');
  },

  escapeHTML: function(str) {
    return str.replace(/&/g, "&amp;").replace(/</g, "&lt;").replace(/>/g, "&gt;");
  }
};
