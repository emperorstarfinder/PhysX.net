#include "StdAfx.h"
#include "Aggregate.h"
#include "Scene.h"
#include "Actor.h"
#include "Articulation.h"
#include "Serializable.h"
#include <PxAggregate.h> 

Aggregate::Aggregate(PxAggregate* aggregate, PhysX::Scene^ owner)
{
	if (aggregate == NULL)
		throw gcnew ArgumentNullException("aggregate");
	ThrowIfNullOrDisposed(owner, "owner");

	_aggregate = aggregate;
	_scene = owner;

	ObjectTable::Add((intptr_t)aggregate, this, owner);
}
Aggregate::~Aggregate()
{
	this->!Aggregate();
}
Aggregate::!Aggregate()
{
	OnDisposing(this, nullptr);

	if (this->Disposed)
		return;

	_aggregate->release();
	_aggregate = NULL;

	_scene = nullptr;

	OnDisposed(this, nullptr);
}

bool Aggregate::Disposed::get()
{
	return (_aggregate == NULL);
}

bool Aggregate::AddActor(Actor^ actor)
{
	ThrowIfNullOrDisposed(actor, "actor");

	return _aggregate->addActor(*actor->UnmanagedPointer);
}

bool Aggregate::RemoveActor(Actor^ actor)
{
	ThrowIfNullOrDisposed(actor, "actor");

	return _aggregate->removeActor(*actor->UnmanagedPointer);
}

bool Aggregate::AddArticulation(Articulation^ articulation)
{
	ThrowIfNullOrDisposed(articulation, "articulation");

	return _aggregate->addArticulation(*articulation->UnmanagedPointer);
}

bool Aggregate::RemoveArticulation(Articulation^ articulation)
{
	ThrowIfNullOrDisposed(articulation, "articulation");

	return _aggregate->removeArticulation(*articulation->UnmanagedPointer);
}

Actor^ Aggregate::GetActor(int index)
{
	int actorCount = _aggregate->getNbActors();

	if (index < 0 || index >= actorCount)
		throw gcnew ArgumentOutOfRangeException(String::Format("The index argument must be between 0 and the number of actors (currently {0})", actorCount));

	PxActor** actors = new PxActor*[1];

	_aggregate->getActors(actors, 1, index);

	return ObjectTable::GetObject<Actor^>((intptr_t)actors[0]);
}
array<Actor^>^ Aggregate::GetActors()
{
	int actorCount = _aggregate->getNbActors();
	PxActor** actors = new PxActor*[actorCount];

	_aggregate->getActors(actors, actorCount, 0);

	return ObjectTable::GetObjects<Actor^>((intptr_t*)actors, actorCount);
}

Serializable^ Aggregate::AsSerializable()
{
	return gcnew Serializable(_aggregate);
}

//

PhysX::Scene^ Aggregate::Scene::get()
{
	return _scene;
}

int Aggregate::MaximumNumberOfActors::get()
{
	return _aggregate->getMaxNbActors();
}

int Aggregate::NumberOfActors::get()
{
	return _aggregate->getNbActors();
}

bool Aggregate::SelfCollision::get()
{
	return _aggregate->getSelfCollision();
}

PxAggregate* Aggregate::UnmanagedPointer::get()
{
	return _aggregate;
}