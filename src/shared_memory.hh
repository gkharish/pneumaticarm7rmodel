#ifndef _PNEUMATIC_ARM_7R_SHARED_MEMORY_HH_
#define _PNEUMATIC_ARM_7R_SHARED_MEMORY_HH_

#define SHM_LOG_FILENAME "/var/log/pneumatic_arm.shm"
#define SEM_FILENAME "/pneumaticarmsem"

#include <string>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <signal.h>
#include <semaphore.h>

/** \brief This function creates a shared memory to control the INSA pneumatic ARM 
    It uses the file SHM_LOG_FILE_NAME with proj_id=2064.
    The map is 14 pressure values and 7 potentiometer values.
    If create is set to true then the system fails if the key already exists,
    otherwise it continues.
 */
double * CreateSharedMemoryForPneumaticArm(bool create);

class Semaphore
{
public:
  Semaphore();
  virtual ~Semaphore();

  void InitSemaphore(std::string &filename);

  // Try to acquire the release.
  void Acquire();

  // Release the shared memory.
  void Release();

protected:
  sem_t * semid_;
  std::string sem_filename_;
  
};
#endif
