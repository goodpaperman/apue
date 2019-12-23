int send_fd (int fd, int fd_to_send); 
int send_err (int fd, int status, char const* errmsg); 
int recv_fd (int fd, ssize_t (*userfunc)(int, const void *, size_t)); 
