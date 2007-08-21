#include "mremotecmd.h"

int fifo_open_read(char *fifoname) {
                                                                                
  int fd;
  int flags;
                                                                                
  fd=open(fifoname,O_RDONLY|O_NDELAY);
  if(fd<0) {
    return(fd);
  }
                                                                                
  flags=fcntl(fd,F_GETFL);
  if(flags < 0) {
    return(flags);
  }
  // clear the O_NDELAY bit so that subsequent
  // reads will be blocking
  flags &= ~O_NDELAY;
  flags=fcntl(fd,F_SETFL,flags);
  if(flags < 0) {
    return(flags);
  }
                                                                                
  return(fd);
}


ssize_t fifo_read(int fd, void *buf, size_t count) {
                                                                                
  fd_set rfds;
  struct timeval tv;
  int res;
  int pos;
                                                                                
  pos=0;
                                                                                
  FD_ZERO(&rfds);
  FD_SET(fd, &rfds);
  tv.tv_sec = 0;
  tv.tv_usec = 0;
//printf("before select read count =%d fd=%d\n",count,fd);
  res = select(fd+1, &rfds, NULL, NULL, &tv);
//printf("after select res =%d\n",res);
  if(res <= 0) return(res);
  if(FD_ISSET(fd, &rfds)) {
    while(count > 0) {
//printf("read while .... count =%d\n",count);
      res=read(fd,(char *)buf+pos,count);
//printf("partial read res =%d\n",res);
       if(res < 0) return(res);
      if(res == 0) return(pos);
      pos += res;
      count -= res;
    }
    return(pos);
  }
  return(0);
}
 
ssize_t fifo_write(int fd, void *buf, size_t count) {

  int res;
  int pos;

  pos=0;

  while(count > 0) {
    printf("write while .... count =%d\n",count);
    res=write(fd,(char *)buf+pos,count);
    printf("partial write res =%d\n",res);
    if(res < 0) return(res);
    if(res == 0) return(pos);
    pos += res;
    count -= res;
  }
  printf("fifo_write returning %d ....\n",pos);
  return(pos);
}

int fifo_open_write(char *fifoname) {
                                                                                
  int fd;
  int flags;
                                                                                
  fd=open(fifoname,O_WRONLY|O_NDELAY);
  if(fd<0) {
    return(fd);
  }
                                                                                
  flags=fcntl(fd,F_GETFL);
  if(flags < 0) {
    return(flags);
  }
  // clear the O_NDELAY bit so that subsequent
  // reads will be blocking
  flags &= ~O_NDELAY;
  flags=fcntl(fd,F_SETFL,flags);
  if(flags < 0) {
    return(flags);
  }
                                                                                
  return(fd);
}


// MESSAGE PASSING API

// opens a message port in server mode (creates the port)
// key is needed to identify the port by clients that
// will connect to

int mcmd_open_server_port(int key, mcmd_info_t *mcmd_info) {

	// just in case
	msgctl(key, IPC_RMID, 0);

  int msqid;
  mcmd_info->receive_mtype=key;
  msqid =msgget(key, IPC_CREAT | 0666);
  mcmd_info->msqid=msqid;
  return(msqid);
}
                                                                                                               

// opens a message port in client mode 
// key is the port identifier where to connect to

int mcmd_open_client_port(int key, mcmd_info_t *mcmd_info) {

  int msqid;

  // the receive message type is set to our own PID
  // this is needed to create an unique ID which we include
  // in the messages we send out to the server.
  // That way the server can send back a message with this id
  // and we (the client) wait only for messages with this id.
  // This avoid conflicts in the case there are multiple clients
  // accessing the same queue.

  mcmd_info->receive_mtype=getpid()+1000; // FIXME this hack
  // allows only server IDs from 0-999 (in order to keep unique receive_mtype)
  // perhaps we will find a better solution later

  msqid = msgget(key, 0666);
  mcmd_info->msqid=msqid;
  return(msqid);
}

int mcmd_send(mcmd_info_t *mcmd_info, void *buffer, int buflen, int mtype) {

    int res;

    mcmd_t *buf1=(mcmd_t *)buffer;
    buf1->mtype=mtype;
    // set the reply_mtype to our own receive_mtype;
    buf1->reply_mtype=mcmd_info->receive_mtype;

    res=msgsnd(mcmd_info->msqid, buffer, buflen, 0);
    return(res);
}

// receives a message. If no message is pending in the queue
// wait until message was received
int mcmd_receive(mcmd_info_t *mcmd_info, void *buffer, int buflen) {

  int res;
  int i;
  res=msgrcv(mcmd_info->msqid, buffer, buflen, mcmd_info->receive_mtype, 0);
  return(res);
}

// receives a message without waiting 
int mcmd_receive_nowait(mcmd_info_t *mcmd_info, void *buffer, int buflen) {

  int res;
  int i;
  res=msgrcv(mcmd_info->msqid, buffer, buflen, mcmd_info->receive_mtype, IPC_NOWAIT);
  return(res);
}
                                                                                                               

