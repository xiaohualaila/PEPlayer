#include "PanoDef.h"

void PanoComm_Init(struct PanoComm *comm)
{
    comm->keyword[0] = 'P';
    comm->keyword[1] = 'E';
    comm->keyword[2] = 'C';
    comm->keyword[3] = 'M';
}

bool PanoComm_Check(struct PanoComm *comm)
{
    if (comm->keyword[0] == 'P' && comm->keyword[1] == 'E'&& comm->keyword[2] == 'C'&& comm->keyword[3] == 'M')
        return true;
    return false;
}

unsigned int PanoComm_Size()
{
    return 12;
}
