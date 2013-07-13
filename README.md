This is something I hacked together to control Pandora with global keyboard shortcuts.  It is very useful to be able to play/pause and skip songs when I am in applications other than my browser.

### How it works

There are two components to this probject.  A websocket server and a Chrome extension.  The websocket server listens for HTTP requests on localhost then sends messages to the Chrome extension over the websocket connection.  The extension listens on the websocket connection and interacts with Pandora via Javascript when it receives a message.

To send HTTP requests I just made keyboard shortcuts that execute curl.

Example:

* curl http://localhost:9000/skip
* Server writes the message "skip" to the websocket
* Chrome extension sees "skip" and executes some Javascript to skip the song on Pandora

### Running it

The websocket server is written in C and uses the libwebsockets library, which you can find at http://libwebsockets.org.  Once you have that installed just compile with:

`gcc ws_server.c /path/to/your/libwebsockets.so`

Start the server executable then load the extension in Chrome (turn on developer mode and load unpacked extension).

Then just visit http://localhost:9000/playpause or /skip to control Pandora.

### Credit

I'd like to give credit to Christian Cantrell who came up with this idea before me.  I stumbled upon his video http://www.youtube.com/watch?v=tDWPeQagPKo when I was researching for this project and although my implemention used different tools, his video was very helpful.
