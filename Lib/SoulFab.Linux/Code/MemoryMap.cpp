#include "MemoryMap.hpp"

include <sys/types.h>
include <sys/ipc.h>
include <sys/shm.h>

namespace SoulFab::Link
{
    MemoryMap::MemoryMap()
        : Address(nullptr)
    {

    }

    MemoryMap::~MemoryMap()
    {
        this->Close();
    }

    void MemoryMap::Create(unsigned long size, char* name, char* file)
    {
        //    Check parameter
        if (NULL != pAddress || -1 != hMapping) return -1;

        //    Get share memory id
        //int iFlags = SHM_R|SHM_W|IPC_CREAT|IPC_EXCL;
        hMapping = shmget(name, Length, IPC_CREAT);
        if (-1 == hMapping) { return -2; }

        // Set access mode
        //struct shmid_ds shmds;
        //shmctl(hMapping , IPC_STAT , &shmds );
        //shmds.shm_perm.mode |= 0x1B4; /* rw-rw-r-- */
        //shmctl(hMapping , IPC_SET , &shmds );

        // Attach share memory
        pAddress = shmat(hMapping, 0, 0);
        if (NULL != pAddress)
        {
            nAccess = -1;
            return 0;
        }

        //    Remove share memory id
        int hShare = hMapping; hMapping = -1;
        return shmctl(hShare, IPC_RMID, 0);
    }

    void MemoryMap::Open(char* name)
    {
        //    Check parameter
        if (NULL != pAddress || -1 != hMapping) return -1;

        //    Open share memory
        nAccess = Write ? SHM_R : SHM_R | SHM_W;
        hMapping = shmget(Key, 0, nAccess);
        if (-1 == hMapping) { return -2; }

        //    Attach share memory
        pAddress = shmat(hMapping, 0, 0);
        return pAddress ? 0 : -3;
    }

    void MemoryMap::Close()
    {
        //    Check Parameter
        if (NULL == pAddress || -1 == hMapping) return -1;
        if (-1 == nAccess) { return Delete(); }

        //    Share memory detech
        void* pAddr = pAddress; 
        pAddress = NULL;
        hMapping = -1; 
        nAccess = 0;

        return shmdt(pAddr);
    }

    FileHandle MemoryMap::CreateMapFiel(char* file)
    {

    }

    void* MemoryMap::Get()
    {
        return this->Address;
    }

    void MemoryMap::Delete()
    {
        //    Check parameter
        if (NULL == pAddress || -1 == hMapping) return -1;
        if (-1 != nAccess) { return Close(); }

        // Detech and remove share memory
        shmdt(pAddress); 
        pAddress = NULL;
        
        shmctl(hMapping, IPC_RMID, 0);
        hMapping = -1; 
        nAccess = 0;
    }
}