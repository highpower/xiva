$(document).ready(function() {
  var inited = false;
  
  // Blocks
  var bLogin = $('.b-login');
  var bUserList = $('.b-userlist');
  var bChatLogin = $('.b-chat_login');
  var bChatChat = $('.b-chat_chat');
  var bRooms = $('.b-rooms');
  var bMessages = $('.b-messages');
  var bMessageArea = $('.b-writemessage');

  // Elements
  var userContainer = $('.b-userlist__users', bUserList);
  var userLinks = $('a.b-userlist__user', bUserList);
  var roomLinks = $('.b-rooms__room a', bRooms);
  var messageArea = $('.b-writemessage__area', bMessageArea);
  var loginButton = $('.b-login__send', bLogin);
  
  var currentName, currentRoom;

  var ws;

  // WebSocket implementation
  WebSocket.__swfLocation = 'web-socket-js/WebSocketMain.swf';

  var initSocket = function() {
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
      setTimeout(initSocket, 1000);
    };
  };

  initSocket();

  // UI
  loginButton.click(function(e) {
    e.preventDefault();
    
    var loginName = $('#username');
    var loginRoom = $('#roomname :selected');
    
    if ($.trim(loginName.val()) == '') return;
    
    currentName = loginName.val();
    currentRoom = loginRoom.attr('name');
    
    // Show chat window    
    bChatLogin.addClass('g-hidden');
    bChatChat.removeClass('g-hidden');
    
    // Show current chat room
    $('#messages_' + currentRoom).removeClass('g-hidden');
    $('header h1').html('Xiva Chatroom &mdash; ' + $('#' + currentRoom).html());
    
    // Make chosen room active
    var roomElement = $('#' + currentRoom).parent();
    
    roomElement.addClass('b-rooms__room_active');
    roomElement.html('<span id="' + currentRoom + '">' + loginRoom.parent().val() + '</a>');
    
    // Append user to the userlist
    bUserList.removeClass('g-hidden');
    userContainer.append('<span class="b-userlist__user"><strong>' + currentName + '</strong> (you)</span>');

    // Send 'login' event
    postMessage("login");
  });
  
  messageArea.keydown(function(e) {
    var target = $(this);

    if (e.keyCode == 13) {
      e.preventDefault();

      if (target.val() != '') {
        postMessage("message");
        target.val('');
      }
    }
  });

  userLinks.live('click', function(e) {
    var target = $(this);

    e.preventDefault();
    messageArea.val('@' + target.html() + ' ');
    messageArea.focus();
  });

  roomLinks.live('click', function(e) {
    e.preventDefault();
    
    var newRoom = this.id;
    
    $('#messages_' + currentRoom).addClass('g-hidden');
    $('#messages_' + newRoom).removeClass('g-hidden');
    
    var currentRoomLink = $('#' + currentRoom);
    var newRoomLink = $('#' + newRoom);

    var currentRoomElement = currentRoomLink.parent();
    var newRoomElement = newRoomLink.parent();
    
    var currentRoomLabel = currentRoomLink.html();
    var newRoomLabel = newRoomLink.html();
    
    currentRoomElement.removeClass('b-rooms__room_active');
    newRoomElement.addClass('b-rooms__room_active');
    
    currentRoomElement.html('<a id="' + currentRoom + '" class="b-pseudo-link" href="#">' + currentRoomLabel + '</a>');
    newRoomElement.html('<span id="' + newRoom + '">' + newRoomLabel + '</a>');
    
    $('header h1').html('Xiva Chatroom &mdash; ' + newRoomLabel);
    
    currentRoom = newRoom;
  });
  
  function postMessage(type) {
    var messageObj = {
      username: currentName,
      room: currentRoom,
      type: type,
      text: (type == "message") ? encodeMessage(messageArea.val()) : ''
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

    var username = messageObj.username,
        room = messageObj.room,
        type = messageObj.type,
        text = decodeMessage(messageObj.text),
        date = new Date();
    
    if (type == "message") {
      var message = $('<div class="b-message" style="opacity: 0.5">' +
                        '<div class="b-message__head">' +
                          '<div class="b-message__head__user">' + username + '</div>' +
                          '<div class="b-message__head__time">' + date + '</div>' +
                        '</div>' +
                        '<div class="b-message__body">' + text + '</div>' +
                      '</div>');
    
      if (username == currentName) {
        message.addClass('b-message_you');
      }
    
      // Add message to the room it was sent to
      $('#messages_' + room).append(message);

      // Animate new message
      message.animate({ opacity: 1 }, 1000);
      bMessages.scrollTop(10000);
    
      // Update unread count if the user is not in the current room
      if (room != currentRoom) {
        var roomElement = $('#' + room).parent();
        var roomUnread = roomElement.find('.b-rooms__room__unread');
      
        if (roomUnread.length > 0) {
          roomUnread.html(parseInt(roomUnread.html(), 10) + 1);
        } else {
          roomElement.append($('<span class="b-rooms__room__unread">1</span>'));
        }
      }
    }
    
    // Update user list ('message' and 'login' events)
    if (username != currentName) {
      var existingUser = false;

      userLinks = $('a.b-userlist__user', bUserList);
      userLinks.each(function() {
        if ($(this).html() == username) {
          existingUser = true;
        }
      });

      if (existingUser === false) {
        userContainer.append(', ' + '<a href="#" class="b-userlist__user b-pseudo-link">' + username + '</a>');
      }
    }
  }

  function encodeMessage(str) {
    var encodedStr = '';

    function pad(chr) {
      while (chr.length < 4) {
        chr = '0' + chr;
      }

      return chr;
    }

    for (var i=0; i<str.length; i++) {
      var chr = '\\u';

      chr += pad(str[i].charCodeAt(0).toString(16));

      encodedStr += chr;
    }

    return encodedStr;
  }

  function decodeMessage(str) {
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
  }
});