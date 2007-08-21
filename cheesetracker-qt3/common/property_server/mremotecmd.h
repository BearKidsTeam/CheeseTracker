#ifndef MREMOTECMD_H
#define MREMOTECMD_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <fcntl.h>
#include <netinet/in.h>


// maximum size of a message that can be sent on a messageport
#define MSG_MAXSIZE 1024

typedef struct {
// message queue id needed for msg send and receive
                 int msqid;

// own message type, needed to receive only that kind of
// message on the queue (the queue can be multi client so we
// need a way to avoid reading our own messages we sent out as
// response)
                int receive_mtype;
              } mcmd_info_t;


int fifo_open_write(char *fifoname);
ssize_t fifo_write(int fd, void *buf, size_t count);
ssize_t fifo_read(int fd, void *buf, size_t count);
int fifo_open_read(char *fifoname);

int mcmd_open_server_port(int key, mcmd_info_t *mcmd_info);
int mcmd_open_client_port(int key, mcmd_info_t *mcmd_info);
int mcmd_send(mcmd_info_t *mcmd_info, void *buffer, int buflen, int mtype);
int mcmd_receive(mcmd_info_t *mcmd_info, void *buffer, int buflen);
int mcmd_receive_nowait(mcmd_info_t *mcmd_info, void *buffer, int buflen);







typedef struct {
                 long mtype;        // needed by SYS V msgsnd,msgrcv
                 long reply_mtype;  // needed for the server to set
                                    // the msgbuf.mtype in the reply message
                 short cmdtype;     // type of command
                 short datalength;  // length of the payload (can be 0)
                 // char data[1];      // an arbitrary amount of data following the header
                                    // number of bytes = datalength
} mcmd_t;


// list of command types (cmdtype field)

enum {
       MCMD_ACK,             // general acknowledge command sent in response by
                             // the server
                             // arguments: return value (long = 4 bytes)
			     // 0

       MCMD_MIDI_START_PLAY, // start MIDI playback
                             // takes no arguments, datalength 0 bytes
                             // return value: long (4 bytes)
                             // returns 0 if successful or a negative error value

       MCMD_MIDI_STOP_PLAY,  // stop MIDI playback
                             // takes no arguments, datalength 0 bytes
                             // return value: long (4 bytes)
                             // returns 0 if successful or a negative error value

       MCMD_MIDI_PAUSE_PLAY, // pause MIDI playback
                             // takes no arguments, datalength 0 bytes
                             // return value: long (4 bytes)
                             // returns 0 if successful or a negative error value

       MCMD_MIDI_REV,        // fast backward skipping
                             // arguments: long containing the MIDI ticks to skip
                             // datalength 4 bytes
                             // return value: long (4 bytes)
                             // returns 0 if successful or a negative error value

       MCMD_MIDI_FFWD,       // fast forward skipping
                             // arguments: long containing the MIDI ticks to skip
                             // datalength 4 bytes
                             // return value: long (4 bytes)
                             // returns 0 if successful or a negative error value
			     // 5

       MCMD_MIDI_SEEK_ABSOLUTE, // seeks the MIDI stream to an absolute position
                             // arguments: long containing the absolute position
                             // expressed in MIDI ticks
                             // return value: long (4 bytes)
                             // returns 0 if successful or a negative error value

       MCMD_MIDI_LOAD_FILE,  // loads a MIDI file into the player
                             // arguments string char[200] that contains the full
                             // path and filename
                             // return value: long (4 bytes)
                             // returns 0 if successful or a negative error value

       MCMD_MIDI_SET_TEMPO_FACTOR, // sets the value which the current MIDI tempo
                             // is multiplied to (default = 1.0)
                             // arguments: double (8 bytes) containing the tempo factor
                             // return value: long (4 bytes)
                             // returns 0 if successful or a negative error value

       MCMD_MIDI_SET_PITCH_SHIFTS, // sets the pitch shift offsets
                             // for all 16 channels
                             // arguments: array of 16 bytes (signed char)
                             // return value: long (4 bytes) 0 means ok

       MCMD_MIDI_SET_CONTROLLER, // sets the value of a controller
                             // arguments: see mcmd_midi_set_controller_t
                             // return value: long (4 bytes) 0 means ok
			     // 10

       MCMD_MIDI_ACTIVATE_CHANNEL, // activate/deactivate channel
                            // arguments: see mcmd_midi_activate_channel_t
                            // (channel, activate)
                            // return value: long (4 bytes) 0 means ok

       MCMD_MIDI_SEND_CURRENT_TICK, // indicates the current MIDI tick (eg player sends it
                              // regularly to the GUI)
                              // arguments: long (4 bytes) containing the current MIDI tick
                              // no return value

      MCMD_MIDI_GET_CONTROLLER_VALUES, // get the current values (16 channels) of a
                              // MIDI CC controller
                              // arguments: long (4 bytes) that indicates the controller
                              // (0-127) to read
                              // returns an array of 16 bytes containing the controller
                              // values for each channel

      MCMD_MIDI_CONTROLLER_VALUES, // message that contains the requested controller values
                             // payload: array of 16 bytes (controller value for each channel)

      MCMD_GET_PLAYER_STATUS, // fetches a structure that contain many status informations
                              // about the player
                              // takes no arguments
			      // 15

      MCMD_PLAYER_STATUS,    // this message contains the player status informations
                             // needed by a remote GUI
                             // see the structure mcmd_player_status_t

      MCMD_PERFORM_DIR,      // this message tells the server to list files and directories
                             // contained in the specified directory
                             // the file browse id in the server is incremented, this means
                             // that all clients will refresh their local list by requesting
                             // the list via MCMD_GENERATE_DIRFILE
                             // arguments char dirname[], see mcmd_perform_dir_t
                             // return message: ACK (0 = ok)

      MCMD_GENERATE_DIRFILE, // with this message the client requests the current file list
                             // (the 'dir' done by PERFORM_DIR) from the server
                             // arguments: clientid (usually the PID of the client)
                             // return value ACK (0 = ok)

	MCMD_GET_BRIDGE,
	MCMD_GET_PATH,          // 20
	MCMD_BRIDGE,
	MCMD_BRIDGE_GROUP,
	MCMD_PING,
	MCMD_PONG,
	MCMD_REGISTER,          // 25
	MCMD_UNREGISTER,
        MCMD_MAX,
     };

// ping? pong!
typedef struct {
	mcmd_t cmdheader;
        unsigned long timestamp;
} mcmd_ping_t;
typedef mcmd_ping_t mcmd_pong_t;
#define set_mcmd_ping(a,b) (a)->cmdheader.cmdtype=MCMD_PING; (a)->cmdheader.datalength=sizeof(mcmd_ping_t)-sizeof(mcmd_t); (a)->timestamp=htonl(b)
#define set_mcmd_pong(a,b) (a)->cmdheader.cmdtype=MCMD_PONG; (a)->cmdheader.datalength=sizeof(mcmd_pong_t)-sizeof(mcmd_t); (a)->timestamp=htonl(b)


typedef struct {
                 mcmd_t cmdheader;
                 char dirname[200];
               } mcmd_perform_dir_t;

typedef struct {
                 mcmd_t cmdheader;
                 int clientid;
               } mcmd_generate_dirfile_t;


typedef struct {
                 mcmd_t cmdheader;
                 long position;
               } mcmd_midi_seek_absolute_t;

typedef struct {
                 mcmd_t cmdheader;
                 char pitch_shift_arr[16];
               } mcmd_midi_pitch_shifts_t;

typedef struct {
                 mcmd_t cmdheader;
                 char channel;
                 char controller;
                 char value;
                 char padbyte;
               } mcmd_midi_set_controller_t;

typedef struct {
                 mcmd_t cmdheader;
                 short channel;
                 short activate;
               } mcmd_midi_activate_channel_t;
                                                                                  


typedef struct {
                 mcmd_t cmdheader;
                 char filename[200];
               } mcmd_midi_load_file_t;



typedef struct {
                 mcmd_t cmdheader;
                 double tempo_factor;
               } mcmd_midi_tempo_factor_t;

typedef struct {
                 mcmd_t cmdheader;
                 long controller;
               } mcmd_midi_get_controller_values_t;

typedef struct {
                 mcmd_t cmdheader;
                 char controller_value_arr[16];
               } mcmd_midi_controller_values_t;

typedef struct {
                 mcmd_t cmdheader;
                 long playing_active;
                 long paused;
                 double tempo_factor; 
                 long current_miditick; // current midi tick (playback position)
                 long song_len; // song len in ticks
                 long divisions; // divisions 
                 long microtempo; // current microtempo 
                 short timesig_numerator;
                 short timesig_denominator;
                 long chan_offset;
                 char channel_volume_arr[16]; // channel MIDI volume (CC 7)
                 char channel_pan_arr[16]; // channel MIDI panorama
                 char channel_pitch_shift_arr[16]; // channel pitch shift 
                 char channel_active_arr[16]; // channels on/off
               } mcmd_player_status_t;

// mcmd_ack_t is an acknowledge command sent by some commands
// it contains only the return value

typedef struct {
                 mcmd_t cmdheader;
                 long retval;
               } mcmd_ack_t;

#define set_mcmd_midi_seek_absolute(a,b) (a)->cmdheader.cmdtype=MCMD_MIDI_SEEK_ABSOLUTE; (a)->cmdheader.datalength=sizeof(mcmd_midi_seek_absolute_t)-sizeof(mcmd_t); (a)->position=b

#define set_mcmd_ack(a,b) (a)->cmdheader.cmdtype=MCMD_ACK; (a)->cmdheader.datalength=sizeof(mcmd_ack_t)-sizeof(mcmd_t); (a)->retval=htonl(b)

#define set_mcmd_midi_start_play(a) (a)->cmdtype=MCMD_MIDI_START_PLAY; (a)->datalength=0
#define set_mcmd_midi_stop_play(a) (a)->cmdtype=MCMD_MIDI_STOP_PLAY; (a)->datalength=0
#define set_mcmd_midi_pause_play(a) (a)->cmdtype=MCMD_MIDI_PAUSE_PLAY; (a)->datalength=0

#define set_mcmd_midi_get_controller_values(a,b) (a)->cmdheader.cmdtype=MCMD_MIDI_GET_CONTROLLER_VALUES; (a)->cmdheader.datalength=sizeof(mcmd_midi_get_controller_values_t)-sizeof(mcmd_t); (a)->controller=b

#define set_mcmd_midi_controller_values(a,b) (a)->cmdheader.cmdtype=MCMD_MIDI_CONTROLLER_VALUES; (a)->cmdheader.datalength=sizeof(mcmd_midi_controller_values_t)-sizeof(mcmd_t); memcpy((a)->controller_value_arr,b,16)


#define set_mcmd_get_player_status(a) (a)->cmdtype=MCMD_GET_PLAYER_STATUS; (a)->datalength=0

#define set_mcmd_set_tempo_factor(a,b) (a)->cmdheader.cmdtype=MCMD_MIDI_SET_TEMPO_FACTOR; (a)->cmdheader.datalength=sizeof(mcmd_midi_tempo_factor_t)-sizeof(mcmd_t); (a)->tempo_factor=b

#define set_mcmd_midi_load_file(a,b) (a)->cmdheader.cmdtype=MCMD_MIDI_LOAD_FILE; (a)->cmdheader.datalength=sizeof(mcmd_midi_load_file_t)-sizeof(mcmd_t); strncpy((a)->filename,b,199); (a)->filename[199]=0

#define set_mcmd_midi_pitch_shifts(a,b) (a)->cmdheader.cmdtype=MCMD_MIDI_SET_PITCH_SHIFTS; (a)->cmdheader.datalength=sizeof(mcmd_midi_pitch_shifts_t)-sizeof(mcmd_t); memcpy((a)->pitch_shift_arr, b, 16)

#define set_mcmd_midi_activate_channel(a,b,c) (a)->cmdheader.cmdtype=MCMD_MIDI_ACTIVATE_CHANNEL; (a)->cmdheader.datalength=sizeof(mcmd_midi_activate_channel_t)-sizeof(mcmd_t); (a)->channel=b; (a)->activate=c

#define set_mcmd_midi_set_controller(a,b,c,d) (a)->cmdheader.cmdtype=MCMD_MIDI_SET_CONTROLLER; (a)->cmdheader.datalength=sizeof(mcmd_midi_set_controller_t)-sizeof(mcmd_t); (a)->channel=b; (a)->controller=c; (a)->value=d


#define set_mcmd_perform_dir(a,b) (a)->cmdheader.cmdtype=MCMD_PERFORM_DIR; (a)->cmdheader.datalength=sizeof(mcmd_perform_dir_t)-sizeof(mcmd_t); strncpy((a)->dirname,b,200)

#define set_mcmd_generate_dirfile(a,b) (a)->cmdheader.cmdtype=MCMD_GENERATE_DIRFILE; (a)->cmdheader.datalength=sizeof(mcmd_generate_dirfile_t)-sizeof(mcmd_t); (a)->clientid=b



#endif
