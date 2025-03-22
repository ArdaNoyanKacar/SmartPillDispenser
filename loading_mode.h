#ifndef LOADING_MODE_H
#define LOADING_MODE_H

/**
 * @brief Run the loading mode UI
 * 
 * In loading mode, user selects which container (pill_type) to update
 * and then enters the number of pills that were added. The count entry
 * will roll over if it exceeds the missing number of pills in that container
 * 
 */
void loading_mode(void);

#endif