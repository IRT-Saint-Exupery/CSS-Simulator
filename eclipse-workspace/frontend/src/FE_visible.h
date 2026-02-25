#ifndef SRC_FE_VISIBLE_H_
#define SRC_FE_VISIBLE_H_

#include <vector>
#include <functional>

// Reads the /tmp/visiX.txt files and returns vector of 0/1 values
std::vector<int> getVisi();

void runVisiMonitorForever(int nSat);
int get_NSat_value_from_script(const char *filename);

#endif /* SRC_FE_VISIBLE_H_ */
