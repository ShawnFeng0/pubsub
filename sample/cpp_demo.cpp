//
// Created by fs on 2/23/20.
//

#include <ulog/ulog.h>
#include <unistd.h>

#include "../base/condition_variable.hpp"
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
  for (int i = 0; i < 10; i++) {
    pub.get().a += 1;
    pub.get().b += 10;
    pub.get().c += 100;
    pub.get().d += 1000;
    pub.PublishChange();
    sleep(1);
  }
  return nullptr;
}

void *SubThread(void *) {
  PubSub::MonoClockSemaphore sem(0);
  PubSub::SubscriptionData<data> sub(
      [](void *data) { ((PubSub::MonoClockSemaphore *)data)->release(); }, &sem,
      true);

  for (int i = 0; i < 10; i++) {
    // first update
    if (sub.Update()) {
      LOGGER_TOKEN(sub.get().a);
      LOGGER_TOKEN(sub.get().b);
      LOGGER_TOKEN(sub.get().c);
      LOGGER_TOKEN(sub.get().d);
    } else {
      LOGGER_INFO("Not updated, waiting");
    }
    sem.acquire();  // Use Update () first to avoid not being notified of the
                    // first update
  }
  return nullptr;
}

int main(int argc, char *argv[]) {
  pthread_t tid;
  pthread_create(&tid, nullptr, PubThread, nullptr);
  pthread_create(&tid, nullptr, SubThread, nullptr);
  pthread_create(&tid, nullptr, SubThread, nullptr);
  pthread_create(&tid, nullptr, SubThread, nullptr);
  pthread_exit(nullptr);
}
