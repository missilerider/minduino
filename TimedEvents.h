#pragma once

#define MAXPRIORITY		255

class TimedEvents
{
public:
	class Event
	{
	public:
		unsigned long timeout;
		byte priority;
		void *userData;
		void(*eventHandler)(Event*);

		Event *next, *prev;

	public:
		void print();
	};

public:
	TimedEvents();
	~TimedEvents();

	void updateFirst();
	void update();
	void addEvent(unsigned long waitMillis, void(*eventHandler)(Event*), void *userData, byte priority);
	void removeFirst();
	void remove(Event *E);
	void removeAll();
	void print();

public:
	Event *first, *last;

	byte currentPriority = MAXPRIORITY;

private:
	void updatePriority();
};

