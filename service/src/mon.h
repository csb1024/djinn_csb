#ifndef MON_H
#define MON_H



// the following returns 1 if successful, 0 if not
int initNVML();
int initMonitor(int gpuid, nvmlDevice_t *gpu0); // initialize 
void  getUtilization(nvmlDevice_t gpu0, float util_info[4]); //returns array of info
int endNVML(); //also includes freeing memory

#endif 
