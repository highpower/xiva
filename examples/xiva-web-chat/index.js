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
  
  var usernames = {};
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
        loginButton.removeClass('b-login__send_disabled').html('Login');
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

    // Send 'login' event
    postMessage("login", {});
  });
  
  messageArea.keydown(function(e) {
    var target = $(this);

    if (e.keyCode == 13) {
      e.preventDefault();

      if (target.val() != '') {
        postMessage("message", {});
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

    // Switch rooms
    var _currentRoom = currentRoom, _newRoom = newRoom;
    switchUserRoom(currentName, currentRoom);
    currentRoom = newRoom;
    postMessage("switch", {oldRoom: _currentRoom, newRoom: _newRoom});
  });
  
  function postMessage(type, params) {
    var messageObj = {
      username: encodeMessage(currentName),
      room: currentRoom,
      type: type,
      text: (type == "message") ? encodeMessage(messageArea.val()) : '',
      params: params
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

    var username = decodeMessage(messageObj.username),
        room = messageObj.room,
        type = messageObj.type,
        text = decodeMessage(messageObj.text),
        date = new Date(),
        params = messageObj.params;
    
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

    if (type == "switch") {
      switchUserRoom(username, params.oldRoom);
      addCreateUser(username, params.newRoom);
    } else {
      addCreateUser(username, currentRoom);
    }

    updateUserList();
  }

  function addCreateUser(username, room) {
    usernames[room] = usernames[room] || [];
    usernames[room].push(username);

    usernames[room] = uniqueArray(usernames[room]).sort();
  }

  function switchUserRoom(username, currentRoom) {
    // Remove user from current room
    var currentRoomUsers = usernames[currentRoom] || [];

    var idx = currentRoomUsers.indexOf(username);

    idx > -1 && currentRoomUsers.splice(idx, 1);
  }

  function updateUserList() {
    userContainer.html('');
    userContainer.append('<span class="b-userlist__user"><strong>' + currentName + '</strong> (you)</span>');

    var usersInRoom = usernames[currentRoom] || [];

    for (var i=0; i<usersInRoom.length; i++) {
      if (usersInRoom[i] && usersInRoom[i] != currentName) {
        userContainer.append(', ' + '<a href="#" class="b-userlist__user b-pseudo-link">' + usersInRoom[i] + '</a>');
      }
    }
  }

  // Utils
  function uniqueArray(arr) {
    var a = [];
    var l = arr.length;
    for(var i=0; i<l; i++) {
      for(var j=i+1; j<l; j++) {
        if (arr[i] === arr[j]) {
          j = ++i;
        }
      }
      a.push(arr[i]);
    }
    return a;
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