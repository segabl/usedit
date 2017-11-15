/*
 * Signal.h
 *
 *  Created on: 12.11.2017
 *      Author: Sebastian
 */

#ifndef SRC_GUI_SIGNAL_H_
#define SRC_GUI_SIGNAL_H_

#include <functional>
#include <map>

class Signal {
  private:
    int slot_id;
    std::map<int, std::function<void()>> slots;
  public:
    Signal();
    int connect(std::function<void()> function);
    void disconnect(int id);
    void send();
};

#endif /* SRC_GUI_SIGNAL_H_ */
