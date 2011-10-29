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
  var loginForm = $('.b-login__form', bLogin);
  var loginButton = $('.b-login__send', bLogin);
  var userContainer = $('.b-userlist__users', bUserList);
  var userLinks = $('a.b-userlist__user', bUserList);
  var roomLinks = $('.b-rooms__room a', bRooms);
  var messageArea = $('.b-writemessage__area', bMessageArea);

  var usernames = {};
  var currentName, currentRoom;

  var onopen = function() {
    if (inited === false) {
      loginButton.removeClass('b-login__send_disabled').html('Login');
      messageArea.val('');
      inited = true;
    }
  };

  var onmessage = function(e) {
    e.data && receiveMessage(e.data);
  };

  var onclose = function() {
    setTimeout(function(transport) {
      transport.initSocket();
    }(transport), 1000);
  };

  var transport = new Xiva.Transport(onopen, onmessage, onclose);

  transport.initSocket();

  // UI
  loginButton.click(login);
  loginForm.submit(login);

  var createTextareaListener = function() {
    messageArea = $('.b-writemessage__area', bMessageArea);

    messageArea.focus();

    messageArea.keypress(function(e) {
      var target = $(this);

      if (e.keyCode == 13) {
        if (target.val() != '') {
          postMessage("message", {});
          target.val('');

          // Despite e.preventDefault(), a line break is still inserted in Opera - fix by recreating the textarea
          if (window.opera) {
            target.replaceWith('<textarea class="b-writemessage__area" cols="" rows=""></textarea>');
            createTextareaListener();
          }
        }
        return false;
      }
    });
  };

  createTextareaListener();
  userLinks.live('click', function(e) {
    var target = $(this);

    messageArea.val('@' + target.html() + ' ');
    messageArea.focus();

    return false;
  });

  roomLinks.live('click', function(e) {
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

    return false;
  });

  function login(e) {
    if (inited === false) return false;

    var loginName = $('#username');
    var loginRoom = $('#roomname :selected');

    var loginFieldContainer = loginName.parent();
    var loginErrorContainer = loginFieldContainer.find('.b-login__field__error');

    if (isValidUser(loginName.val())) {
      loginFieldContainer.removeClass('b-login__field_invalid');
      loginErrorContainer.addClass('g-hidden');
    } else {
      bChatLogin.effect('shake', {distance: 10, times: 2}, 35, function() {
        loginFieldContainer.addClass('b-login__field_invalid');
        loginErrorContainer.removeClass('g-hidden');
        loginName.focus();
      });

      return false;
    }

    currentName = transport.escapeHTML(loginName.val());
    currentRoom = loginRoom.attr('name');

    // Show chat window
    bChatLogin.addClass('g-hidden');
    bChatChat.removeClass('g-hidden').slideDown(1000, function() {
      $(this).height('auto');
    });

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

    messageArea.focus();

    return false;
  }

  function postMessage(type, params) {
    var messageObj = {
      username: transport.encodeMessage(currentName),
      room: currentRoom,
      type: type,
      text: (type == "message") ? transport.encodeMessage(messageArea.val()) : '',
      params: params
    };

    var message = 'cmd=' + 'msg chat/room_1 ' + JSON.stringify(messageObj);

    transport.postMessage(message);
  }

  function receiveMessage(messageObj) {
    if (messageObj === "ping\n") return;

    messageObj = JSON.parse(messageObj);

    var username = transport.decodeMessage(messageObj.username),
        room = messageObj.room,
        type = messageObj.type,
        text = transport.decodeMessage(messageObj.text),
        params = messageObj.params,
        date = formatDate(new Date());

    if (type == "message") {
      var message = $('<div class="b-message" style="opacity: 0.5">' +
                        '<div class="b-message__head">' +
                          '<div class="b-message__head__user">' + username + '</div>' +
                          '<div class="b-message__head__time">' + date + '</div>' +
                        '</div>' +
                        '<div class="b-message__body">' + transport.escapeHTML(text) + '</div>' +
                      '</div>');

      if (username == currentName) {
        message.addClass('b-message_you');
      }

      // Add message to the room it was sent to
      $('#messages_' + room).append(message);

      // Animate new message
      message.animate({ opacity: 1 }, 1000);
      bMessages.scrollTop(bMessages.attr("scrollHeight"));

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

    var idx = currentRoomUsers.indexOf && currentRoomUsers.indexOf(username);

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

  // Checks if a given username is already taken
  // Very limited usefulness due to lack of persistent storage
  function isValidUser(username) {
    username = $.trim(username);

    if (username == '') return false;

    var isDuplicate = false;

    for (var i in usernames) {
      for (var j=0; j<usernames[i].length; j++) {
        if (usernames[i][j] == username) {
          isDuplicate = true;
        }
      }
    }

    return !isDuplicate;
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

  function formatDate(date) {
    return date.getDate() + '.' + date.getMonth() + '.' + date.getFullYear() + ' ' + date.getHours() + ':' + date.getMinutes();
  }
});