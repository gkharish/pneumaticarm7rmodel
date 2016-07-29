#include <iostream>
#include <fcntl.h>
#include <sys/stat.h>
#include <semaphore.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <stdlib.h>
#include </home/user/catkin_ws/src/pneumaticarm7rmodel/src/shared_memory.hh>
//#include <shared_memory.hh>

struct semid_ds
{
  struct  ipc_perm sem_per; /* operation's permission structure */
  struct  sem * sem_base; /* pointer to first sem in a set */
  ushort sem_nsems; /* number of sem in a set */
  time_t sem_otime; /*time of last semop */
  time_t sem_ctime; /* time of last change */
};

int chshm(int shmid, uid_t uid, gid_t gid, int mode)
{
	int ret;
	struct shmid_ds buf;

	//save current stat
	ret = shmctl(shmid, IPC_STAT, &buf);
	if(ret == -1 ){
		perror("error get shared memory info");
		return -1;
	}	

	// if uid or gid is -1, it will not be changed
	if( uid != -1 ){
		buf.shm_perm.uid = uid;
	}
	if( gid != -1 ){
		buf.shm_perm.gid = gid;
	}
	
	if(mode != -1){
		buf.shm_perm.mode = mode;
	}
	
	ret = shmctl(shmid, IPC_SET, &buf);
	if( ret == -1){
		perror("error: unable to change owner");
		return -1;
	}
	return 0;
}

double * CreateSharedMemoryForPneumaticArm(bool create)
{

  // Create the key related to the file.
  key_t key_to_shm = ftok(SHM_LOG_FILENAME,2061);
  int shmflg=SHM_R | SHM_W;
  if (create) 
    shmflg = IPC_CREAT | SHM_R | SHM_W;
  
  // Create the memory shared link with the key.
  int shmid;
  uid_t uid =1000;
  gid_t gid =1001 ;
  mode_t mode = 0664;

  if ((shmid = shmget(key_to_shm, 16*sizeof(double)+7*sizeof(double)+sizeof(int)+sizeof(double), 
                       shmflg)) < 0)
    {
      shmflg = SHM_R ;
      if ((shmid = shmget(key_to_shm, 16*sizeof(double)+7*sizeof(double)+sizeof(int)+sizeof(double), 
			  shmflg)) < 0)
	{
	  perror("Pb with the shared memory.");
	  exit(-1);
	}
    }
  
  // Attached the shared memory to a memory segment.
  double *shmaddr = (double *)shmat(shmid, 0,0);
 
  return shmaddr;
}


Semaphore::Semaphore()
{
  std::string filename(SEM_FILENAME);
  Semaphore::InitSemaphore(filename);
}

void Semaphore::InitSemaphore(std::string & filename)
{
  /**
   * Semaphore open
   */
  sem_filename_ = filename;
  semid_=NULL;
  //sem_unlink(sem_filename_.c_str());
  semid_=sem_open(sem_filename_.c_str(), O_CREAT,  0664, 1);
  if (semid_==SEM_FAILED)
    { 
      std::cerr << "File " << sem_filename_ << " for shared memory semaphore." <<std::endl;
      perror("Error while creating the semaphore for the shared memory\n");
    }
  
      
}


Semaphore::~Semaphore()
{
  sem_close(semid_);
  sem_unlink(sem_filename_.c_str());
}

void Semaphore::Acquire()
{
  int r=0;
  if ((r=sem_trywait(semid_))<0)
    {
      //std::cerr << "Unble to acquire the semaphore."<<std::endl;
    }
}

void Semaphore::Release()
{
  sem_post(semid_);
}


