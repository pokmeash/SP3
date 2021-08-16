#ifndef EVENT_HANDLER_H
#define EVENT_HANDLER_H

#include <functional>
#include <vector>
#include "Event.h"

#include "../DesignPatterns/SingletonTemplate.h"

struct Listener {
    std::function<void (Event*)> cb;
    bool ic;
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
    virtual void On(std::function<void(Event*)> callback) {
        if (!callback) return;

        auto* l = new Listener();
        l->cb = callback;
        l->ic = false;
        listeners.push_back(l);
    }

    virtual void IgnoreCancelOn(std::function<void(Event*)> callback) {
        if (!callback) return;

        auto* l = new Listener();
        l->cb = callback;
        l->ic = true;
        listeners.push_back(l);
    }

    virtual void Call(Event* e) {
        if (!e) return;
        for (unsigned i = 0; i < listeners.size(); ++i) {
            Listener* c = listeners[i];
            if (c->ic) {
                c->cb(e);
                continue;
            }

            Cancellable* ca = dynamic_cast<Cancellable*>(e);
            if (ca && !ca->isCancelled()) c->cb(e);
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
};

#endif