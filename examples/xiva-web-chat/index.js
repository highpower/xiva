$(document).ready(function() {
  var bMessages = $('.b-messages');
  var messageArea = $('.b-writemessage__area');
  var messageSend = $('.b-writemessage__send');

  var userLinks = $('a.b-userlist__user');
  var roomLinks = $('.b-rooms__room a');

  messageArea.keydown(function(e) {
    if (e.keyCode == 13) {
      e.preventDefault();

      if (messageArea.val() != '') {
        postMessage();
        messageArea.val('');
      }
    }
  });

  messageSend.click(function(e) {
    e.preventDefault();
    postMessage();
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
    var message = $('<div class="b-message b-message_you" style="opacity: 0.5"><div class="b-message__head"><div class="b-message__head__user">Leonya</div><div class="b-message__head__time">Aug 31, 2010 15:35</div></div><div class="b-message__body">' + messageArea.val() + '</div></div>');
    bMessages.append(message);
    message.animate({ opacity: 1 }, 1000);
    bMessages.scrollTop(10000);
  }
});