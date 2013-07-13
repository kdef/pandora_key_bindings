// Connect to the websocket server
var ws = new WebSocket('ws://127.0.0.1:9000', 'music_control');

ws.onopen = function () {
    console.log('Opened websocket connection!');
};

ws.onmessage = function (msg) {
    console.log('Received message: ' + msg.data);
    
    // find the Pandora tab
    chrome.tabs.query({title:'Pandora Internet Radio*'}, function (tabs) {
        if (tabs.length > 0) {
            // click the right button
            var id = tabs[0].id;
            if (msg.data == '/skip') {
                chrome.tabs.executeScript(id, {file:'skip.js'});
            } else if (msg.data == '/playpause') {
                chrome.tabs.executeScript(id, {file:'playpause.js'});
            }
        }
    }); 
};
