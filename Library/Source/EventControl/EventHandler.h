#ifndef EVENT_HANDLER_H
#define EVENT_HANDLER_H

#include <functional>
#include <vector>
#include <iostream>
#include "Event.h"

#include "../DesignPatterns/SingletonTemplate.h"

struct Listener {
    std::function<void (Event*)> cb;
    bool ic = false;
};

class EventHandler : public CSingletonTemplate<EventHandler> {
    friend CSingletonTemplate<EventHandler>;
protected:
    std::vector<Listener*> listeners;

    EventHandler() {}

    virtual ~EventHandler() {
        Clear();
    }
public:
    virtual Listener* On(std::function<void(Event*)> callback) {
        if (!callback) return nullptr;

        auto* l = new Listener();
        l->cb = callback;
        l->ic = false;
        listeners.push_back(l);
        return l;
    }

    virtual Listener* IgnoreCancelOn(std::function<void(Event*)> callback) {
        if (!callback) return nullptr;

        auto* l = new Listener();
        l->cb = callback;
        l->ic = true;
        listeners.push_back(l);
        return l;
    }

    virtual void Call(Event* e) {
        if (!e) return;
        Cancellable* ca = dynamic_cast<Cancellable*>(e);
        for (unsigned i = 0; i < listeners.size(); ++i) {
            Listener* c = listeners[i];
            if (ca && ca->isCancelled()) {
                if (c->ic) c->cb(e);
                continue;
            }
            c->cb(e);
        }
    }

    virtual void CallThenDelete(Event* e) {
        Call(e);
        if (e) delete e;
    }

    virtual bool CallDeleteIsCancelled(Event* e) {
        Call(e);
        Cancellable* ca = dynamic_cast<Cancellable*>(e);
        bool cancelled = false;
        if (ca) {
            cancelled = ca->isCancelled();
        }
        delete e;
        return cancelled;
    }

    virtual void Clear() {
        for (unsigned i = 0; i < listeners.size(); ++i) {
            Listener* c = listeners[i];
            delete c;
        }
        listeners.clear();
    }

    virtual void Remove(Listener* listener) {
        if (listener == nullptr) return;
        for (auto it = listeners.begin(); it != listeners.end(); it++) {
            Listener* c = *it;
            if (listener != c) continue;
            delete c;
            listeners.erase(it);
            break;
        }
    }
};

#endif