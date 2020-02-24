//
// Created by fs on 2/23/20.
//

#include <ulog/ulog.h>
#include <unistd.h>

#include "../condition_variable.hpp"
#include "../publication.hpp"
#include "../subscription.hpp"

struct data {
  int a;
  int b;
  int c;
  int d;
};

void *PubThread(void *) {
  PubSub::PublicationData<data> pub{};
  LOG_TOKEN(sizeof(pub));
  for (int i = 0; i < 10; i++) {
    pub.get().a += 1;
    pub.get().b += 10;
    pub.get().c += 100;
    pub.get().d += 1000;
    pub.Publish();
    sleep(1);
  }
  return nullptr;
}

void *SubThread(void *) {
  PubSub::Subscription<data> sub{};
  PubSub::MonoClockSemaphore sem(0);
  sub.RegisterCallback([&]() { sem.release(); });
  LOG_TOKEN(sizeof(sub));
  LOG_TOKEN(sizeof(sem));
  int i = 0;
  while (i < 10) {
    if (sub.Update()) {
      i++;
      LOG_TOKEN(sub.get().a);
      LOG_TOKEN(sub.get().b);
      LOG_TOKEN(sub.get().c);
      LOG_TOKEN(sub.get().d);
    } else {
      LOG_INFO("Not updated, waiting");
      sem.acquire();
    }
  }
  return nullptr;
}

int main(int argc, char *argv[]) {
  LOG_INFO("test");
  pthread_t tid;
  pthread_create(&tid, nullptr, PubThread, nullptr);
  pthread_create(&tid, nullptr, SubThread, nullptr);
  pthread_create(&tid, nullptr, SubThread, nullptr);
  pthread_create(&tid, nullptr, SubThread, nullptr);
  pthread_exit(nullptr);
}
