/* ws_server.c
 * 
 * WebSocket server for Pandadora Global Hotkeys.  Listens on port 9000, 
 * parses URLs and sends the appropriate message to the Chrome extension which
 * is connected as a client.
 * 
 * Author: Kyle DeFrancia
 * Date: 7/12/2013
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <libwebsockets.h>

struct libwebsocket * extension = NULL;
int run = 1;

// handle signals so we can stop the server
void sighandler(int signum) {
    printf("Signal intercepted, shutting down...");
    run = 0;
}

// handle a HTTP request - this is what the shortcut keys will trigger
static int callback_http(struct libwebsocket_context * this,
                         struct libwebsocket * wsi,
                         enum libwebsocket_callback_reasons reason,
                         void * user, void * in, size_t len)  
{
    switch (reason) {
        case LWS_CALLBACK_CLIENT_WRITEABLE:
            printf("HTTP connection established!\n");

        case LWS_CALLBACK_HTTP: {
            printf("Message received: %s\n", (char *)in);
            
            //call function to send message to extension
            if (extension != NULL) {
                printf("Sending message to extension\n");
                
                //add padding bytes to our message
                unsigned char* buf  = (unsigned char*) malloc(LWS_SEND_BUFFER_PRE_PADDING + len + LWS_SEND_BUFFER_POST_PADDING);
                memcpy(&buf[LWS_SEND_BUFFER_PRE_PADDING], in, len);
                libwebsocket_write(extension, &buf[LWS_SEND_BUFFER_PRE_PADDING], len, LWS_WRITE_TEXT);
                //give that space back
                free(buf);
            } else {
                printf("No extension connected!\n");
            }
            //kill the connection
            return 1;
        }
        default: break;        
    }
    
    return 0;
}

// handle a music_control request
static int callback_music_control(struct libwebsocket_context * this,
                                  struct libwebsocket * wsi,
                                  enum libwebsocket_callback_reasons reason,
                                  void * user, void * in, size_t len)
{
    // we only care that a client has connected, nothing else
    if (reason == LWS_CALLBACK_ESTABLISHED) {
        extension = wsi;
        printf("Chrome extension connection established!\n");
    }

    return 0;
}

//the protocols this websocket server will handle
static struct libwebsocket_protocols protocols[] = {
    { "http-only", callback_http, 0},
    { "music_control", callback_music_control, 0},
    { NULL, NULL, 0 } //terminator
};

// start a websocket server
int main(void)
{
    signal(SIGINT, sighandler);

    // context parameters
    struct lws_context_creation_info info;
    info.port = 9000;               //listen on port 9000
    info.iface = NULL;              //listen on all interfaces
    info.protocols = protocols;     //protocols we will support
    info.extensions = libwebsocket_get_internal_extensions();
    info.ssl_cert_filepath = NULL;  //no SSL
    info.ssl_private_key_filepath = NULL;
    info.ssl_ca_filepath = NULL;
    info.gid = -1;                  //do not change group id
    info.uid = -1;                  //do not change user id
    info.options = 0;               //no special options
    info.ka_time = 0;               //no keepalive

    struct libwebsocket_context * context = libwebsocket_create_context(&info);

    if (context == NULL) {
        fprintf(stderr, "Failed to initialize websocket server context!\n");
        return -1;
    }

    printf("Starting websocket server...\n");

    // server will sit in infinte loop until it is terminated
    // send SIGTERM to exit
    while (run) {
        //process all waiting events every 50ms
        libwebsocket_service(context, 50);
    }

    // clean up before we exit
    libwebsocket_context_destroy(context);
    printf(" All cleaned up.  Good bye.\n");
    return 0;
}
