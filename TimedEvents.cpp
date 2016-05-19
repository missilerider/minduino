#include <Arduino.h>
#include "TimedEvents.h"

TimedEvents::TimedEvents()
{
	first = NULL;
	last = NULL;
}


TimedEvents::~TimedEvents()
{
}

void TimedEvents::updateFirst()
{
	unsigned long newMillis = millis();

	Event *E, *best = NULL;

	while (first)
	{
		if (first->timeout > newMillis) return;
		
		E = first;

		removeFirst();

		// Ejecuta llamada
		if (E->eventHandler)
			E->eventHandler(E);
	}
}

void TimedEvents::update()
{
	unsigned long newMillis = millis();

	Event *E = last, *best = NULL;
	byte currPrio = 255;
	unsigned long currMillis = -1; // MAX ULONG

	while (E)
	{
		if (E->priority <= currPrio)
		{
			currPrio = E->priority;
			currMillis = E->timeout;
			best = E;
		}

		E = E->prev;
	}

	if (E->timeout <= newMillis)
	{
		if (E->priority <= currPrio)
		{
			best = E;
			currPrio = E->priority;
		}
	}

	if (best)
	{
		currentPriority = best->priority;

		if (best->timeout > newMillis) return;

		if (best->eventHandler)
			best->eventHandler(best);
		
		remove(best);
	}
}

void TimedEvents::removeFirst()
{
	remove(first);

	updatePriority();
}

void TimedEvents::remove(Event *E)
{
	if (E->prev == NULL)
		first = E->next;
	else
		E->prev->next = E->next;
	
	if (E->next == NULL)
		last = E->prev;
	else
		E->next->prev = E->prev;

	delete E;
}

void TimedEvents::removeAll()
{
	while (first)
		removeFirst();

	currentPriority = 255;
}

void TimedEvents::Event::print()
{
	if (prev)
		Serial.print(prev->timeout);
	else
		Serial.print(F("NULL"));
	Serial.print(F(" <-- "));
	Serial.print(F("[ "));
	Serial.print(timeout);
	Serial.print(F(", "));
	Serial.print(priority);
	Serial.print(F(" ] "));
	Serial.print(F("--> "));
	if (next)
		Serial.println(next->timeout);
	else
		Serial.println(F("NULL"));
}

void TimedEvents::print()
{
	Serial.print(F("FIRST: "));
	if (first) Serial.println(first->timeout);
	else Serial.println(F("NULL"));

	Event *P = first;
	while (P)
	{
		P->print();
		P = P->next;
	}

	Serial.print(F("LAST: "));
	if (last) Serial.println(last->timeout);
	else Serial.println(F("NULL"));
}

void TimedEvents::addEvent(unsigned long waitMillis, void(*eventHandler)(Event*), void *userData, byte priority)
{
	unsigned long realWhen = millis() + waitMillis;

	Event *E = new Event();
	Event *P = last;
	Event *prev = NULL;

	while (P != NULL)
	{
		if (P->timeout <= realWhen)
		{
			prev = P;
			P = NULL;
		}
		else P = P->prev;
	}

	E->eventHandler = eventHandler;
	E->timeout = realWhen;
	E->priority = priority;
	E->userData = userData;
	E->prev = prev;

	if (prev != NULL)
	{
		E->next = prev->next;
		prev->next = E;
		if (E->next != NULL)
		{
			E->next->prev = E;
		}
		else
			last = E;
	}
	else
	{
		E->next = first;
		if (first != NULL)
		{
			first->prev = E;
			first = E;
		}
		else first = E;

		if (last == NULL)
		{
			last = E;
		}
	}

	if (priority < currentPriority) currentPriority = priority;
}

void TimedEvents::updatePriority()
{
	byte newPriority = MAXPRIORITY;
	Event *E = first;
	while (E)
	{
		if (E->priority < newPriority) newPriority = E->priority;
		E = E->next;
	}
}