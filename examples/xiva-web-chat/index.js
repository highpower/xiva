$(document).ready(function() {
  var inited = false;
  
  var bMessages = $('.b-messages');
  var messageArea = $('.b-writemessage__area');

  var userLinks = $('a.b-userlist__user');
  var roomLinks = $('.b-rooms__room a');
  
  var loginButton = $('.b-login__send');
  
  var currentName, currentRoom;
  
  // WebSocket implementation
  WebSocket.__swfLocation = 'web-socket-js/WebSocketMain.swf';

  var ws;
  
  function init() {
    // Connect to Web Socket.
    // Change host/port here to your own Web Socket server.
    ws = new WebSocket('ws://leonya.dev.yandex.net:8881/?id=chat');

    // Set event handlers.
    ws.onopen = function() {
      if (inited === false) {
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
  loginButton.click(function(e) {
    e.preventDefault();
    
    var loginName = $('#username');
    var loginRoom = $('#roomname :selected');
    
    if ($.trim(loginName.val()) == '') return;
    
    currentName = loginName.val();
    currentRoom = loginRoom.attr('name');
    
    // Show chat window    
    $('.b-chat_login').addClass('g-hidden');
    $('.b-chat_chat').removeClass('g-hidden');
    
    // Show current chat room
    $('#messages_' + currentRoom).removeClass('g-hidden');
    $('header h1').html('Xiva Chatroom &mdash; ' + $('#' + currentRoom).html());
    
    // Make chosen room active
    var roomElement = $('#' + currentRoom).parent();
    
    roomElement.addClass('b-rooms__room_active');
    roomElement.html('<span id="' + currentRoom + '">' + loginRoom.parent().val() + '</a>');
    
    // Append user to the userlist
    $('.b-userlist').removeClass('g-hidden');
    $('.b-userlist__users').append('<span class="b-userlist__user"><strong>' + currentName + '</strong> (you)</span>');
    
    postMessage("login");
  });
  
  messageArea.keydown(function(e) {
    if (e.keyCode == 13) {
      e.preventDefault();

      if (messageArea.val() != '') {
        postMessage("message");
        messageArea.val('');
      }
    }
  });

  userLinks.live('click', function(e) {
    e.preventDefault();
    messageArea.val('@' + $(e.target).html() + ' ');
    messageArea.focus();
  });

  roomLinks.live('click', function(e) {
    e.preventDefault();
    
    var roomId = this.id;
    
    $('#messages_' + currentRoom).addClass('g-hidden');
    $('#messages_' + roomId).removeClass('g-hidden');
    
    var currentRoomElement = $('#' + currentRoom).parent();
    var newRoomElement = $('#' + roomId).parent();
    
    var currentRoomLabel = $('#' + currentRoom).html();
    var newRoomLabel = $('#' + roomId).html();
    
    currentRoomElement.removeClass('b-rooms__room_active');
    newRoomElement.addClass('b-rooms__room_active');
    
    currentRoomElement.html('<a id="' + currentRoom + '" class="b-pseudo-link" href="#">' + currentRoomLabel + '</a>');
    newRoomElement.html('<span id="' + roomId + '">' + newRoomLabel + '</a>');
    
    $('header h1').html('Xiva Chatroom &mdash; ' + newRoomLabel);
    
    currentRoom = roomId;
  });
  
  function postMessage(type) {
    var messageObj = {
      username: currentName,
      room: currentRoom,
      type: type,
      text: (type == "message") ? messageArea.val() : ''
    };
    
    var message = 'cmd=' + 'msg chat/room_1 ' + JSON.stringify(messageObj);
    
    var client = new XMLHttpRequest();
    client.onreadystatechange = function() {};
    client.open("POST", "/xiva");
    client.setRequestHeader('Content-Type', 'application/x-www-form-urlencoded');  
    client.send(message);
  }

  function receiveMessage(messageObj) {
    if (messageObj === "ping\n") return;
    
    messageObj = JSON.parse(messageObj);
    
    if (messageObj.type == "message") {
      var date = new Date();
    
      var message = $('<div class="b-message" style="opacity: 0.5"><div class="b-message__head"><div class="b-message__head__user">' + messageObj.username + '</div><div class="b-message__head__time">' + date + '</div></div><div class="b-message__body">' + messageObj.text + '</div></div>');
    
      if (messageObj.username == currentName) {
        message.addClass('b-message_you');
      }
    
      // Add message to the room it was sent to
      $('#messages_' + messageObj.room).append(message);
      message.animate({ opacity: 1 }, 1000);
      bMessages.scrollTop(10000);
    
      // Update unread count if the user is not in the current room
      if (messageObj.room != currentRoom) {
        var roomElement = $('#' + messageObj.room).parent();
        var roomUnread = roomElement.find('.b-rooms__room__unread');
      
        if (roomUnread.length > 0) {
          roomUnread.html(parseInt(roomUnread.html(), 10) + 1);
        } else {
          roomElement.append($('<span class="b-rooms__room__unread">1</span>'));
        }
      }
    }
    
    // Update user list
    if (messageObj.username != currentName) {
      var existingUser = false;
      $('a.b-userlist__user').each(function() {
        if ($(this).html() == messageObj.username) {
          existingUser = true;
        }
      });

      if (existingUser === false) {
        $('.b-userlist__users').append(', ' + '<a href="#" class="b-userlist__user b-pseudo-link">' + messageObj.username + '</a>');
      }
    }
  }
});