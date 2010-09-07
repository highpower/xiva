$(document).ready(function() {
  var inited = false;
  
  var bMessages = $('.b-messages');
  var messageArea = $('.b-writemessage__area');

  var userLinks = $('a.b-userlist__user');
  var roomLinks = $('.b-rooms__room a');
  
  // WebSocket implementation
  WebSocket.__swfLocation = 'web-socket-js/WebSocketMain.swf';

  var ws;
  
  function init() {
    // Connect to Web Socket.
    // Change host/port here to your own Web Socket server.
    ws = new WebSocket('ws://leonya.dev.yandex.net:8881/?id=chat&channels=rooms_all,room_1,room_2,room_3');

    // Set event handlers.
    ws.onopen = function() {
      if (inited === false) {
        messageArea.get(0).disabled = false;
        messageArea.val('');
        inited = true;
      }
    };
    
    ws.onmessage = function(e) {
      receiveMessage(e.data);
    };
    
    ws.onclose = function() {
      setTimeout(init, 1000);
    };
  }
  
  init();
	
  // UI
  messageArea.keydown(function(e) {
    if (e.keyCode == 13) {
      e.preventDefault();

      if (messageArea.val() != '') {
        postMessage();
        messageArea.val('');
      }
    }
  });

  userLinks.click(function(e) {
    e.preventDefault();
    messageArea.val('@' + $(e.target).html() + ' ');
    messageArea.focus();
  });

  roomLinks.click(function(e) {
    e.preventDefault();
  });
  
  function postMessage() {
    var message = 'cmd=' + encodeURIComponent('msg chat/room_1 ' + messageArea.val());
    
    var client = new XMLHttpRequest();
    client.onreadystatechange = function() {};
    client.open("POST", "/xiva");
    client.setRequestHeader('Content-Type', 'application/x-www-form-urlencoded');  
    client.send(message);
  }

  function receiveMessage(messageText) {
    if (messageText === "ping\n") return;
    
    var message = $('<div class="b-message b-message_you" style="opacity: 0.5"><div class="b-message__head"><div class="b-message__head__user">Leonya</div><div class="b-message__head__time">Aug 31, 2010 15:35</div></div><div class="b-message__body">' + messageText + '</div></div>');
    bMessages.append(message);
    message.animate({ opacity: 1 }, 1000);
    bMessages.scrollTop(10000);
  }
});